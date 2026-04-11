#include <chrono>
#include <memory>
#include <sstream>
#include <string_view>
#include <vector>

#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

#include "../msg_system/error.hpp"

#include "app.hpp"
#include "background.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "film.hpp"
#include "geometry.hpp"
#include "paramset.hpp"
#include "parser.hpp"
#include "sphere.hpp"

namespace gc {

//=== App's static members declaration and initialization.
App::AppState App::m_current_block_state = AppState::Uninitialized;
RunningOptions App::m_current_run_options;
std::unique_ptr<RenderOptions> App::m_render_options;

/// Check whether the current state has been intialized.
bool App::check_in_initialized_state(std::string_view func_name) {
  if (m_current_block_state == AppState::Uninitialized) {
    std::ostringstream oss;
    oss << "App::init() must be called before " << func_name << ". Ignoring...";
    ERROR(oss.str());
    return false;
  }
  return true;
}

/// Check whether the current state corresponds to setup section.
bool App::check_in_setup_block_state(std::string_view func_name) {
  check_in_initialized_state(func_name);
  if (m_current_block_state == AppState::WorldBlock) {
    std::ostringstream oss;
    oss << "Rendering setup cannot happen inside World Definition block; ";
    oss << func_name << " not allowed. Ignoring...";
    ERROR(oss.str());
    return false;
  }
  return true;
}

/// Check whether the current state corresponds to the world section.
bool App::check_in_world_block_state(std::string_view func_name) {
  check_in_initialized_state(func_name);
  if (m_current_block_state == AppState::SetupBlock) {
    std::ostringstream oss;
    oss << "Scene description must happen inside World Definition block; ";
    oss << func_name << " not allowed. Ignoring...";
    ERROR(oss.str());
    return false;
  }
  return true;
}

//=== App's public methods implementation
void App::init_engine(const RunningOptions &run_options) {
  // Save running option sent from the main().
  m_current_run_options = run_options;
  // Check current machine state.
  if (m_current_block_state != AppState::Uninitialized) {
    ERROR("App::init_engine() has already been called! ");
  }
  // Set proper machine state
  m_current_block_state = AppState::SetupBlock;
  // Preprare render infrastructure for a new scene.
  m_render_options = std::make_unique<RenderOptions>();
  // Create a new initial GS
  // m_current_gs = GraphicsState();
  MESSAGE("[1] Rendering engine initiated.\n");
}

void App::run() {
  // Try to load and parse the scene from a file.
  MESSAGE("[2] Beginning scene file parsing...\n");
  // Recall that the file name comes from the running option struct.
  parse_scene_file(m_current_run_options.filename.c_str());
}

void App::world_begin(const ParamSet &ps) {
  check_in_setup_block_state("App::world_begin()");
  m_current_block_state = AppState::WorldBlock; // correct machine state.
  hard_engine_reset();
}

/// Erase temporary engine states so that we may render another scene with the
/// same configuration.
void App::hard_engine_reset() {
  // Render options reset
  // TODO: in the future.
}

void App::world_end(const ParamSet &ps) {
  MESSAGE(
      "====================================================================");
  MESSAGE("   Parsing Phase has ended. Rendering process starts now...");
  MESSAGE(
      "====================================================================");

  check_in_world_block_state("App::world_end()");

  // ===============================================================
  // 1) Create the integrator.
  // 2) Create the scene (requires the list of objects and background)
  // ===============================================================
  // For now, we create the film here but in the future it will be
  // instantiated somewhere else.
  // 1. Create the film
  Film *film = make_film(m_render_options->actors["film"]);

  if (film == nullptr) {
    ERROR("App::setup_camera(): Unable to create film.");
    return; // Exit if film creation failed
  }
  MESSAGE("Film created");

  LookAt *lookat = make_look_at(m_render_options->actors["look_at"]);

  // 2. Create the camera
  MESSAGE("WIll create camera");
  Camera *camera =
      make_camera(m_render_options->actors["camera"], film, lookat);

  if (camera == nullptr) {
    ERROR("App::setup_camera(): Unable to create camera.");
    return;
  }
  m_render_options->camera.reset(camera);
  MESSAGE("Camera created");

  std::vector<std::unique_ptr<Primitive>> objs = make_objects(m_render_options->primitives);

  // TODO : Provavelmente errado
  m_render_options->objects = std::move(objs);

  // The scene has already been parsed and properly set up. It's time to render
  // the scene. [1] Create the integrator. [2] Create the scene. [3] Run
  // integrator if previous instantiations went ok
  bool scene_and_integrator_ok{true}; // THIS is a STUB.
  MESSAGE("Integrator shit done");
  if (scene_and_integrator_ok) {
    MESSAGE("    Parsing scene successfuly done!\n");
    MESSAGE("[2] Starting ray tracing progress.\n");
    MESSAGE("    Ray tracing is usually a slow process, please be patient: \n");
    //================================================================================
    auto start = std::chrono::steady_clock::now();
    // m_integrator->render(*m_scene);
    render();
    auto end = std::chrono::steady_clock::now();
    //================================================================================
    auto diff = end - start; // Store the time difference between start and end
    // Seconds
    auto diff_sec = std::chrono::duration_cast<std::chrono::seconds>(diff);
    MESSAGE("    Time elapsed: " + std::to_string(diff_sec.count()) +
            " seconds (" +
            std::to_string(
                std::chrono::duration<double, std::milli>(diff).count()) +
            " ms) \n");
  }
  // [4] Basic clean up, preparing for new rendering, in case we have
  // several scene setup + world in a single input scene file.
  m_current_block_state = AppState::SetupBlock; // correct machine state.
}

void App::film(const ParamSet &ps) {
  if (not check_in_setup_block_state("App::film()")) {
    return;
  }
  // Store the ps associated with camera for later retrieval.
  m_render_options->actors["film"] = ps;
  if (m_current_run_options.verbose) {
    auto type = ps.retrieve<std::string>("type", "unknown");
    std::cout << ">>> film type: " << std::quoted(type) << '\n';
    // TODO: Add the rest of the logs
  }
}

void App::camera(const ParamSet &ps) {
  if (not check_in_setup_block_state("App::camera()")) {
    return;
  }
  // Store the ps associated with camera for later retrieval.
  m_render_options->actors["camera"] = ps;
  if (m_current_run_options.verbose) {
    auto type = ps.retrieve<std::string>("type", "unknown");
    std::cout << ">>> camera type: " << std::quoted(type) << '\n';
  }
}

void App::object(const ParamSet &ps) {
  // Store the ps associated with the objects for later retrieval.
  m_render_options->primitives.push_back(ps);
  if (m_current_run_options.verbose) {
    // TODO : Add logs
  }
}

// Atenção!! Está guardando o material na mesma lista dos objects!!
void App::material(const ParamSet &ps) {
  m_render_options->primitives.push_back(ps);
  if (m_current_run_options.verbose) {
    // TODO : Add logs
  }
}

void App::look_at(const ParamSet &ps) {
  if (not check_in_setup_block_state("App::look_at()")) {
    return;
  }
  // Store the ps associated with camera for later retrieval.
  m_render_options->actors["look_at"] = ps;
  if (m_current_run_options.verbose) {
    // TODO : Add Logs
  }
}

void App::background(const ParamSet &ps) {
  check_in_world_block_state("App::background");

  auto type = ps.retrieve<std::string>("type", "unknown");
  if (type == "unknown") {
    ERROR(
        "API::background(): Missing \"type\" specificaton for the background.");
  }
  Background *bkg{nullptr};
  if (type == "single_color" or type == "4_colors") {
    bkg = create_color_background(type, ps);
  } else {
    WARNING(std::string{"API::background(): unknown background type \""} +
            type + std::string{"\" provided; assuming colored background."});
    bkg = create_color_background(type, ps);
  }
  // Store current background objec.
  m_render_options->background.reset(bkg);
}

void App::render() {
  Camera *camera = m_render_options->camera.get();
  Film *film = camera->film.get();
  auto film_resolution = film->get_resolution();
  auto w = film_resolution.x;
  auto h = film_resolution.y;

  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      auto ray{camera->generate_ray(i, j, w, h)};
      //std::cout << "Ray Gerado: " << ray << std::endl;
      float u = float(i) / float(w - 1);
      float v = float(j) / float(h - 1);
      auto color = m_render_options->background->sampleUV(u, v);
      
      for (const auto &obj : m_render_options->objects) {
        if (obj->intersect_p(ray)) {
            color = ColorXYZ(255,0,0); // Fica vermelho se bater
        }
}

      camera->film->add_sample(Point2i{i, j}, color);
    }
  }
  camera->film->write_image();
}

