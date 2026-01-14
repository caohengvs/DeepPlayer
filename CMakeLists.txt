cmake_minimum_required(VERSION 3.20.0)
include(ProjectConfigs.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DeepPlayer VERSION 0.1.0 LANGUAGES C CXX)
add_subdirectory(examples)