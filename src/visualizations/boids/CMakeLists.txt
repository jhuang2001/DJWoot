cmake_minimum_required(VERSION 3.16)

add_executable(boid_visualizer
    boid_viz.cpp
)

target_include_directories(boid_visualizer PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(boid_visualizer PRIVATE
    boids_algorithm
    water_algorithm
    user32
    gdi32
)

if(WIN32)
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libgcc_s_seh-1.dll
        OUTPUT_VARIABLE MINGW_LIBGCC_S_DLL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++-6.dll
        OUTPUT_VARIABLE MINGW_LIBSTDCXX_DLL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libwinpthread-1.dll
        OUTPUT_VARIABLE MINGW_LIBWINPTHREAD_DLL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    add_custom_command(TARGET boid_visualizer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${MINGW_LIBGCC_S_DLL}
            ${MINGW_LIBSTDCXX_DLL}
            ${MINGW_LIBWINPTHREAD_DLL}
            $<TARGET_FILE_DIR:boid_visualizer>
    )
endif()

if(WIN32)
    target_link_options(boid_visualizer PRIVATE -static-libgcc -static-libstdc++)
endif()
