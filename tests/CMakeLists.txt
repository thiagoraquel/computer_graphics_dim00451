include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Helper function to add tests easily
function(add_rt3_test TEST_NAME SRC_FILE)
    add_executable(${TEST_NAME} ${SRC_FILE})
    target_link_libraries(${TEST_NAME} GTest::gtest_main rt3_core)
    gtest_discover_tests(${TEST_NAME})
endfunction()

# Add your tests here
add_rt3_test(screen_window_test screen_window_test.cpp)
