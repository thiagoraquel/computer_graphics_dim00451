#ifndef IMAGE_HPP
# define IMAGE_HPP

# include <cstdint>
# include <string>
# include <vector>

namespace gc {
/// Routines to write images to a file.
bool save_ppm6(const std::vector<unsigned char> &, size_t, size_t, size_t = 1, const std::string& = "image.ppm");

/// Saves an image as a **ascii** PPM file.
bool save_ppm3(const std::vector<unsigned char> &, size_t, size_t, size_t = 1, const std::string& = "image.ppm");

/// Saves an image as a PNG file.
bool save_png(const std::vector<unsigned char> &, size_t, size_t, size_t = 1, const std::string& = "image.png");
}  // namespace gc

#endif

//================================[ imagem.h ]================================//
