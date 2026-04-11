#ifndef SPHERE_HPP
#define SPHERE_HPP
#include "geometry.hpp"
#include "primitive.hpp"
#include <cmath>
#include <memory>

namespace gc {
class Sphere : public Primitive {
private:
  const Point3f center;
  const real_type radius;
  std::shared_ptr<Material> material;

public:
  Sphere(const Point3f &center, const real_type &radius);

  bool intersect(const Ray &r, Surfel *sf) const override;

  // Simpler & faster version of intersection that only return true/false.
  // It does not compute the hit point information.
  bool intersect_p(const Ray &r) const override;

  const gc::Material *get_material(void) const override;
};

} // namespace gc
#endif
