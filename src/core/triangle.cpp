#include "triangle.hpp"
#include <cmath>

namespace gc {

Triangle::Triangle(const Point3f &v0, const Point3f &v1, const Point3f &v2, std::shared_ptr<Material> mat)
    : v0(v0), v1(v1), v2(v2), material(mat) {
    // A normal do plano do triângulo é o produto vetorial das duas arestas
    Vector3f e1 = v1 - v0;
    Vector3f e2 = v2 - v0;
    normal = normalize(cross(e1, e2));
}

bool Triangle::intersect_p(const Ray &r) const {
    Vector3f e1 = v1 - v0;
    Vector3f e2 = v2 - v0;
    Vector3f pvec = cross(r.d, e2);
    
    // Determinante: se for muito perto de 0, o raio é paralelo ao triângulo
    float det = dot(e1, pvec);
    if (std::abs(det) < 1e-8f) return false;

    float inv_det = 1.0f / det;
    Vector3f tvec = r.o - v0;
    
    // Calcula o parâmetro U e testa os limites
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) return false;

    // Calcula o parâmetro V e testa os limites
    Vector3f qvec = cross(tvec, e1);
    float v = dot(r.d, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) return false;

    // Calcula o T para saber a distância
    float t = dot(e2, qvec) * inv_det;
    
    // Verifica se a colisão ocorreu dentro do alcance visível da câmera
    if (t < r.t_min || t > r.t_max) return false;

    return true;
}

bool Triangle::intersect(const Ray &r, Surfel *sf) const {
    Vector3f e1 = v1 - v0;
    Vector3f e2 = v2 - v0;
    Vector3f pvec = cross(r.d, e2);
    
    float det = dot(e1, pvec);
    if (std::abs(det) < 1e-8f) return false;

    float inv_det = 1.0f / det;
    Vector3f tvec = r.o - v0;
    
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) return false;

    Vector3f qvec = cross(tvec, e1);
    float v = dot(r.d, qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) return false;

    float t = dot(e2, qvec) * inv_det;
    
    if (t < r.t_min || t > r.t_max) return false;

    // Colisão Válida! 
    r.t_max = t; // Encurta o raio para evitar renderizar objetos atrás

    if (sf != nullptr) {
        Point3f p = r(t);
        // Uma vantagem das coordenadas baricêntricas é que o (u,v) 
        // serve perfeitamente para mapear texturas (Point2f) depois!
        *sf = Surfel(p, normal, -r.d, 0.0f, Point2f{u, v}, this);
    }

    return true;
}

const Material *Triangle::get_material() const {
    return material.get();
}

} // namespace gc