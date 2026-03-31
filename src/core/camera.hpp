#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp"
#include "film.hpp"
#include "geometry.hpp"
#include <memory>

namespace gc {

struct LookAt {
  Point3f look_at;
  Point3f look_from;
  Vector3f up;
};
inline std::ostream& operator<<(std::ostream& os, const LookAt& v) {
  os << "Look At" << v.look_at <<
   "Look From" << v.look_from <<
   "Up" << v.up;
  return os;
}

class Camera {
public:
  enum class camera_type_e : byte { ORTHOGRAPHIC = 0, PERSPECTIVE };

  //== Camera Public Methods
  Camera(const Point3f &look_from, const Point3f &look_at, const Vector3f &vup,
         const ScreenWindow &screen_window, const Film& film);

  virtual ~Camera() = default;

  // Pure virtual function: Every camera must know how to shoot a ray
  virtual Ray generate_ray(int i, int j, int nx, int ny) const = 0;

  /// Returns the film's resolution
  Point2i get_film_resoulution() const { return film->get_resolution(); }

  Point3f m_origin;
  Vector3f m_u, m_v, m_w;

  /// Camera Frame
  std::unique_ptr<Film> film;

  // Screen Window Bounds
  ScreenWindow m_screen_window;
};

class OrthographicCamera : public Camera {
public:
  OrthographicCamera(const Point3f &look_from, const Point3f &look_at,
                     const Vector3f &vup, const ScreenWindow &screen_window, const Film& film);

  Ray generate_ray(int i, int j, int nx, int ny) const override;
};

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const Point3f &look_from, const Point3f &look_at,
                    const Vector3f &vup, const ScreenWindow &screen_window, const Film& film);

  Ray generate_ray(int i, int j, int nx, int ny) const override;
};

/// Factory creation
Camera *create_camera(const ParamSet &ps);
OrthographicCamera *create_orthographic_camera(const ParamSet &ps, const LookAt &lookat, const Film &film);
PerspectiveCamera *create_perspective_camera(const ParamSet &ps, const LookAt &lookat, const Film &film);
ScreenWindow resolve_screen_window(const ParamSet &ps); // NOTE: Might not need to be here

} // namespace gc

#endif // CAMERA_HPP
