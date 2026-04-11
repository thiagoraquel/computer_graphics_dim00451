#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "common.hpp"

namespace gc {
// WARN : This is a stub. The actual implementation will be done later
class Material {
public:
    virtual ColorXYZ get_color() const = 0;
    virtual ~Material() = default;
};
} // namespace gc

#endif // MATERIAL_HPP