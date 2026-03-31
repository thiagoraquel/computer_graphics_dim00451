#include "camera.hpp"
#include "common.hpp"
#include "paramset.hpp"

namespace gc {

Camera::Camera(const Point3f &look_from, const Point3f &look_at,
               const Vector3f &vup, const ScreenWindow &screen_window,
               const Film &film)
    : m_origin{look_from}, film{std::make_unique<Film>(film)},
      m_screen_window{screen_window} {
  std::cout << "Look at: " << look_at << std::endl;
  std::cout << "Look from: " << look_from << std::endl;
  Vector3f gaze = look_at - look_from;
  std::cout << "Gaze: " << gaze << std::endl;
  m_w = normalize(gaze);            // Forward
  m_u = normalize(cross(vup, m_w)); // Right
  m_v = normalize(cross(m_w, m_u)); // Up
}

OrthographicCamera::OrthographicCamera(const Point3f &look_from,
                                       const Point3f &look_at,
                                       const Vector3f &vup,
                                       const ScreenWindow &screen_window,
                                       const Film &film)
    : Camera(look_from, look_at, vup, screen_window, film) {};

Ray OrthographicCamera::generate_ray(int i, int j, int nx, int ny) const {
  real_type u = m_screen_window.l + (m_screen_window.r - m_screen_window.l) *
                                        (static_cast<real_type>(i) + 0.5f) /
                                        static_cast<real_type>(nx);
  real_type v = m_screen_window.b + (m_screen_window.t - m_screen_window.b) *
                                        (static_cast<real_type>(j) + 0.5f) /
                                        static_cast<real_type>(ny);

  // 2. For Orthographic, the direction is constant (the camera's gaze)
  Vector3f dir = m_w;

  // 3. The origin moves along the u and v axes of the camera frame
  // Origin = e + u*U + v*V
  Point3f origin = m_origin + (u * m_u) + (v * m_v);

  return Ray(origin, dir);
}

ScreenWindow resolve_screen_window(const ParamSet &ps) {
  auto has_screen_window = ps.contains<ScreenWindow>("screen_window");
  auto has_aspect_ratio = ps.contains<float>("frame_aspectratio");
  auto has_fovy = ps.contains<float>("fovy");

  if (has_screen_window) {
    auto screen_window = ps.retrieve<ScreenWindow>("screen_window");
    return screen_window;
  }

  if (has_aspect_ratio and has_fovy) {
    // TODO: Stub
    return ScreenWindow();
  }

  if (has_aspect_ratio) {
    float ratio = ps.retrieve<float>("frame_aspectratio");
    std::array<float, 2> fixed_axis = {-1, 1};
    std::array<float, 2> var_axis = {-1 * ratio, 1 * ratio};

    ScreenWindowBuilder builder = ScreenWindowBuilder();
    if (ratio > 1) {
      return builder.set_l(var_axis[0])
          .set_b(fixed_axis[0])
          .set_r(var_axis[0])
          .set_t(fixed_axis[1])
          .build();
    }
    return builder.set_l(fixed_axis[0])
        .set_b(var_axis[0])
        .set_r(fixed_axis[1])
        .set_t(var_axis[1])
        .build();
  }

  // TODO : Find out how to receive the film's dimensions here
  return ScreenWindow();
}

// TODO : Use template method and LookAt attributes extraction
OrthographicCamera *create_orthographic_camera(const ParamSet &ps,
                                               const LookAt &lookat,
                                               const Film &film) {
  Point3f look_at = lookat.look_at;
  Point3f look_from = lookat.look_from;
  Vector3f up = lookat.up;

  std::cout << "Lookout na camera ortografica: " << lookat;

  ScreenWindow screen_window = resolve_screen_window(ps);
  return new OrthographicCamera(look_from, look_at, up, screen_window, film);
}

PerspectiveCamera *create_perspective_camera(const ParamSet &ps,
                                             const LookAt &lookat,
                                             const Film &film) {
  // TODO: Wrap the 'ps' extractions on 'handle' functions
  Point3f look_at = lookat.look_at;
  Point3f look_from = lookat.look_from;
  Vector3f up = lookat.up;

  ScreenWindow screen_window = resolve_screen_window(ps);

  return new PerspectiveCamera(look_from, look_at, up, screen_window, film);
}

PerspectiveCamera::PerspectiveCamera(const Point3f &look_from,
                                     const Point3f &look_at,
                                     const Vector3f &vup,
                                     const ScreenWindow &screen_window,
                                     const Film &film)
    : Camera(look_from, look_at, vup, screen_window, film) {};

Ray PerspectiveCamera::generate_ray(int i, int j, int nx, int ny) const {
  // 1. Map pixel (i, j) to screen space (u, v)
  real_type u = m_screen_window.l + (m_screen_window.r - m_screen_window.l) *
                                        (static_cast<real_type>(i) + 0.5f) /
                                        static_cast<real_type>(nx);
  real_type v = m_screen_window.b + (m_screen_window.t - m_screen_window.b) *
                                        (static_cast<real_type>(j) + 0.5f) /
                                        static_cast<real_type>(ny);

  // 2. For Perspective, the origin is ALWAYS the eye (look_from)
  Point3f origin = m_origin;

  // 3. The direction is the vector pointing from the eye through the (u,v)
  // point In the instructions: dir = (fd)*w + u*U + v*V Assuming focal distance
  // (fd) = 1.0
  real_type fd = 1.0f;
  Vector3f dir = (fd * m_w) + (u * m_u) + (v * m_v);

  return Ray(origin, dir);
}
} // namespace gc
