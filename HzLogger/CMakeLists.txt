project(HzLogger VERSION 1.0.0 LANGUAGES CXX)

file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_HEADER_ONLY)

target_include_directories(${PROJECT_NAME} PRIVATE include)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)

include(${CMAKE_SOURCE_DIR}/ProjConfig.cmake)
