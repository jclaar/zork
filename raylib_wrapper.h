#pragma once

#include <string>
#include <memory>

// Forward declarations to avoid including raylib.h in header
class RaylibWindow;

// Check if raylib is available at runtime
bool raylib_available();

// Initialize raylib window
bool raylib_init(int width = 800, int height = 600, const std::string& title = "Zork++");

// Display an image in the raylib window
// Returns when user closes window or presses a key
void raylib_display_image(const std::string& image_path, int wait_ms = 0);

// Display an image and wait for user input
// Returns: 'c' = continue, 'q' = quit, 'n' = next (for multiple images)
char raylib_display_image_interactive(const std::string& image_path);

// Close raylib window
void raylib_close();

// Check if raylib window is open
bool raylib_window_open();

// RAII wrapper for raylib
class RaylibGuard {
public:
    RaylibGuard(int width = 800, int height = 600, const std::string& title = "Zork++");
    ~RaylibGuard();
    bool is_valid() const;
private:
    bool valid_;
};