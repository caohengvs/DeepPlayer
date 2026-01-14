project(examples)
add_executable(${PROJECT_NAME} WIN32 main.cpp)
find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

set(PROJECT_DESTINATION_DIR ${CMAKE_SOURCE_DIR})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_DESTINATION_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_DESTINATION_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_DESTINATION_DIR}/bin
        OUTPUT_NAME "$<IF:$<CONFIG:Debug>,${PROJECT_NAME}d,${PROJECT_NAME}>"
    )
    set(OUTPUT_NAME "$<IF:$<CONFIG:Debug>,${PROJECT_NAME}d,${PROJECT_NAME}>")

    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt PATHS ${Qt6_ROOT}/bin)
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "find windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            ${PROJECT_DESTINATION_DIR}/bin/${OUTPUT_NAME}.exe
        )
    else()
        message(WARNING "未找到 windeployqt。部署可能需要手动步骤。")

    endif()
endif()
