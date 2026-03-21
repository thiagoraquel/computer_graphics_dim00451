#include <cstdint>

#include <fstream>

#include "image_io.hpp"
#include <lodepng.h>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace gc {

/// Saves an image as a **binary** PPM file.
bool save_ppm6(const std::vector<unsigned char> & data,
               size_t w,
               size_t h,
               size_t d,
               const std::string& file_name_) {
    try
    {
      // Simple PPM saving logic
      std::ofstream ofs(file_name_, std::ios::out | std::ios::binary);
      ofs << "P6\n" << w << " " << h << "\n255\n";
      ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
      ofs.close();
    }
    catch(const std::exception& e)
    {
      std::cerr << "Erro em salvar imagem PPM6" << e.what() << '\n';
      return false;
    }
  return true; 
}

/// Saves an image as a **ascii** PPM file.
bool save_ppm3(const std::vector<unsigned char> & data,
               size_t w,
               size_t h,
               size_t d,
               const std::string& file_name_) {
    try
    {
      // Simple PPM saving logic
      std::ofstream ofs(file_name_);
      ofs << "P3\n" << w << " " << h << "\n255\n";
      std::copy(data.begin(), data.end(), std::ostream_iterator<int>(ofs, " "));     
      ofs.close();
    }
    catch(const std::exception& e)
    {
      std::cerr << "Erro em salvar imagem PPM3" << e.what() << '\n';
      return false;
    }
  return true; 
}

bool save_png(const std::vector<unsigned char> & data,
              size_t w,
              size_t h,
              size_t d,
              const std::string& file_name_) {
  unsigned error = lodepng::encode(file_name_, data, w, h, LCT_RGB);
  if (error) {
      std::cerr << "PNG Encoder Error: " << lodepng_error_text(error) << std::endl;
      return false;
  }
  return true;  
}

}  // namespace gc

//================================[ imagem_io.h ]================================//
