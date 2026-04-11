#include "sphere.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "material.hpp"

namespace gc {
// d -> vetor, o -> ponto, c -> ponto, r -> float

Sphere::Sphere(const Point3f &center, const real_type &radius)
    : center(center), radius(radius) {};

bool Sphere::intersect_p(const Ray &r) const {

  // TODO : STUB
  auto a = dot(r.d,r.d);
  auto b = dot(static_cast<real_type>(2)*(r.o-center),r.d);
  auto c = dot(r.o-center,r.o-center) - std::pow(radius, 2);
  auto delta = std::pow(b,2) - 4 * a * c;
  return delta >= 0;
}

bool Sphere::intersect(const Ray &r, Surfel *sf) const {
  auto a = dot(r.d,r.d);
  auto b = dot(static_cast<real_type>(2)*(r.o-center),r.d);
  auto c = dot(r.o-center,r.o-center) - std::pow(radius, 2);
  auto delta = std::pow(b,2) - 4 * a * c;
  return delta >= 0;
  return false;
}

const Material * Sphere::get_material() const {
  return material.get();
}

} // namespace gc
