#include "precomp.h"
#include "ascii_art.h"
#include "chafa_wrapper.h"
#include "raylib_wrapper.h"
#include "raylib_console.h"
#include "globals.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// STB image library
#if defined(_MSC_VER) || defined(__APPLE__)
#define STB_IMAGE_IMPLEMENTATION
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include "stb_image.h"

namespace fs = std::filesystem;

// ASCII characters from dark to light (more detailed set)
const char* ASCII_CHARS = " .'`^\",:;Il!i~<>+_-?][}{1)(|\\//tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const int NUM_ASCII_CHARS = 70;

// Get terminal size
std::pair<int, int> get_terminal_size()
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
    // Fallback to environment variables
    const char* cols = getenv("COLUMNS");
    const char* rows = getenv("LINES");
    int width = cols ? atoi(cols) : 80;
    int height = rows ? atoi(rows) : 24;
#endif
    return { width, height };
}

// Try to find an image file for the room ID
std::optional<fs::path> find_image_file(const std::string& room_id)
{
    const char* extensions[] = { ".png", ".jpg", ".jpeg", ".jfif", ".gif", ".bmp" };

    // Check in zork_pics directory
    fs::path pics_dir = "zork_pics";

    // Try lowercase version of room ID
    std::string lower_id = room_id;
    std::transform(lower_id.begin(), lower_id.end(), lower_id.begin(), ::tolower);

    for (const auto& ext : extensions)
    {
        fs::path img_path = pics_dir / (lower_id + ext);
        if (fs::exists(img_path))
        {
            return img_path;
        }
    }

    return std::nullopt;
}

// ANSI color escape code helper
std::string rgb_color(int r, int g, int b, bool foreground = true)
{
    int code = foreground ? 38 : 48; // 38 = foreground, 48 = background
    return "\033[" + std::to_string(code) + ";2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

const std::string RESET_COLOR = "\033[0m";

// Convert image to ASCII art and display it with color
bool display_room_image(const std::string& room_id)
{
    auto img_path_opt = find_image_file(room_id);
    if (!img_path_opt)
    {
        std::cerr << "No image found for room: " << room_id << std::endl;
        return false;
    }

    fs::path img_path = *img_path_opt;
    //std::cerr << "Found image for room " << room_id << ": " << img_path << std::endl;

    // Try GUI console first if enabled
    if (flags[FlagId::gui_mode] && raylib_console_available())
    {
        std::cerr << "Setting image in GUI console" << std::endl;
        raylib_console_set_image(img_path.string());
        return true;
    }

    // Try GUI window if enabled (fallback for when console isn't available)
    if (flags[FlagId::gui_mode] && raylib_available())
    {
        char result = raylib_display_image_interactive(img_path.string());
        return true;
    }

    // Try Chafa for terminal graphics
    if (chafa_available())
    {
        auto [term_width, term_height] = get_terminal_size();
        const int margin = 4;
        int max_width = term_width - margin;
        int max_height = term_height - 12;

        // Change to 32x16 to try to regulate the size, since AI images can be all over the place.
        max_width = 32;
        max_height = 16;

        if (display_image_chafa(img_path.string(), max_width, max_height))
        {
            return true;
        }
        // Fall through to ASCII art if Chafa fails
    }

    // Load image using stb_image (request 3 channels for RGB)
    int width, height, channels;
    unsigned char* data = stbi_load(img_path.string().c_str(), &width, &height, &channels, 3);

    if (!data)
    {
        return false;
    }

    // Get terminal size
    auto [term_width, term_height] = get_terminal_size();

    // Leave some margin for borders and text
    const int margin = 4;
    const int max_width = term_width - margin;
    // Use most of the terminal height, but leave room for description text
    const int max_height = term_height - 12;

    // Calculate scaling to fill terminal while maintaining aspect ratio
    // Characters are roughly 2x as tall as they are wide, so we adjust for that
    double scale_x = static_cast<double>(width) / max_width;
    double scale_y = static_cast<double>(height) / (max_height * 2); // Multiply by 2 for char aspect ratio
    double scale = std::max(scale_x, scale_y);

    if (scale < 1.0) scale = 1.0;

    int new_width = static_cast<int>(width / scale);
    int new_height = static_cast<int>(height / scale / 2); // Divide by 2 for character aspect ratio

    if (new_width < 1) new_width = 1;
    if (new_height < 1) new_height = 1;

    // Center the image
    int padding = (term_width - new_width - 2) / 2;
    if (padding < 0) padding = 0;
    std::string pad_str(padding, ' ');

    // Print top border
    std::cout << pad_str << "+";
    for (int x = 0; x < new_width; ++x)
        std::cout << "-";
    std::cout << "+\n";

    // Convert and print ASCII art with color
    for (int y = 0; y < new_height; ++y)
    {
        std::cout << pad_str << "|";
        for (int x = 0; x < new_width; ++x)
        {
            // Map from output coordinates to source image coordinates
            int src_x = static_cast<int>(x * scale);
            int src_y = static_cast<int>(y * 2 * scale);

            if (src_x >= width) src_x = width - 1;
            if (src_y >= height) src_y = height - 1;

            // Get pixel RGB values
            int pixel_idx = (src_y * width + src_x) * 3;
            unsigned char r = data[pixel_idx];
            unsigned char g = data[pixel_idx + 1];
            unsigned char b = data[pixel_idx + 2];

            // Calculate brightness for character selection
            int brightness = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);

            // Map to ASCII character with more detail
            int char_index = (brightness * (NUM_ASCII_CHARS - 1)) / 255;
            char ascii_char = ASCII_CHARS[char_index];

            // Output colored character
            std::cout << rgb_color(r, g, b) << ascii_char << RESET_COLOR;
        }
        std::cout << "|\n";
    }

    // Print bottom border
    std::cout << pad_str << "+";
    for (int x = 0; x < new_width; ++x)
        std::cout << "-";
    std::cout << "+\n";

    stbi_image_free(data);
    return true;
}
