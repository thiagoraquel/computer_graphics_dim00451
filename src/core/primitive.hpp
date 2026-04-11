#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP
#include "common.hpp"
#include "geometry.hpp"
#include <memory>

namespace gc {
class Primitive {
public:
  virtual ~Primitive() = default;
  virtual bool intersect(const gc::Ray &r, gc::Surfel *sf) const = 0;
  // Simpler & faster version of intersection that only return true/false.
  // It does not compute the hit point information.
  // WARN : Virtual function might be missing declaration
  virtual bool intersect_p(const gc::Ray &r) const = 0;

  virtual const Material *get_material(void) const = 0;

private:
  std::shared_ptr<gc::Material> material;
};

} // namespace gc

#endif
