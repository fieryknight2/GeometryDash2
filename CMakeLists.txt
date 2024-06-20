CMAKE_MINIMUM_REQUIRED(VERSION 3.25)

PROJECT(GeometryDash2 CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies)

add_executable(GeometryDash2
        # Headers
        include/GeometryDash.h
        include/Window.h
        include/State.h
        include/MainMenuState.h
        include/gui/Button.h

        # Source Files
        src/gui/Button.cpp
        src/MainMenuState.cpp
        src/GeometryDash.cpp
        src/Window.cpp
        src/main.cpp
)

target_link_libraries(GeometryDash2 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        SimpleLogger
)

target_include_directories(GeometryDash2 PRIVATE
        include
        ${SIMPLE_LOGGER_INCLUDE_DIR}
)


if (WIN32)
    message(STATUS "Added command for copying DLLs")
    add_custom_command(TARGET GeometryDash2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE_DIR:GeometryDash2>
    )
    add_custom_command(TARGET GeometryDash2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE_DIR:GeometryDash2>
    )
    add_custom_command(TARGET GeometryDash2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE_DIR:GeometryDash2>
    )
endif ()