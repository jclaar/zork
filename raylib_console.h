#pragma once

#include <string>
#include <vector>
#include <functional>

// Console configuration
constexpr int CONSOLE_FONT_SIZE = 18;
constexpr int CONSOLE_LINE_HEIGHT = 22;
constexpr int CONSOLE_MARGIN = 10;
constexpr int CONSOLE_MAX_LINES = 1000;
constexpr int INPUT_BUFFER_SIZE = 256;

// Initialize raylib console
bool raylib_console_init(int width = 1024, int height = 768, const std::string& title = "Zork++");

// Close raylib console
void raylib_console_close();

// Check if console is running
bool raylib_console_is_running();

// Print text to console
void raylib_console_print(const std::string& text);
void raylib_console_print_ln(const std::string& text);

// Set current room image (will be displayed above console)
void raylib_console_set_image(const std::string& image_path);
void raylib_console_clear_image();

// Get input from user (blocking)
// Returns empty string if window was closed
std::string raylib_console_get_input();

// Non-blocking input check
// Returns true if input is ready, string is filled with the input
bool raylib_console_try_get_input(std::string& out_input);

// Set input prompt (e.g., "> ")
void raylib_console_set_prompt(const std::string& prompt);

// Scroll console
void raylib_console_scroll(int lines);

// Clear console
void raylib_console_clear();

// Update and render one frame (for non-blocking mode)
void raylib_console_update();

// Set callback for when input is received
using InputCallback = std::function<void(const std::string&)>;
void raylib_console_set_input_callback(InputCallback callback);

// Check if raylib console is available
bool raylib_console_available();