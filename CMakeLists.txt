# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.22)

# Project declaration
project("game_engine"
    LANGUAGES C
    VERSION 1.2.0
)

set(C_FLAGS "-Wall -std=c11 -g")
set(CMAKE_C_FLAGS ${C_FLAGS})

# add cglm include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# set assimp library name
set(ASSIMP_LIBRARY "assimp")


if(ANDROID)
    set(ASSIMP_LIB_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/assimp/${ANDROID_ABI}/libassimp.so
    )

    add_library(
        ${ASSIMP_LIBRARY}
        SHARED
        IMPORTED
    )

    set_property(TARGET assimp PROPERTY IMPORTED_LOCATION ${ASSIMP_LIB_DIR})
endif(ANDROID)

if(LINUX)
    set(OpenGL_GL_PREFERENCE LEGACY)

    find_package(assimp REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)

    if (assimp_FOUND)
        add_library(
            ${ASSIMP_LIBRARY}
            SHARED
            IMPORTED
        )
        set_target_properties(
            ${ASSIMP_LIBRARY}
            PROPERTIES
            IMPORTED_LOCATION
            "${ASSIMP_LIBRARY_DIRS}/libassimp.so"
        )
    endif(assimp_FOUND)
endif(LINUX)

aux_source_directory("src" GAME_ENGINE_SRC_DIR)

add_library(
    # The name of the library.
    game_engine
    # The library as a shared library.
    SHARED
    # source files
    ${GAME_ENGINE_SRC_DIR}
)
set_target_properties(game_engine PROPERTIES LINKER_LANGUAGE C)

# Link Android library
if(ANDROID)
    find_library(
        log-lib
        log
    )

    add_custom_command(
            TARGET "game_engine" POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${ASSIMP_LIB_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    target_link_libraries(
        game_engine

        ${log-lib}
        android
        log
        EGL
        GLESv3
        ${ASSIMP_LIBRARY}
    )
endif(ANDROID)

if(LINUX)
    target_link_libraries(
        main
        ${OPENGL_gl_LIBRARY}
        glfw
        ${ASSIMP_LIBRARY}
    )
endif(LINUX)
