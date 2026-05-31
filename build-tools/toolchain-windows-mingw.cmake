# Cross-compile from WSL/Linux to Windows using MinGW-w64.
#
# Usage:
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build-tools/toolchain-windows-mingw.cmake -DCMAKE_BUILD_TYPE=Debug
#
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# If you need the toolchain to search additional MinGW root paths, add them here.
set(CMAKE_FIND_ROOT_PATH "/usr/x86_64-w64-mingw32" "/usr/local/x86_64-w64-mingw32")