Film *App::make_film(const ParamSet &ps) {
  Film *film{nullptr};
  auto film_type = ps.retrieve<std::string>("type");
  if (film_type == "image") {
    film = create_film(ps);
  } else {
    WARNING(std::string{"Film \""} + film_type + std::string{"\" unknown."});
  }
  std::cout << "DEBUG - Resolution do filme na criacao : "
            << film->get_resolution().x << ", " << film->get_resolution().y
            << std::endl;
  return film;
}

Camera *App::make_camera(const ParamSet &ps, Film *film, LookAt *lookat) {
  Camera *camera{nullptr};
  std::string camera_type = ps.retrieve<std::string>("type", "orthographic");
  if (camera_type == "orthographic") {
    camera = create_orthographic_camera(ps, *lookat, *film);
  } else if (camera_type == "perspective") {
    camera = create_perspective_camera(ps, *lookat, *film);
  } else {
    WARNING(std::string{" \""} + camera_type + std::string{"\" unknown."});
  }
  return camera;
}

//std::shared_ptr<Material> App::make_material(const ParamSet &ps) {
//  if (ps.retrieve<std::string>("type") == "flat") {
//    auto color = ps.retrieve<ColorXYZ>("color");
//    return std::make_shared<Material>();
//  }
//}
std::vector<std::unique_ptr<Primitive>> App::make_objects(const std::vector<ParamSet>& param_sets) {
    std::vector<std::unique_ptr<Primitive>> objects;
    objects.reserve(param_sets.size());

    for (const auto& ps : param_sets) {
        if (ps.retrieve<std::string>("type") == "sphere") {
            auto center = ps.retrieve<Point3f>("center");
            auto radius = ps.retrieve<real_type>("radius");
            objects.push_back(std::make_unique<Sphere>(center,radius));
        }
    }
    return objects;
}

LookAt *App::make_look_at(const ParamSet &ps) {
  LookAt *la = new LookAt();

  Point3f from = ps.retrieve<Point3f>("look_from");
  Point3f target = ps.retrieve<Point3f>("look_at");
  Vector3f up = ps.retrieve<Vector3f>("up");
  la->look_from = from;
  la->look_at = target;
  la->up = up;
  return la;
}

} // namespace gc
