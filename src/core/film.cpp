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
    // Populate the image_io functions
    m_image_io_dict = {
          {image_type_e::PNG, save_png},
          {image_type_e::PPM3, save_ppm3},
          {image_type_e::PPM6, save_ppm6}
        };
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
    std::vector<unsigned char> byte_buffer;
    byte_buffer.reserve(m_full_resolution.x * m_full_resolution.y * 3);

    for (const auto& spec : m_color_buffer) {
        Spectrum color = spec;

        // TODO : Understand what gamma correction even is
        if (m_activate_gamma_correction) {
          // TODO
        }

        // Using std::clamp to ensure values in [0,255]
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.r * 255.0f, 0.0f, 255.0f)));
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.g * 255.0f, 0.0f, 255.0f)));
        byte_buffer.push_back(static_cast<unsigned char>(std::clamp(color.b * 255.0f, 0.0f, 255.0f)));
    }
    // TODO : Better error handling for unidentified output tag
    if (m_image_io_dict.count(m_image_type) == 0) {
      std::cerr << ">>> Image type not acceptable <<<";
      return;
    }
    auto image_io = m_image_io_dict.at(m_image_type);
    bool image_writing_success = image_io(byte_buffer, m_full_resolution.x, m_full_resolution.y, 3, m_filename);
    if (image_writing_success) {
      std::cout << ">>> Image saved successfully to: " << m_filename << "\n";
    }
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
