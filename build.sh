#!/bin/bash
set -e

BUILD_DIR="build"
CMAKE_ARGS=("-S" "." "-B" "${BUILD_DIR}" "-DCMAKE_BUILD_TYPE=Debug")

while [[ $# -gt 0 ]]; do
  case "$1" in
    -w|--windows)
      BUILD_DIR="build-windows"
      CMAKE_ARGS=("-S" "." "-B" "${BUILD_DIR}" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_TOOLCHAIN_FILE=build-tools/toolchain-windows-mingw.cmake")
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [options]"
      echo "  -w, --windows   Build for Windows using the MinGW toolchain file"
      echo "  -h, --help           Show this help message"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Usage: $0 [-w|--windows]"
      exit 1
      ;;
  esac
done

cmake "${CMAKE_ARGS[@]}"
cmake --build "${BUILD_DIR}" --config Debug
