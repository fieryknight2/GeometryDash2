CMAKE_MINIMUM_REQUIRED(VERSION 3.25)

PROJECT(GeometryDash2 CXX)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies)

set(GEOMETRYDASH2_SOURCES
        # Headers
        include/GeometryDash.h
        include/Window.h
        include/State.h
        include/MainMenuState.h
        include/PlayState.h
        include/gui/Button.h
        include/gui/Slider.h
        include/game/GameObject.h
        include/game/Arena.h
        include/game/ArenaItem.h
        include/game/PauseState.h
        include/game/SettingsState.h
        include/game/Collision.h
        include/game/Player.h
        include/AssetManager.h
        include/gui/Panel.h
        include/OptionsState.h

        # Source Files
        src/AssetManager.cpp
        src/game/Player.cpp
        src/game/PauseState.cpp
        src/game/Arena.cpp
        src/game/ArenaItem.cpp
        src/game/SettingsState.cpp
        src/game/Collision.cpp
        src/gui/Button.cpp
        src/gui/Panel.cpp
        src/gui/Slider.cpp
        src/PlayState.cpp
        src/OptionsState.cpp
        src/MainMenuState.cpp
        src/GeometryDash.cpp
        src/Window.cpp
)

add_executable(GeometryDash2
        ${GEOMETRYDASH2_SOURCES}
        src/main.cpp
        include/OptionsState.h
)

target_link_libraries(GeometryDash2 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        tinyxml2::tinyxml2
        SimpleLogger
)

target_include_directories(GeometryDash2 PRIVATE
        include
        ${SIMPLE_LOGGER_INCLUDE_DIR}
        ${TINYXML2_INCLUDE_DIR}
)

if (CMakeBuildType STREQUAL "Release")
    target_compile_options(GeometryDash2 PRIVATE -DNDEBUG)
endif ()


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
    add_custom_command(TARGET GeometryDash2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:tinyxml2>
            $<TARGET_FILE_DIR:GeometryDash2>
    )
endif ()

# Install
add_custom_command(TARGET GeometryDash2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:GeometryDash2>/assets
)


option(DEVTEST "For easily creating/testing features" OFF)
if (DEVTEST)
    message(STATUS "Building Devtest")
    add_executable(Test
            src/test.cpp
            ${GEOMETRYDASH2_SOURCES}
    )

    target_link_libraries(Test PRIVATE
            sfml-graphics
            sfml-window
            sfml-system
            tinyxml2::tinyxml2
            SimpleLogger
    )

    target_include_directories(Test PRIVATE
            include
            ${SIMPLE_LOGGER_INCLUDE_DIR}
            ${TINYXML2_INCLUDE_DIR}
    )

    target_compile_definitions(Test PRIVATE DEVTEST)
endif ()
