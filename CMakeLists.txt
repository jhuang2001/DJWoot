cmake_minimum_required(VERSION 3.16)

project(DJWoot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(main)

# TODO: Remove hardcoded paths and use find_package (current workaround used for WSL paths)
if(WIN32)
    find_library(HIDAPI_LIBRARY
        REQUIRED NAMES hidapi hidapi-msvc
    )
    get_filename_component(HIDAPI_LIBRARY_DIR ${HIDAPI_LIBRARY} DIRECTORY)
    find_file(HIDAPI_DLL
        NAMES hidapi.dll libhidapi.dll libhidapi-0.dll
        PATHS ${HIDAPI_LIBRARY_DIR}
    )
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${HIDAPI_DLL}
            $<TARGET_FILE_DIR:main>
    )

    set(WOOTING_RGB_SDK_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-rgb-sdk
    )
    set(WOOTING_ANALOG_SDK_INCLUDE_DIR
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-analog-sdk
    )

    set(WOOTING_RGB_SDK_LIBRARY
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-rgb-sdk/wooting-rgb-sdk64.lib
    )
    set(RGB_SDK_DLL
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-rgb-sdk/wooting-rgb-sdk64.dll
    )
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RGB_SDK_DLL}
            $<TARGET_FILE_DIR:main>
    )
    set(WOOTING_ANALOG_SDK_LIBRARY
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-analog-sdk/wooting_analog_sdk_dist.dll.lib
    )
    set(ANALOG_SDK_DLL
        ${CMAKE_SOURCE_DIR}/external-windows/wooting-analog-sdk/wooting_analog_sdk_dist.dll
    )
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ANALOG_SDK_DLL}
            $<TARGET_FILE_DIR:main>
    )

elseif(UNIX)
    find_path(WOOTING_RGB_SDK_INCLUDE_DIR
        wooting-usb.h
        PATHS /usr/local/include /usr/include
    )

    find_library(WOOTING_RGB_SDK_LIBRARY
        NAMES wooting-rgb-sdk
        PATHS /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
    )

    find_library(HIDAPI_LIBRARY
        NAMES hidapi-libusb
        PATHS /usr/local/lib /usr/local/lib64 /usr/lib /usr/lib64
    )

    if(NOT WOOTING_RGB_SDK_INCLUDE_DIR OR NOT WOOTING_RGB_SDK_LIBRARY)
        message(FATAL_ERROR "Could not find wooting-usb.h or wooting-rgb-sdk library. Install the Wooting RGB SDK or set the appropriate paths.")
    endif()

else()
    message(FATAL_ERROR "Unsupported platform. Project only supports Windows and Unix-like systems")
endif()

target_sources(main PRIVATE src/main.cpp)

target_include_directories(main PRIVATE ${WOOTING_RGB_SDK_INCLUDE_DIR} ${WOOTING_ANALOG_SDK_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${WOOTING_RGB_SDK_LIBRARY} ${HIDAPI_LIBRARY})

# TODO: Workaround for static linking on Windows, allow for cross compilation from WSL
if(WIN32)
    target_link_options(main PRIVATE -static-libgcc -static-libstdc++)
endif()