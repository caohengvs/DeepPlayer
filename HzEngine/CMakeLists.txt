project(HzEngine VERSION 1.0 LANGUAGES CXX)
file(GLOB SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
)
target_compile_definitions(${PROJECT_NAME} PRIVATE __STDC_CONSTANT_MACROS)
find_package(FFMPEG REQUIRED)
find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${FFMPEG_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/HzLogger/include)
target_link_directories(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARY_DIRS}
    ${CMAKE_SOURCE_DIR}/HzLogger/lib
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${FFMPEG_LIBRARIES}
    ${OpenCV_LIBS}
    "$<IF:$<CONFIG:Debug>,HzLoggerd,HzLogger>"
)
include(${CMAKE_SOURCE_DIR}/LibConfig.cmake)

# add_subdirectory(unitTest)