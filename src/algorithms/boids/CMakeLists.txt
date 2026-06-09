add_library(boids_algorithm SHARED
    boid.cpp
    flock.cpp
    vector2.cpp
)

target_include_directories(boids_algorithm
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(boids_algorithm PUBLIC cxx_std_20)
