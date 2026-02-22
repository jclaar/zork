#include "precomp.h"
#include "raylib_wrapper.h"
#include <iostream>
#include <filesystem>

// Conditional compilation - if RAYLIB_SUPPORT is defined, use raylib
// Otherwise provide stub implementations

#ifdef RAYLIB_SUPPORT

#include "raylib.h"

namespace {
    bool s_initialized = false;
    bool s_window_open = false;
    Texture2D s_current_texture = {0};
}

bool raylib_available()
{
    return true;
}

bool raylib_init(int width, int height, const std::string& title)
{
    if (s_initialized)
        return s_window_open;
    
    // Set config flags for a nicer window
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_TOPMOST);
    
    InitWindow(width, height, title.c_str());
    
    if (!IsWindowReady())
    {
        std::cerr << "Failed to initialize raylib window" << std::endl;
        return false;
    }
    
    // Set window icon if available
    s_initialized = true;
    s_window_open = true;
    
    return true;
}

void raylib_display_image(const std::string& image_path, int wait_ms)
{
    if (!s_window_open)
    {
        if (!raylib_init())
            return;
    }
    
    // Load texture
    Texture2D texture = LoadTexture(image_path.c_str());
    if (texture.id == 0)
    {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return;
    }
    
    // Unload previous texture
    if (s_current_texture.id != 0 && s_current_texture.id != texture.id)
    {
        UnloadTexture(s_current_texture);
    }
    s_current_texture = texture;
    
    // Resize window to fit image (with some padding)
    int win_width = std::min(texture.width + 20, GetMonitorWidth(GetCurrentMonitor()) - 100);
    int win_height = std::min(texture.height + 20, GetMonitorHeight(GetCurrentMonitor()) - 100);
    SetWindowSize(win_width, win_height);
    
    // Center window
    int monitor_width = GetMonitorWidth(GetCurrentMonitor());
    int monitor_height = GetMonitorHeight(GetCurrentMonitor());
    SetWindowPosition((monitor_width - win_width) / 2, (monitor_height - win_height) / 2);
    
    // Display loop
    bool should_close = false;
    double start_time = GetTime();
    
    while (!should_close && !WindowShouldClose())
    {
        // Check for timeout
        if (wait_ms > 0)
        {
            double elapsed = (GetTime() - start_time) * 1000;
            if (elapsed >= wait_ms)
                should_close = true;
        }
        
        // Handle resize
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Calculate scaling to fit window
        Rectangle dest = {0, 0, (float)texture.width, (float)texture.height};
        Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
        
        // Scale to fit while maintaining aspect ratio
        float scale_x = (float)GetScreenWidth() / texture.width;
        float scale_y = (float)GetScreenHeight() / texture.height;
        float scale = std::min(scale_x, scale_y);
        
        dest.width = texture.width * scale;
        dest.height = texture.height * scale;
        dest.x = (GetScreenWidth() - dest.width) / 2;
        dest.y = (GetScreenHeight() - dest.height) / 2;
        
        DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
        EndDrawing();
    }
    
    // Don't unload texture here - keep it for potential reuse
}

char raylib_display_image_interactive(const std::string& image_path)
{
    if (!s_window_open)
    {
        if (!raylib_init())
            return 'q';
    }
    
    // Load texture
    Texture2D texture = LoadTexture(image_path.c_str());
    if (texture.id == 0)
    {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return 'q';
    }
    
    // Unload previous texture
    if (s_current_texture.id != 0 && s_current_texture.id != texture.id)
    {
        UnloadTexture(s_current_texture);
    }
    s_current_texture = texture;
    
    // Resize window to fit image
    int win_width = std::min(texture.width + 20, GetMonitorWidth(GetCurrentMonitor()) - 100);
    int win_height = std::min(texture.height + 20, GetMonitorHeight(GetCurrentMonitor()) - 100);
    SetWindowSize(win_width, win_height);
    
    // Center window
    int monitor_width = GetMonitorWidth(GetCurrentMonitor());
    int monitor_height = GetMonitorHeight(GetCurrentMonitor());
    SetWindowPosition((monitor_width - win_width) / 2, (monitor_height - win_height) / 2);
    
    // Display loop - wait for input
    char result = 'c';
    
    while (!WindowShouldClose())
    {
        // Check for key presses
        if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
        {
            result = 'c';  // continue
            break;
        }
        if (IsKeyPressed(KEY_N))
        {
            result = 'n';  // next
            break;
        }
        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE))
        {
            result = 'q';  // quit
            break;
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Scale to fit
        float scale_x = (float)GetScreenWidth() / texture.width;
        float scale_y = (float)GetScreenHeight() / texture.height;
        float scale = std::min(scale_x, scale_y);
        
        Rectangle dest = {
            (GetScreenWidth() - texture.width * scale) / 2,
            (GetScreenHeight() - texture.height * scale) / 2,
            texture.width * scale,
            texture.height * scale
        };
        Rectangle source = {0, 0, (float)texture.width, (float)texture.height};
        
        DrawTexturePro(texture, source, dest, {0, 0}, 0, WHITE);
        
        // Draw instructions
        DrawText("Press: SPACE/ENTER=continue, N=next, Q/ESC=quit", 10, 10, 16, LIGHTGRAY);
        
        EndDrawing();
    }
    
    return result;
}

void raylib_close()
{
    if (s_current_texture.id != 0)
    {
        UnloadTexture(s_current_texture);
        s_current_texture = {0};
    }
    
    if (s_window_open)
    {
        CloseWindow();
        s_window_open = false;
    }
    
    s_initialized = false;
}

bool raylib_window_open()
{
    return s_window_open && !WindowShouldClose();
}

// RAII wrapper implementation
RaylibGuard::RaylibGuard(int width, int height, const std::string& title)
    : valid_(false)
{
    valid_ = raylib_init(width, height, title);
}

RaylibGuard::~RaylibGuard()
{
    if (valid_)
    {
        raylib_close();
    }
}

bool RaylibGuard::is_valid() const
{
    return valid_;
}

#else // RAYLIB_SUPPORT not defined - stub implementations

bool raylib_available()
{
    return false;
}

bool raylib_init(int, int, const std::string&)
{
    std::cerr << "Raylib support not compiled in" << std::endl;
    return false;
}

void raylib_display_image(const std::string&, int)
{
}

char raylib_display_image_interactive(const std::string&)
{
    return 'q';
}

void raylib_close()
{
}

bool raylib_window_open()
{
    return false;
}

RaylibGuard::RaylibGuard(int, int, const std::string&) : valid_(false) {}
RaylibGuard::~RaylibGuard() {}
bool RaylibGuard::is_valid() const { return false; }

#endif // RAYLIB_SUPPORT