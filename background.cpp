#include <iostream>
#include <vector>
#include <array>

// Simple structure to hold color channels
struct RGBColor {
    float r, g, b;
};

class BackgroundColor {
  private:
    // Corner storage using the indices provided in the prompt
    std::array<RGBColor, 4> corners;

    enum Corners_e {
        bl = 0, // Bottom left
        tl,     // Top left
        tr,     // Top right
        br      // Bottom right
    };

    // The linear interpolation math: P(t) = (1 - t)A + tB
    RGBColor lerp(const RGBColor& A, const RGBColor& B, float t) const {
        return {
            (1.0f - t) * A.r + t * B.r,
            (1.0f - t) * A.g + t * B.g,
            (1.0f - t) * A.b + t * B.b
        };
    }

  public:
    // Constructor takes a vector of 4 colors (bl, tl, tr, br)
    BackgroundColor(const std::vector<RGBColor>& colors) {
        for (size_t i = 0; i < 4 && i < colors.size(); ++i) {
            corners[i] = colors[i];
        }
    }

    ~BackgroundColor() {}

    // The Bilinear Interpolation logic hidden inside a "sample" method
    RGBColor sampleUV(float u, float v) const {
        // 1. Interpolate horizontally along the bottom (bl to br)
        RGBColor bottom_lerp = lerp(corners[bl], corners[br], u);
        
        // 2. Interpolate horizontally along the top (tl to tr)
        RGBColor top_lerp = lerp(corners[tl], corners[tr], u);
        
        // 3. Interpolate vertically between the two horizontal results
        return lerp(bottom_lerp, top_lerp, v);
    }
};

int main() {
    const int width = 400;
    const int height = 200;

    // Initialize the background with 4 colors: bl, tl, tr, br
    std::vector<RGBColor> cornerColors = {
        {255, 0, 0},   // Bottom Left: Red
        {0, 255, 0},   // Top Left: Green
        {255, 255, 0}, // Top Right: Yellow
        {0, 0, 255}    // Bottom Right: Blue
    };

    BackgroundColor bkg(cornerColors);

    // PPM Header
    std::cout << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // Normalize i and j to [0, 1] range
            float u = static_cast<float>(i) / (width - 1);
            
            // Remember: j=0 is the top of the image in PPM.
            // Our sampleUV expects v=1.0 at the top and v=0.0 at the bottom.
            float v = 1.0f - (static_cast<float>(j) / (height - 1));

            // Just "sample" the background!
            RGBColor pixel = bkg.sampleUV(u, v);

            // Output the final values
            std::cout << static_cast<int>(pixel.r) << " "
                      << static_cast<int>(pixel.g) << " "
                      << static_cast<int>(pixel.b) << "\n";
        }
    }

    return 0;
}