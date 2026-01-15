project(HzEngine VERSION 1.0 LANGUAGES CXX)
file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
)
find_package(FFMPEG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS} include)
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
include(${CMAKE_SOURCE_DIR}/LibConfig.cmake)

# add_subdirectory(unitTest)