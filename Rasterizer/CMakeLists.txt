cmake_minimum_required(VERSION 3.15.0)
project(CPURasterizer)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenMP REQUIRED COMPONENTS CXX)


add_subdirectory(Rasterizer)