#include <gtest/gtest.h>
#include <cmath>
#include "camera.hpp"
#include "paramset.hpp"
#include "film.hpp"
#include "geometry.hpp"

using namespace gc;

class ScreenWindowTest : public ::testing::Test {
protected:
    LookAt default_lookat{ Point3f(0.f, 0.f, 0.f), Point3f(0.f, 0.f, 1.f), Vector3f(0.f, 1.f, 0.f) };
    Film default_film{ {800, 450}, "output.png", Film::image_type_e::PNG };
    
    // helper to compare floats
    bool is_close(float a, float b, float epsilon = 1e-4) const {
        return std::abs(a - b) < epsilon;
    }
};

// Case 1: Testing calculation purely from Aspect Ratio (Ratio > 1) 
// Resolution is 800x450, ratio is 1.7777...
TEST_F(ScreenWindowTest, FromAspectRatioGreater1) {
    ParamSet ps;
    ps.assign<real_type>("focal_distance", 1.0f);
    
    auto* cam = create_orthographic_camera(ps, default_lookat, default_film);
    
    // Expected for ratio > 1:
    // l = -ratio, r = ratio
    // b = -1, t = 1
    float ratio = 1920.0f / 1080.0f;
    EXPECT_TRUE(is_close(cam->m_screen_window.l, -ratio));
    EXPECT_TRUE(is_close(cam->m_screen_window.r, ratio));
    EXPECT_TRUE(is_close(cam->m_screen_window.b, -1.0f));
    EXPECT_TRUE(is_close(cam->m_screen_window.t, 1.0f));
    
    delete cam;
}

// Case 2: Testing calculation purely from Aspect Ratio (Ratio < 1)
TEST_F(ScreenWindowTest, FromAspectRatioLess1) {
    ParamSet ps;
    ps.assign<real_type>("focal_distance", 1.0f);
    
    Film tall_film{ {450, 800}, "output.png", Film::image_type_e::PNG };
    auto* cam = create_orthographic_camera(ps, default_lookat, tall_film);
    
    // Expected for ratio < 1:
    // l = -1, r = 1
    // b = -ratio, t = ratio 
    float ratio = 450.0f / 800.0f;
    EXPECT_TRUE(is_close(cam->m_screen_window.l, -1.0f));
    EXPECT_TRUE(is_close(cam->m_screen_window.r, 1.0f));
    EXPECT_TRUE(is_close(cam->m_screen_window.b, -ratio));
    EXPECT_TRUE(is_close(cam->m_screen_window.t, ratio));
    
    delete cam;
}

// Case 3: Testing calculation from FovY provided in ParamSet
TEST_F(ScreenWindowTest, FromFovy) {
    ParamSet ps;
    ps.assign<real_type>("focal_distance", 2.0f); // Non-default focal distance
    ps.assign<real_type>("fovy", 90.0f); // 90 degrees fov y
    
    auto* cam = create_orthographic_camera(ps, default_lookat, default_film);
    
    // fovy_rad = 90 * pi / 180 = pi / 2
    // height = tan(pi / 4) * focal_distance = 1.0 * 2.0 = 2.0
    // l = -ratio * height
    // r = ratio * height
    // b = -height
    // t = height
    float ratio = 800.0f / 450.0f;
    float height = 2.0f; // tan(45 steps) is exactly 1, but float might be slightly off due to pi approx
    
    EXPECT_TRUE(is_close(cam->m_screen_window.l, -ratio * height));
    EXPECT_TRUE(is_close(cam->m_screen_window.r, ratio * height));
    EXPECT_TRUE(is_close(cam->m_screen_window.b, -height));
    EXPECT_TRUE(is_close(cam->m_screen_window.t, height));
    
    delete cam;
}

// Case 4: Testing provided screen_window overriding others
TEST_F(ScreenWindowTest, ProvidedScreenWindow) {
    ParamSet ps;
    ps.assign<real_type>("fovy", 90.0f); // Should be ignored because screen_window is provided
    
    ScreenWindow expected_sw{-2.0f, -1.5f, 2.0f, 1.5f};
    ps.assign<ScreenWindow>("screen_window", expected_sw);
    
    auto* cam = create_orthographic_camera(ps, default_lookat, default_film);
    
    EXPECT_TRUE(is_close(cam->m_screen_window.l, -2.0f));
    EXPECT_TRUE(is_close(cam->m_screen_window.r, 2.0f));
    EXPECT_TRUE(is_close(cam->m_screen_window.b, -1.5f));
    EXPECT_TRUE(is_close(cam->m_screen_window.t, 1.5f));
    
    delete cam;
}
