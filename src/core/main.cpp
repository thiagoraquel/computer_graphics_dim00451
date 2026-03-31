#include <filesystem>
#include <iomanip>
#include <ios>
#include <iostream>  // std::cout, std::cerr
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "../msg_system/error.hpp"
#include "../msg_system/tcolor.hpp"
#include "app.hpp"
#include "common.hpp"
#include "geometry.hpp"
#include "camera.hpp"

using namespace gc; // Allows us to use 'Ray', 'Point3f', etc., without the gc:: prefix

//void run_camera_and_ray_tests() {
//    std::cout << TColor::colorize("\n=== STARTING UNIT TESTS ===\n", TColor::YELLOW);
//
//    // [1/2] Ray Math Test
//    Point3f r_origin{ 0.0f, 0.0f, 0.0f };
//    Vector3f r_direction{ 1.0f, 2.0f, 3.0f };
//    Ray r(r_origin, r_direction);
//    
//    // Testing P(t) = o + d * t
//    if (r(2.5f).x == 2.5f) { 
//        std::cout << TColor::colorize("  >> [OK] Ray math is correct!\n", TColor::GREEN);
//    }
//
//    // [2/2] Testing Camera Projections
//    std::cout << "\n[2/2] Testing Camera Projections:\n";
//
//    Point3f eye{0, 0, 0};
//    Point3f look_at{0, 0, 1}; 
//    Vector3f up{0, 1, 0};
//    std::vector<real_type> window = {-1.0, 1.0, -1.0, 1.0}; 
//    int nx = 200, ny = 200;
//
//    PerspectiveCamera persp(eye, look_at, up, window);
//
//    // Test the CENTER pixel
//    Ray r_persp = persp.generate_ray(100, 100, nx, ny);
//
//    std::cout << "  Persp Center Ray: " << r_persp << "\n";
//
//    // UPDATED: Using .d instead of .direction()
//    if (std::abs(r_persp.d.x) < 0.01f && r_persp.d.z > 0.9f) {
//        std::cout << TColor::colorize("  >> SUCCESS: Perspective center ray points Forward!\n", TColor::GREEN);
//    } else {
//        std::cout << TColor::colorize("  >> ERROR: Perspective direction is off. Check your m_w calculation.\n", TColor::RED);
//    }
//
//    std::cout << TColor::colorize("\n=== TESTS COMPLETED ===\n", TColor::YELLOW);
//}

void usage(std::string_view msg = "") {
  if (not msg.empty()) {
    std::cout << TColor::colorize(">>> Error: ", TColor::RED) << TColor::colorize(msg, TColor::RED)
              << "\n\n";
  }
  std::cout
    << "Usage: gcrt [<options>] <input_scene_file>\n"
    << "  Rendering simulation options:\n"
    << "    --help                     Print this help text.\n"
    << "    --verbose or -v            Run in verbose mode.\n"
    << "    --cropwindow x0 x1 y0 y1   Specify an image crop window; values must be between 0 and "
       "1.\n"
    << "    --quick                    Reduces quality parameters to render image quickly.\n"
    << "    --outfile <filename>       Write the rendered image to <filename>.\n\n";
  exit(msg.empty() ? 1 : 0);
}

void validate_cropwindow(std::array<std::string_view, 4> arguments,
                         gc::RunningOptions& run_opt_out) {
  // Validate crop values.
  try {
    unsigned short idx{ 0 };
    for (const auto& value : arguments) {
      run_opt_out.crop_window[idx++] = std::stof((std::string)value);
    }
  } catch (const std::invalid_argument& e) {
    usage("could not convert --cropwindow's arguments into real number.");
  } catch (const std::out_of_range& e) {
    usage("at least one of the --cropwindow values are out of range.");
  }
  float& x0{ run_opt_out.crop_window[0] };
  float& x1{ run_opt_out.crop_window[1] };
  float& y0{ run_opt_out.crop_window[2] };
  float& y1{ run_opt_out.crop_window[3] };
  // Check whether the values are in the expected range (in [0,1])...
  if (x0 < 0.F or x0 > 1.F or x1 < 0.F or x1 > 1.F or y0 < 0.F or y0 > 1.F or y1 < 0.F
      or y1 > 1.F) {
    usage("at least one of the --cropwindow values are out of range.");
  }
  // ... and if they obey x0<x1 and y0<y1.
  if (x0 > x1 or y0 > y1) {
    usage("the --cropwindow values doest not comply with x0<x1 and y0<y1.");
  }
  // Finally, turn on flag to indicate we got a crop window via CLI.
  run_opt_out.crop_window_provided = true;
}

void validate_arguments(int argc, char* argv[], gc::RunningOptions& run_options) {
  if (argc == 1) {  // Missing arguments.
    usage();        // Send default message to the user, and exit.
  }
  // Prepare to parse input argumnts.
  std::ostringstream oss;
  bool has_scenefile{ false };  // use must provide a scene file.
  for (int i{ 1 }; i < argc; ++i) {
    std::string option{ gc::str_lowercase(argv[i]) };
    // Parsing cropwindow values.
    if (option == "--cropwindow" or option == "-cropwindow" or option == "--crop"
        or option == "-crop") {
      if (i + 4 >= argc) {  // The option's argument is missing.
        usage("expected at least 4 values after --cropwindow argument.");
      }
      std::array<std::string_view, 4> arguments{ argv[++i], argv[++i], argv[++i], argv[++i] };
      validate_cropwindow(arguments, run_options);
    } else if (option == "--outfile" or option == "-outfile" or option == "-o") {
      if (i + 1 == argc) {  // The option's argument is missing.
        usage("missing filename after --outfile argument.");
      }
      // Get output image file name.
      run_options.outfile = argv[++i];
    } else if (option == "--quickrender" or option == "-quickrender" or option == "-q"
               or option == "--quick" or option == "-quick") {
      run_options.quick_render = true;
    } else if (option == "--verbose" or option == "-v") {
      run_options.verbose = true;
    } else if (option == "--help" or option == "-help" or option == "-h") {
      usage();
    } else {
      // Required: scene file.
      if (not std::filesystem::exists(std::filesystem::path(option))) {
        usage("scene file provided does not exist.");
      }
      run_options.filename = option;
      has_scenefile = true;
    }
  }  // for to traverse the argument list.
  if (not has_scenefile) {
    usage("missing scene file, cannot proceed.");
  }
}

std::string to_string(const gc::RunningOptions& ro) {
  std::ostringstream oss;
  oss << "- cropwindow: [" << ro.crop_window[0] << " " << ro.crop_window[1] << " "
      << ro.crop_window[2] << " " << ro.crop_window[3] << "]\n";
  oss << "- quick rendering: " << std::boolalpha << ro.quick_render << '\n';
  oss << "- verbose: " << std::boolalpha << ro.verbose << '\n';
  oss << "- outfile: " << std::quoted(ro.outfile) << '\n';
  oss << "- scene file: " << std::quoted(ro.filename);
  return oss.str();
}

#include <iostream>


using namespace gc;

int main(int argc, char* argv[]) {
    gc::RunningOptions run_options;
    bool should_run_tests = false;

    // We check if the user just wants to run the tests
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test") should_run_tests = true;
    }


    // Normal Production Flow
    validate_arguments(argc, argv, run_options);
    
    if (run_options.verbose) {
        std::cout << ">>> Running options active.\n";
    }

    MESSAGE("Ray Tracer Teaching Tool -- rt3, v1.0\n");

    gc::App::init_engine(run_options);
    gc::App::run();

    MESSAGE("\n  --> Thanks for using rt3! <--\n");
    return EXIT_SUCCESS;
}


