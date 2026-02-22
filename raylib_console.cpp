#include "precomp.h"
#include "raylib_console.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <thread>

#ifdef RAYLIB_SUPPORT

#include "raylib.h"

namespace {

// Console state
struct ConsoleState {
    bool initialized = false;
    bool running = false;
    
    // Text buffer
    std::vector<std::string> lines;
    std::mutex lines_mutex;
    int scroll_offset = 0;
    int max_visible_lines = 20;
    
    // Input
    std::string input_buffer;
    std::string prompt = "> ";
    std::mutex input_mutex;
    std::condition_variable input_cv;
    bool input_ready = false;
    std::string pending_input;
    
    // Image
    Texture2D current_image = {0};
    std::string current_image_path;
    std::mutex image_mutex;
    float image_scale = 1.0f;
    
    // Callback
    InputCallback input_callback;
    
    // Window dimensions
    int window_width = 1024;
    int window_height = 768;
    int console_y = 300;  // Y position where console starts (below image)
    
    // Font
    Font font = {0};
    int font_size = CONSOLE_FONT_SIZE;
    int line_height = CONSOLE_LINE_HEIGHT;
    
    // Colors
    Color bg_color = {20, 20, 30, 255};
    Color text_color = {200, 200, 200, 255};
    Color input_color = {255, 255, 100, 255};
    Color prompt_color = {100, 200, 100, 255};
    Color border_color = {60, 60, 80, 255};
};

ConsoleState g_console;

// Helper: Word wrap text to fit console width
std::vector<std::string> wrap_text(const std::string& text, int max_width) {
    std::vector<std::string> result;
    std::string line;
    std::string word;
    
    for (char c : text) {
        if (c == '\n') {
            if (!line.empty()) {
                result.push_back(line);
                line.clear();
            }
            result.push_back("");
        } else if (c == ' ' || c == '\t') {
            if (!line.empty()) {
                line += ' ';
            }
        } else {
            line += c;
            // Check if line is too long
            int line_width = MeasureText(line.c_str(), g_console.font_size);
            if (line_width > max_width) {
                // Find last space to break
                size_t last_space = line.find_last_of(' ');
                if (last_space != std::string::npos && last_space > 0) {
                    result.push_back(line.substr(0, last_space));
                    line = line.substr(last_space + 1);
                } else {
                    result.push_back(line);
                    line.clear();
                }
            }
        }
    }
    
    if (!line.empty()) {
        result.push_back(line);
    }
    
    return result;
}

// Calculate console area dimensions
void update_layout() {
    // If we have an image, console starts below it
    if (g_console.current_image.id != 0) {
        int img_height = (int)(g_console.current_image.height * g_console.image_scale);
        g_console.console_y = std::min(img_height + 10, g_console.window_height / 2);
    } else {
        g_console.console_y = 10;
    }
    
    // Calculate visible lines
    int console_height = g_console.window_height - g_console.console_y - CONSOLE_MARGIN * 2 - g_console.line_height;
    g_console.max_visible_lines = console_height / g_console.line_height;
}

// Render the console
void render_console() {
    BeginDrawing();
    ClearBackground(g_console.bg_color);
    
    // Draw image if available
    if (g_console.current_image.id != 0) {
        std::lock_guard<std::mutex> lock(g_console.image_mutex);
        
        // Calculate scaling to fit width
        float scale = (float)(g_console.window_width - 20) / g_console.current_image.width;
        scale = std::min(scale, 0.5f);  // Max 50% of original size for console mode
        
        Rectangle dest = {
            (g_console.window_width - g_console.current_image.width * scale) / 2,
            5,
            g_console.current_image.width * scale,
            g_console.current_image.height * scale
        };
        Rectangle source = {0, 0, (float)g_console.current_image.width, (float)g_console.current_image.height};
        
        DrawTexturePro(g_console.current_image, source, dest, {0, 0}, 0, WHITE);
    }
    
    // Draw console border
    int console_top = g_console.console_y;
    DrawRectangleLines(CONSOLE_MARGIN - 2, console_top - 2, 
                       g_console.window_width - CONSOLE_MARGIN * 2 + 4,
                       g_console.window_height - console_top - CONSOLE_MARGIN + 4,
                       g_console.border_color);
    
    // Draw text lines
    {
        std::lock_guard<std::mutex> lock(g_console.lines_mutex);
        
        int start_line = std::max(0, (int)g_console.lines.size() - g_console.max_visible_lines - g_console.scroll_offset);
        int y = console_top + CONSOLE_MARGIN;
        
        for (int i = start_line; i < (int)g_console.lines.size() - g_console.scroll_offset && i < start_line + g_console.max_visible_lines; i++) {
            if (i >= 0 && i < (int)g_console.lines.size()) {
                DrawText(g_console.lines[i].c_str(), CONSOLE_MARGIN, y, g_console.font_size, g_console.text_color);
                y += g_console.line_height;
            }
        }
    }
    
    // Draw input area
    int input_y = g_console.window_height - g_console.line_height - CONSOLE_MARGIN;
    DrawRectangle(CONSOLE_MARGIN - 2, input_y - 2, 
                  g_console.window_width - CONSOLE_MARGIN * 2 + 4,
                  g_console.line_height + 4,
                  {30, 30, 40, 255});
    
    // Draw prompt
    DrawText(g_console.prompt.c_str(), CONSOLE_MARGIN, input_y, g_console.font_size, g_console.prompt_color);
    
    // Draw input buffer
    {
        std::lock_guard<std::mutex> lock(g_console.input_mutex);
        int prompt_width = MeasureText(g_console.prompt.c_str(), g_console.font_size);
        DrawText(g_console.input_buffer.c_str(), CONSOLE_MARGIN + prompt_width, input_y, g_console.font_size, g_console.input_color);
        
        // Draw cursor
        int cursor_x = CONSOLE_MARGIN + prompt_width + MeasureText(g_console.input_buffer.c_str(), g_console.font_size);
        if ((GetTime() - (int)GetTime()) < 0.5) {  // Blinking cursor
            DrawRectangle(cursor_x, input_y, 8, g_console.font_size, g_console.input_color);
        }
    }
    
    // Draw scroll indicator if needed
    {
        std::lock_guard<std::mutex> lock(g_console.lines_mutex);
        if (g_console.lines.size() > (size_t)g_console.max_visible_lines) {
            int total_lines = g_console.lines.size();
            int visible_lines = g_console.max_visible_lines;
            float scroll_ratio = (float)g_console.scroll_offset / (total_lines - visible_lines);
            
            int scroll_bar_height = 100;
            int scroll_bar_y = console_top + (int)((g_console.window_height - console_top - scroll_bar_height - 50) * (1 - scroll_ratio));
            
            DrawRectangle(g_console.window_width - 15, scroll_bar_y, 8, scroll_bar_height, {80, 80, 100, 255});
        }
    }
    
    // Draw help text at top
    DrawText("ZORK++ GUI Mode | UP/DOWN=scroll | ESC=quit", 
             10, 5, 14, {100, 100, 120, 255});
    
    EndDrawing();
}

// Handle keyboard input
void handle_input() {
    std::lock_guard<std::mutex> lock(g_console.input_mutex);
    
    // Handle scrolling
    if (IsKeyPressed(KEY_UP)) {
        std::lock_guard<std::mutex> lines_lock(g_console.lines_mutex);
        g_console.scroll_offset = std::min(g_console.scroll_offset + 1, 
                                           std::max(0, (int)g_console.lines.size() - g_console.max_visible_lines));
    }
    if (IsKeyPressed(KEY_DOWN)) {
        g_console.scroll_offset = std::max(0, g_console.scroll_offset - 1);
    }
    if (IsKeyPressed(KEY_PAGE_UP)) {
        std::lock_guard<std::mutex> lines_lock(g_console.lines_mutex);
        g_console.scroll_offset = std::min(g_console.scroll_offset + g_console.max_visible_lines,
                                           std::max(0, (int)g_console.lines.size() - g_console.max_visible_lines));
    }
    if (IsKeyPressed(KEY_PAGE_DOWN)) {
        g_console.scroll_offset = std::max(0, g_console.scroll_offset - g_console.max_visible_lines);
    }
    
    // Handle text input
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32 && key <= 126) || IsKeyPressed(KEY_SPACE)) {  // Printable ASCII
            g_console.input_buffer += (char)key;
        }
        key = GetCharPressed();
    }
    
    // Handle special keys
    if (IsKeyPressed(KEY_BACKSPACE) && !g_console.input_buffer.empty()) {
        g_console.input_buffer.pop_back();
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        // Submit input
        if (!g_console.input_buffer.empty()) {
            g_console.pending_input = g_console.input_buffer;
            g_console.input_ready = true;
            
            // Echo to console
            raylib_console_print_ln(g_console.prompt + g_console.input_buffer);
            
            g_console.input_buffer.clear();
            g_console.input_cv.notify_one();
            
            // Call callback if set
            if (g_console.input_callback) {
                g_console.input_callback(g_console.pending_input);
            }
        }
    }
    
    // Escape to quit
    if (IsKeyPressed(KEY_ESCAPE)) {
        g_console.running = false;
        g_console.pending_input = "quit";
        g_console.input_ready = true;
        g_console.input_cv.notify_one();
    }
}

