#include <iostream>

struct Color {
    float r, g, b;
};

// The linear interpolation function: P(t) = (1 - t)A + tB
Color lerp(Color a, Color b, float t) {
    return {
        (1.0f - t) * a.r + t * b.r,
        (1.0f - t) * a.g + t * b.g,
        (1.0f - t) * a.b + t * b.b
    };
}

int main() {
    const int width = 400;
    const int height = 200;

    // Define the colors for the 4 corners
    Color bottomLeft  = {255, 0, 0};    // Red
    Color bottomRight = {0, 255, 0};    // Green
    Color topLeft     = {0, 0, 255};    // Blue
    Color topRight    = {255, 255, 0};  // Yellow

    std::cout << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            
            // Calculate u and v (normalized coordinates between 0 and 1)
            float u = static_cast<float>(i) / (width - 1);
            
            // Note: PPM starts at top (j=0), so v=1.0 is the top for our math
            // We flip j to make v=0 at the bottom and v=1 at the top
            float v = 1.0f - (static_cast<float>(j) / (height - 1));

            // Step 1: Interpolate horizontally along the bottom
            Color xb = lerp(bottomLeft, bottomRight, u);

            // Step 2: Interpolate horizontally along the top
            Color xt = lerp(topLeft, topRight, u);

            // Step 3: Interpolate vertically between the two results
            Color finalColor = lerp(xb, xt, v);

            // Output as integers
            std::cout << static_cast<int>(finalColor.r) << " "
                      << static_cast<int>(finalColor.g) << " "
                      << static_cast<int>(finalColor.b) << "\n";
        }
    }

    return 0;
}