cmake_minimum_required(VERSION 3.15.0)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(CPURasterizer)

# Split dependencies file to make it easier for packagers
include(CMake/Dependencies.cmake)
find_package(OpenMP REQUIRED COMPONENTS CXX)

add_subdirectory(Rasterizer)