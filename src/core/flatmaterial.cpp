
#include "flatmaterial.hpp"
#include "geometry.hpp"
#include "common.hpp"

namespace gc {
ColorXYZ FlatMaterial::get_color() const {
  // TODO : Return pointer later
  return color;
}

FlatMaterial::FlatMaterial(const ColorXYZ &c) : color(c) {}
} // namespace gc
