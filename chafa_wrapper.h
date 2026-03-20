#pragma once

#include <string>

// Try to display an image using Chafa (dynamically loaded)
// Returns true if Chafa was available and the image was displayed
// Returns false if Chafa is not available (caller should use fallback)
bool display_image_chafa(const std::string& image_path, int max_width, int max_height);

// Check if Chafa is available on the system
bool chafa_available();

