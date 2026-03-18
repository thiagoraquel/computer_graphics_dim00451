#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>

#include <fstream>   // <--- This fixes the "incomplete type" error
#include <cmath>     // For std::pow in gamma correction

#include "lodepng.h"
#include "../msg_system/error.hpp"
#include "app.hpp"
#include "common.hpp"
#include "film.hpp"
#include "geometry.hpp"
#include "image_io.hpp"
#include "paramset.hpp"
#include <string_view>

namespace gc {

//=== Film Method Definitions
Film::Film(const Point2i& resolution, const std::string& filename, image_type_e image_type, bool gamma_corrected)
    : m_full_resolution{ resolution }, m_filename{ filename },
      m_activate_gamma_correction{ gamma_corrected }, m_image_type{ image_type } {
    
    // Allocate space for all pixels (width * height)
    m_color_buffer.resize(resolution.x * resolution.y);
}

Film::~Film() = default;

/// Add the Spectrum color to image. Pixel coords comes as (x,y).
void Film::add_sample(const Point2i& pixel_coord, const Spectrum& pixel_color) const {
    int index = pixel_coord.y * m_full_resolution.x + pixel_coord.x;

    // Safety check: if the index is too high, something is wrong with our loops
    if (index < m_color_buffer.size()) {
        const_cast<Film*>(this)->m_color_buffer[index] = pixel_color;
    }
}

/// Convert Spectrum image information to RGB, compute final pixel values, write image.
void Film::write_image() const {
    // 1. Prepare a buffer for the final 8-bit pixels
    // PNG/PPM expect: [R, G, B, R, G, B, ...]
    std::vector<unsigned char> byte_buffer;
    byte_buffer.reserve(m_full_resolution.x * m_full_resolution.y * 3);

    for (const auto& spec : m_color_buffer) {
        Spectrum color = spec;

        // 2. Apply Gamma Correction (if requested)
        // Most monitors use a gamma of 2.2
        if (m_activate_gamma_correction) {
            color.r = std::pow(color.r, 1.0f / 2.2f);
            color.g = std::pow(color.g, 1.0f / 2.2f);
            color.b = std::pow(color.b, 1.0f / 2.2f);
        }

        // 3. Clamp and Quantize to [0, 255]
        // We use clamp to ensure a value like 1.1 doesn't wrap around
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.r * 255.0f, 0.0f, 255.0f)));
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.g * 255.0f, 0.0f, 255.0f)));
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.b * 255.0f, 0.0f, 255.0f)));
    }

    // 4. Save to file based on the requested format
    if (m_image_type == image_type_e::PNG) {
        unsigned error = lodepng::encode(m_filename, byte_buffer, m_full_resolution.x, m_full_resolution.y, LCT_RGB);
        if (error) {
            std::cerr << "PNG Encoder Error: " << lodepng_error_text(error) << std::endl;
        }
    } else if (m_image_type == image_type_e::PPM3 || m_image_type == image_type_e::PPM6) {
        // Simple PPM saving logic
        std::ofstream ofs(m_filename, std::ios::out | std::ios::binary);
        ofs << "P6\n" << m_full_resolution.x << " " << m_full_resolution.y << "\n255\n";
        ofs.write(reinterpret_cast<char*>(byte_buffer.data()), byte_buffer.size());
        ofs.close();
    }
    
    std::cout << ">>> Image saved successfully to: " << m_filename << "\n";
}

/// Chooses the filename based on the CLI and scene file info.
std::string handles_filename(const ParamSet& ps) {
  // Retrieve the filename from the XML, default to "render.png" if missing
  return ps.retrieve<std::string>("filename", "render.png");
}

// /// Process ParamSet, extracts, validates a valid crop window.
// gc::Bounds2f handles_cropwindow(const ParamSet& ps) {
//   // TODO:
// }

/// Parses the dimensions of the film from the ParamSet.
gc::Point2i handles_dimensions(const ParamSet& ps) {
  Point2i film_dimension{ ps.retrieve<int>("w_res", 1280), ps.retrieve<int>("h_res", 720) };
  // Quick render?
  if (App::m_current_run_options.quick_render) {
    // decrease resolution.
    film_dimension.x = std::max(1, film_dimension.x / 4);
    film_dimension.y = std::max(1, film_dimension.y / 4);
  }
  return film_dimension;
}

/// Creates and returns a `Film` objected based on the `ParamSet` provided.
Film* create_film(const ParamSet& ps) {
#ifdef DEBUG
  std::cout << ">>> Inside create_film()\n";
#endif
  //==[1] Choose the filename.
  auto filename = handles_filename(ps);

  //==[2] Define the crop window information.
  // auto crop_window = handles_cropwindow(ps);

  //==[3] Retrieve film dimensions and handles quick_render option.
  Point2i dimensions = handles_dimensions(ps);

  //==[4] Retrieve film type.
  std::unordered_map<std::string, Film::image_type_e> image_type{
    { "png", Film::image_type_e::PNG },
    { "ppm3", Film::image_type_e::PPM3 },
    { "ppm6", Film::image_type_e::PPM6 },
    { "ppm", Film::image_type_e::PPM6 },
  };
  auto type{ image_type[ps.retrieve<std::string>("img_type", "png")] };

  //==[5] Get gamma correction request.
  bool apply_gamma_correction = ps.retrieve<bool>("gamma_corrected", false);

#ifdef DEBUG
  std::cout << "================================================\n";
  std::cout << ">>> create_film() - film parameters are:\n";
  std::cout << "    - filename: " << std::quoted(filename) << "\n";
  // std::cout << "    - crop window: " << crop_window << "\n";
  std::cout << "    - w_res: " << dimensions.x << "\n";
  std::cout << "    - h_res: " << dimensions.y << "\n";
  std::cout << "    - image type: " << ps.retrieve<std::string>("img_type", "png") << "\n";
  std::cout << "    - gamma correction: " << std::boolalpha << apply_gamma_correction << "\n";
  std::cout << "================================================\n";
#endif

  return new Film(dimensions, filename, type, apply_gamma_correction);
}
}  // namespace gc
