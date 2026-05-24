#include "precomp.h"
#include "chafa_wrapper.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>

#ifdef _MSC_VER
#define popen _popen
#define pclose _pclose
#endif

// Check if chafa CLI is available
bool chafa_cli_available()
{
    static int available = -1;
    if (available != -1)
        return available;

    // Try to run "chafa --version" to check if it exists
#ifdef _MSC_VER
	FILE* fp = popen("chafa --version 2", "r");
#else
    FILE* fp = popen("chafa --version 2>/dev/null", "r");
#endif
    if (fp) {
        // Must read all output before pclose
        char buf[256];
        while (fgets(buf, sizeof(buf), fp)) {}
        int status = pclose(fp);
        available = (status == 0) ? 1 : 0;
    } else {
        available = 0;
    }
    return available;
}

bool chafa_available()
{
    return chafa_cli_available();
}


// RAII wrapper for FILE*
struct PipeDeleter {
    void operator()(FILE* fp) const {
        if (fp) pclose(fp);
    }
};
using PipePtr = std::unique_ptr<FILE, PipeDeleter>;

// Get terminal size
static std::pair<int, int> get_terminal_size()
{
#if _MSC_VER
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int width = 80, height = 24; // Default fallback
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
    {
        return { ws.ws_col, ws.ws_row };
    }
    // Fallback
    const char* cols = getenv("COLUMNS");
    const char* rows = getenv("LINES");
    int width = cols ? atoi(cols) : 80;
    int height = rows ? atoi(rows) : 24;
#endif
    return { width, height };
}

bool display_image_chafa(const std::string& image_path, int max_width, int max_height)
{
    if (!chafa_cli_available())
        return false;

    // If dimensions not specified, use terminal size
    if (max_width <= 0 || max_height <= 0) {
        auto [term_width, term_height] = get_terminal_size();
        if (max_width <= 0) max_width = term_width - 4;
        if (max_height <= 0) max_height = term_height - 8;
    }

    // Build chafa command
    // --format symbols uses high-res unicode symbols
    // --color-space din99d gives better color reproduction
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
#ifndef _MSC_VER
        "chafa --format symbols --color-space din99d --size %dx%d \"%s\" 2>/dev/null",
#else
        "chafa --color-space din99d --format sixels --size %dx%d \"%s\" 2>nul",
#endif
        max_width, max_height, image_path.c_str());

    // Run chafa and capture output
    PipePtr fp(popen(cmd, "r"));
    if (!fp)
        return false;

    // Center the output
    auto [term_width, term_height] = get_terminal_size();

    char line[4096];
    std::vector<std::string> lines;
    int max_line_width = 0;

    while (fgets(line, sizeof(line), fp.get())) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
            line[len-1] = '\0';

        lines.emplace_back(line);
        if ((int)strlen(line) > max_line_width)
            max_line_width = strlen(line);
    }

    int status = pclose(fp.release());
    if (status != 0)
        return false;

    // Calculate padding to center
    int padding = (term_width - max_line_width) / 2;
    if (padding < 0) padding = 0;
    std::string pad_str(padding, ' ');

    // Print with centering
    for (const auto& l : lines) {
        std::cout << pad_str << l << "\n";
    }

    return true;
}
