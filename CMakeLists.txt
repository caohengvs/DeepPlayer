cmake_minimum_required(VERSION 3.20.0)
include(EnvConfig.cmake)

project(HzProject VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(HzEngine)
add_subdirectory(HzUtils)
add_subdirectory(HzLogger)
add_subdirectory(HzApp)
# add_subdirectory(examples)