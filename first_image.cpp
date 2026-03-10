#include <iostream>

int main() {
    // Image dimensions
    const int width = 400;
    const int height = 200;

    // PPM Header: 
    // P3 means colors are in ASCII, 
    // followed by width, height, and max color value
    std::cout << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            
            // Red increases from left (0) to right (255)
            float r_ratio = static_cast<float>(i) / (width - 1);
            int r = static_cast<int>(255.999 * r_ratio);

            // Green increases from bottom (0) to top (255)
            // Since j=0 is the top, we invert the ratio
            float g_ratio = 1.0f - (static_cast<float>(j) / (height - 1));
            int g = static_cast<int>(255.999 * g_ratio);

            // Blue is constant at 20% intensity
            int b = static_cast<int>(255 * 0.20);

            // Output the pixel
            std::cout << r << " " << g << " " << b << "\n";
        }
    }

    return 0;
}