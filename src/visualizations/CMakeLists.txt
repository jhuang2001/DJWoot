cmake_minimum_required(VERSION 3.16)

add_executable(boid_visualizer
    boid_viz.cpp
    ../algorithms/vector2.cpp
    ../algorithms/boid.cpp
    ../algorithms/flock.cpp
)

target_include_directories(boid_visualizer PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/algorithms)

target_link_libraries(boid_visualizer PRIVATE user32 gdi32)

if(WIN32)
    target_link_options(boid_visualizer PRIVATE -static-libgcc -static-libstdc++)
endif()
