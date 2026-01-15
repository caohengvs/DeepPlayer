project(HzEngine VERSION 1.0 LANGUAGES CXX)
file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
)
find_package(FFMPEG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${FFMPEG_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/HzLogger/include)
target_link_directories(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARY_DIRS}
    ${CMAKE_SOURCE_DIR}/HzLogger/lib
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARIES}
    "$<IF:$<CONFIG:Debug>,HzLoggerd,HzLogger>"
)
include(${CMAKE_SOURCE_DIR}/LibConfig.cmake)

# add_subdirectory(unitTest)