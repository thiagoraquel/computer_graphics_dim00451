#include "sphere.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "material.hpp"

namespace gc {
// d -> vetor, o -> ponto, c -> ponto, r -> float

Sphere::Sphere(const Point3f &center, const real_type &radius)
    : center(center), radius(radius) {};

bool Sphere::intersect_p(const Ray &r) const {
    auto a = dot(r.d, r.d);
    auto b = 2.0f * dot(r.o - center, r.d);
    auto c = dot(r.o - center, r.o - center) - (radius * radius);
    
    auto delta = (b * b) - (4.0f * a * c);

    if (delta < 0.0f) return false; // Errou a esfera

    real_type root_delta = std::sqrt(delta);
    real_type inv_2a = 1.0f / (2.0f * a);

    // Checa o primeiro ponto de contato (t1)
    real_type t1 = (-b - root_delta) * inv_2a;
    if (t1 >= r.t_min && t1 <= r.t_max) return true;

    // Checa o segundo ponto de contato (t2, caso a câmera esteja dentro da esfera)
    real_type t2 = (-b + root_delta) * inv_2a;
    if (t2 >= r.t_min && t2 <= r.t_max) return true;

    return false; // Cruzou, mas estava atrás da câmera ou longe demais
}

bool Sphere::intersect(const Ray &r, Surfel *sf) const {
    auto a = dot(r.d, r.d);
    auto b = 2.0f * dot(r.o - center, r.d);
    auto c = dot(r.o - center, r.o - center) - (radius * radius);
    
    auto delta = (b * b) - (4.0f * a * c);

    // 1. Passou reto pela esfera
    if (delta < 0.0f) return false; 

    real_type root_delta = std::sqrt(delta);
    real_type inv_2a = 1.0f / (2.0f * a);

    // 2. Tenta a primeira raiz (ponto de entrada)
    real_type t_hit = (-b - root_delta) * inv_2a;
    
    // Se a primeira raiz for inválida, tenta a segunda (ponto de saída)
    if (t_hit < r.t_min || t_hit > r.t_max) {
        t_hit = (-b + root_delta) * inv_2a;
        if (t_hit < r.t_min || t_hit > r.t_max) {
            return false; // Nenhuma das raízes está na área de visão válida
        }
    }

    // ==========================================
    // SE CHEGAMOS AQUI, TEMOS UMA COLISÃO VÁLIDA
    // ==========================================

    // 3. Encurta o raio! (Isso evita que objetos de trás apareçam na frente)
    r.t_max = t_hit;

    // 4. Preenche a "ficha de contato" (Surfel)
    if (sf != nullptr) {
        Point3f p = r(t_hit);               // Calcula o ponto 3D exato do impacto: O + tD
        Vector3f n = normalize(p - center); // A normal é um vetor do centro até a borda
        Vector3f wo = -r.d;                 // O raio de luz vem da direção oposta do raio da câmera
        Point2f uv{0.0f, 0.0f};             // UV (textura) preenchido com zero por enquanto
        
        *sf = Surfel(p, n, wo, 0.0f, uv, this);
    }

    return true;
}

const Material * Sphere::get_material() const {
  return material.get();
}

} // namespace gc
