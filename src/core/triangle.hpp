#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "geometry.hpp"
#include "primitive.hpp"
#include <memory>

namespace gc {

class Triangle : public Primitive {
private:
    Point3f v0, v1, v2;
    std::shared_ptr<Material> material;
    Vector3f normal; // A normal do triângulo é constante, podemos pré-calcular

public:
    Triangle(const Point3f &v0, const Point3f &v1, const Point3f &v2, std::shared_ptr<Material> mat = nullptr);

    bool intersect(const Ray &r, Surfel *sf) const override;
    bool intersect_p(const Ray &r) const override;

    const Material *get_material() const override;
};

} // namespace gc
#endif