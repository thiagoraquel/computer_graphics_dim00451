#ifndef FLATMATERIAL_HPP
#define FLATMATERIAL_HPP

#include "common.hpp"
#include "material.hpp"
#include "geometry.hpp"
#include <memory>

namespace gc {
class FlatMaterial : public Material {
  public:
    FlatMaterial(const ColorXYZ &);
    virtual ColorXYZ get_color() const override;
  private:
    ColorXYZ color;
};
} // namespace gc

#endif // FLATMATERIAL_HPP