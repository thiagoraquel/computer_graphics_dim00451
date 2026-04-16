# =============[ Core Library ]=============
add_library( rt3_core
   core/app.cpp
   core/background.cpp
   core/film.cpp
   core/camera.cpp
   core/image_io.cpp
   core/parser.cpp
   core/sphere.cpp
   core/triangle.cpp
   core/flatmaterial.cpp
   msg_system/error.cpp
   extras/pbar.cpp
)

target_compile_features( rt3_core PUBLIC cxx_std_17 )
target_compile_options( rt3_core PUBLIC -Wall -pedantic -O3)
target_compile_definitions( rt3_core PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

target_include_directories( rt3_core PUBLIC 
    ${CMAKE_SOURCE_DIR}/lib_tinyxml2
    ${CMAKE_SOURCE_DIR}/lib_lodepng
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib_glm
    ${CMAKE_CURRENT_SOURCE_DIR}/msg_system
    ${CMAKE_CURRENT_SOURCE_DIR}/extras
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib_cybase
)

target_link_libraries( rt3_core PUBLIC ${LIB_TINYXML2} ${LIB_LODEPNG} )

# =============[ Main App ]=============
add_executable( ${APP_NAME} core/main.cpp )
target_link_libraries( ${APP_NAME} PRIVATE rt3_core )
set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
