#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "geometry.hpp"
#include "paramset.hpp"
#include <vector>

namespace gc {

class Camera {
public:
    //== Camera Public Methods
    Camera(const Point3f& look_from, const Point3f& look_at, const Vector3f& vup,
           const std::vector<real_type>& screen_window)
        : m_origin{ look_from } 
    {
        // 1. Determine the Camera Frame (Left-Hand Rule per instructions)
        Vector3f gaze = look_at - look_from;
        m_w = normalize(gaze);             // Forward
        m_u = normalize(cross(vup, m_w));  // Right
        m_v = normalize(cross(m_w, m_u));  // Up

        // 2. Store Screen Window Bounds [l, r, b, t]
        m_l = screen_window[0];
        m_r = screen_window[1];
        m_b = screen_window[2];
        m_t = screen_window[3];
    }

    virtual ~Camera() = default;

    // Pure virtual function: Every camera must know how to shoot a ray
    virtual Ray generate_ray(int i, int j, int nx, int ny) const = 0;

protected:
    // Camera Frame
    Point3f m_origin;
    Vector3f m_u, m_v, m_w;

    // Screen Window Bounds
    real_type m_l, m_r, m_b, m_t;
};

// --- Derived Classes will go here (Perspective/Orthographic) ---

class OrthographicCamera : public Camera {
public:
    OrthographicCamera(const Point3f& look_from, const Point3f& look_at, const Vector3f& vup,
                       const std::vector<real_type>& screen_window)
        : Camera(look_from, look_at, vup, screen_window) 
    { /* Constructor logic is handled by base class */ }

    Ray generate_ray(int i, int j, int nx, int ny) const override {
        // 1. Map pixel (i, j) to screen space (u, v)
        // We add 0.5 to sample the CENTER of the pixel
        real_type u = m_l + (m_r - m_l) * (static_cast<real_type>(i) + 0.5f) / static_cast<real_type>(nx);
        real_type v = m_b + (m_t - m_b) * (static_cast<real_type>(j) + 0.5f) / static_cast<real_type>(ny);

        // 2. For Orthographic, the direction is constant (the camera's gaze)
        Vector3f dir = m_w;

        // 3. The origin moves along the u and v axes of the camera frame
        // Origin = e + u*U + v*V
        Point3f origin = m_origin + (u * m_u) + (v * m_v);

        return Ray(origin, dir);
    }
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const Point3f& look_from, const Point3f& look_at, const Vector3f& vup,
                      const std::vector<real_type>& screen_window)
        : Camera(look_from, look_at, vup, screen_window) 
    { /* Base class handles the ONB and bounds */ }

    Ray generate_ray(int i, int j, int nx, int ny) const override {
        // 1. Map pixel (i, j) to screen space (u, v)
        real_type u = m_l + (m_r - m_l) * (static_cast<real_type>(i) + 0.5f) / static_cast<real_type>(nx);
        real_type v = m_b + (m_t - m_b) * (static_cast<real_type>(j) + 0.5f) / static_cast<real_type>(ny);

        // 2. For Perspective, the origin is ALWAYS the eye (look_from)
        Point3f origin = m_origin;

        // 3. The direction is the vector pointing from the eye through the (u,v) point
        // In the instructions: dir = (fd)*w + u*U + v*V
        // Assuming focal distance (fd) = 1.0
        real_type fd = 1.0f;
        Vector3f dir = (fd * m_w) + (u * m_u) + (v * m_v);

        return Ray(origin, dir);
    }
};

} // namespace gc

#endif // CAMERA_HPP