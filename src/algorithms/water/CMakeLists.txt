add_library(water_algorithm SHARED
    water.cpp
)

target_include_directories(water_algorithm
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src/algorithms/water
)

target_compile_features(water_algorithm PUBLIC cxx_std_20)