// Handle window resize
void handle_resize() {
    if (IsWindowResized()) {
        g_console.window_width = GetScreenWidth();
        g_console.window_height = GetScreenHeight();
        update_layout();
    }
}

}  // anonymous namespace

bool raylib_console_available() {
    return true;
}

bool raylib_console_init(int width, int height, const std::string& title) {
    if (g_console.initialized) {
        return g_console.running;
    }
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(width, height, title.c_str());
    
    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize raylib window" << std::endl;
        return false;
    }
    
    g_console.window_width = width;
    g_console.window_height = height;
    g_console.initialized = true;
    g_console.running = true;
    
    // Load default font
    g_console.font = GetFontDefault();
    
    update_layout();
    
    // Print welcome message
    raylib_console_print_ln("ZORK++ GUI Mode Started");
    raylib_console_print_ln("Type commands and press ENTER to submit");
    raylib_console_print_ln("Use UP/DOWN to scroll, ESC to quit");
    raylib_console_print_ln("");
    
    return true;
}

void raylib_console_close() {
    if (!g_console.initialized) return;
    
    // Unload image texture
    if (g_console.current_image.id != 0) {
        UnloadTexture(g_console.current_image);
        g_console.current_image = {0};
    }
    
    CloseWindow();
    g_console.initialized = false;
    g_console.running = false;
}

