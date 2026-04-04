#include "camera.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "paramset.hpp"
#include <cmath>
#include <optional>

namespace gc {

Camera::Camera(const Point3f &look_from, const Point3f &look_at,
               const Vector3f &vup, const ScreenWindow &screen_window,
               const Film &film, const real_type focal_distance)
    : m_origin{look_from}, m_focal_distance(focal_distance),
      film{std::make_unique<Film>(film)}, m_screen_window{screen_window} {
  Vector3f gaze = look_at - look_from;
  m_w = normalize(gaze);            // Forward
  m_u = normalize(cross(vup, m_w)); // Right
  m_v = normalize(cross(m_w, m_u)); // Up
}

OrthographicCamera::OrthographicCamera(const Point3f &look_from,
                                       const Point3f &look_at,
                                       const Vector3f &vup,
                                       const ScreenWindow &screen_window,
                                       const Film &film,
                                       real_type focal_distance)
    : Camera(look_from, look_at, vup, screen_window, film, focal_distance) {};

Ray OrthographicCamera::generate_ray(int i, int j, int nx, int ny) const {
  real_type u = m_screen_window.l + (m_screen_window.r - m_screen_window.l) *
                                        (static_cast<real_type>(i) + 0.5f) /
                                        static_cast<real_type>(nx);
  real_type v = m_screen_window.b + (m_screen_window.t - m_screen_window.b) *
                                        (static_cast<real_type>(j) + 0.5f) /
                                        static_cast<real_type>(ny);

  Vector3f dir = m_w;
  Point3f origin = m_origin + (u * m_u) + (v * m_v);
  return Ray(origin, dir);
}

/// Calculates the Screen Window from the aspect ratio alone
ScreenWindow screen_window_from_aspect_ratio(real_type ratio) {
  std::array<float, 2> fixed_axis = {-1, 1};               // smaller axis
  std::array<float, 2> var_axis = {-1 * ratio, 1 * ratio}; // bigger axis

  ScreenWindowBuilder builder = ScreenWindowBuilder();
  if (ratio > 1) {
    return builder.set_l(var_axis[0])
        .set_b(fixed_axis[0])
        .set_r(var_axis[1])
        .set_t(fixed_axis[1])
        .build();
  }
  return builder.set_l(fixed_axis[0])
      .set_b(var_axis[0])
      .set_r(fixed_axis[1])
      .set_t(var_axis[1])
      .build();
}

/// Calculates the Screen Window from the images fovy and aspect ratio
ScreenWindow screen_window_from_fovy(real_type fovy, real_type ratio,
                                     real_type focal_distance) {
  real_type fovy_rad = degrees_to_radians(fovy);
  real_type height = std::tan(fovy_rad / 2) * focal_distance;

  return ScreenWindowBuilder()
      .set_l(-ratio * height)
      .set_r(ratio * height)
      .set_b(-height)
      .set_t(height)
      .build();
}

/// Gets the aspect ratio
real_type resolve_aspect_ratio(const Film &film) {
  // Assuming from the Film's dimensions
  Point2i resolution = film.get_resolution();
  return static_cast<real_type>(resolution.x) / resolution.y;
}

ScreenWindow resolve_screen_window(real_type ratio, const Film &film,
                                   const real_type focal_distance,
                                   std::optional<real_type> fovy_opt) {
  if (fovy_opt.has_value()) {
    return screen_window_from_fovy(fovy_opt.value(), ratio, focal_distance);
  }
  return screen_window_from_aspect_ratio(ratio);
}

// TODO : Try using Template Method design pattern
OrthographicCamera *create_orthographic_camera(const ParamSet &ps,
                                               const LookAt &lookat,
                                               const Film &film) {
  Point3f look_at = lookat.look_at;
  Point3f look_from = lookat.look_from;
  Vector3f up = lookat.up;

  real_type focal_distance = ps.retrieve<real_type>("focal_distance", 1.f);

  real_type aspect_ratio =
      ps.retrieve<real_type>("frame_aspectratio", resolve_aspect_ratio(film));
  std::optional<real_type> fovy_opt = std::nullopt;
  if (ps.contains<real_type>("fovy")) {
    fovy_opt = ps.retrieve<real_type>("fovy");
  }

  ScreenWindow screen_window = ps.retrieve<ScreenWindow>(
      "screen_window",
      resolve_screen_window(aspect_ratio, film, focal_distance, fovy_opt));

  return new OrthographicCamera(look_from, look_at, up, screen_window, film,
                                focal_distance);
}

PerspectiveCamera *create_perspective_camera(const ParamSet &ps,
                                             const LookAt &lookat,
                                             const Film &film) {
  Point3f look_at = lookat.look_at;
  Point3f look_from = lookat.look_from;
  Vector3f up = lookat.up;

  real_type focal_distance = ps.retrieve<real_type>("focal_distance", 1.f);

  real_type aspect_ratio =
      ps.retrieve<real_type>("frame_aspectratio", resolve_aspect_ratio(film));

  std::optional<real_type> fovy_opt = std::nullopt;

  if (ps.contains<real_type>("fovy")) {
    fovy_opt = ps.retrieve<real_type>("fovy");
  }
  ScreenWindow screen_window = ps.retrieve<ScreenWindow>(
      "screen_window",
      resolve_screen_window(aspect_ratio, film, focal_distance, fovy_opt));

  return new PerspectiveCamera(look_from, look_at, up, screen_window, film,
                               focal_distance);
}

PerspectiveCamera::PerspectiveCamera(const Point3f &look_from,
                                     const Point3f &look_at,
                                     const Vector3f &vup,
                                     const ScreenWindow &screen_window,
                                     const Film &film,
                                     const real_type focal_distance)
    : Camera(look_from, look_at, vup, screen_window, film, focal_distance) {};

Ray PerspectiveCamera::generate_ray(int i, int j, int nx, int ny) const {

  real_type u = m_screen_window.l + (m_screen_window.r - m_screen_window.l) *
                                        (static_cast<real_type>(i) + 0.5f) /
                                        static_cast<real_type>(nx);
  real_type v = m_screen_window.b + (m_screen_window.t - m_screen_window.b) *
                                        (static_cast<real_type>(j) + 0.5f) /
                                        static_cast<real_type>(ny);

  Point3f origin = m_origin;

  Vector3f dir = (m_focal_distance * m_w) + (u * m_u) + (v * m_v);

  return Ray(origin, dir);
}
} // namespace gc
