#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "geometry.hpp"
#include "material.hpp"
#include "common.hpp"
#include <memory>

namespace gc {

// 1. "Aviso prévio" de que o Primitive existe, para o Surfel poder usar um ponteiro dele.
class Primitive;

// 2. Agora sim, a definição completa do Surfel
class Surfel {
public:
    Surfel(const Point3f& p, const Vector3f& n, const Vector3f& wo, float time,
           const Point2f& uv, const Primitive *pri)
        : p{p}, n{n}, wo{wo}, time{time}, uv{uv}, primitive{pri} {}

    Point3f p;          //!< Ponto de contato.
    Vector3f n;         //!< Normal da superfície.
    Vector3f wo;        //!< Direção da luz (inverso do raio).
    float time;         // <--- FALTAVA ESTA LINHA AQUI!
    Point2f uv;         //!< Coordenada UV.
    const Primitive *primitive = nullptr; //!< Ponteiro para a primitiva atingida.
};

// 3. E finalmente a classe base Primitive
class Primitive {
public:
    virtual ~Primitive() = default;
    virtual bool intersect(const Ray &r, Surfel *sf) const = 0;
    virtual bool intersect_p(const Ray &r) const = 0;

    virtual const Material *get_material() const = 0;

private:
    std::shared_ptr<Material> material;
};

} // namespace gc

#endif