bool raylib_console_is_running() {
    return g_console.initialized && g_console.running && !WindowShouldClose();
}

void raylib_console_print(const std::string& text) {
    std::lock_guard<std::mutex> lock(g_console.lines_mutex);
    
    // Word wrap and add lines
    int max_width = g_console.window_width - CONSOLE_MARGIN * 2;
    auto wrapped = wrap_text(text, max_width);
    
    for (const auto& line : wrapped) {
        g_console.lines.push_back(line);
        
        // Limit buffer size
        if (g_console.lines.size() > CONSOLE_MAX_LINES) {
            g_console.lines.erase(g_console.lines.begin());
        }
    }
    
    // Auto-scroll to bottom
    g_console.scroll_offset = 0;
}

void raylib_console_print_ln(const std::string& text) {
    raylib_console_print(text + "\n");
}

void raylib_console_set_image(const std::string& image_path) {
    std::lock_guard<std::mutex> lock(g_console.image_mutex);
    
    // Unload previous image
    if (g_console.current_image.id != 0) {
        UnloadTexture(g_console.current_image);
        g_console.current_image = {0};
    }
    
    // Debug: print loading attempt
    std::cerr << "Loading image: " << image_path << std::endl;
    
    // Load new image
    g_console.current_image = LoadTexture(image_path.c_str());
    g_console.current_image_path = image_path;
    
    if (g_console.current_image.id == 0) {
        std::cerr << "Failed to load image: " << image_path << std::endl;
    } else {
        std::cerr << "Image loaded successfully: " << image_path 
                  << " (ID: " << g_console.current_image.id 
                  << ", Size: " << g_console.current_image.width << "x" << g_console.current_image.height << ")" << std::endl;
    }
    
    update_layout();
}

void raylib_console_clear_image() {
    std::lock_guard<std::mutex> lock(g_console.image_mutex);
    
    if (g_console.current_image.id != 0) {
        UnloadTexture(g_console.current_image);
        g_console.current_image = {0};
    }
    
    g_console.current_image_path.clear();
    update_layout();
}

std::string raylib_console_get_input() {
    if (!g_console.initialized || !g_console.running) {
        return "";
    }
    
    // Run event loop until input is received
    while (g_console.running && !WindowShouldClose()) {
        handle_resize();
        handle_input();
        render_console();
        
        // Check if input is ready
        {
            std::lock_guard<std::mutex> lock(g_console.input_mutex);
            if (g_console.input_ready) {
                g_console.input_ready = false;
                return g_console.pending_input;
            }
        }
    }
    
    g_console.running = false;
    return "quit";
}

bool raylib_console_try_get_input(std::string& out_input) {
    std::lock_guard<std::mutex> lock(g_console.input_mutex);
    
    if (g_console.input_ready) {
        g_console.input_ready = false;
        out_input = g_console.pending_input;
        return true;
    }
    
    return false;
}

void raylib_console_set_prompt(const std::string& prompt) {
    std::lock_guard<std::mutex> lock(g_console.input_mutex);
    g_console.prompt = prompt;
}

void raylib_console_scroll(int lines) {
    std::lock_guard<std::mutex> lock(g_console.lines_mutex);
    g_console.scroll_offset = std::max(0, std::min(g_console.scroll_offset + lines,
                                                    (int)g_console.lines.size() - g_console.max_visible_lines));
}

void raylib_console_clear() {
    std::lock_guard<std::mutex> lock(g_console.lines_mutex);
    g_console.lines.clear();
    g_console.scroll_offset = 0;
}

void raylib_console_update() {
    if (!g_console.initialized || !g_console.running) return;
    
    if (WindowShouldClose()) {
        g_console.running = false;
        return;
    }
    
    handle_resize();
    handle_input();
    render_console();
}

void raylib_console_set_input_callback(InputCallback callback) {
    g_console.input_callback = callback;
}

#else  // RAYLIB_SUPPORT not defined

bool raylib_console_available() { return false; }
bool raylib_console_init(int, int, const std::string&) { return false; }
void raylib_console_close() {}
bool raylib_console_is_running() { return false; }
void raylib_console_print(const std::string&) {}
void raylib_console_print_ln(const std::string&) {}
void raylib_console_set_image(const std::string&) {}
void raylib_console_clear_image() {}
std::string raylib_console_get_input() { return ""; }
bool raylib_console_try_get_input(std::string&) { return false; }
void raylib_console_set_prompt(const std::string&) {}
void raylib_console_scroll(int) {}
void raylib_console_clear() {}
void raylib_console_update() {}
void raylib_console_set_input_callback(InputCallback) {}

#endif  // RAYLIB_SUPPORT