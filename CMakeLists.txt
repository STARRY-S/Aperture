# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10)

# Project declaration
project("aperture"
    LANGUAGES C
    VERSION 0.1.3
)

set(C_FLAGS "-Wall -std=c11 -g")
set(CMAKE_C_FLAGS ${C_FLAGS})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# set assimp library name
set(ASSIMP_LIB_NAME "assimp")

# include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory("src"  APERTURE_SRC_DIR)    # library source dir
aux_source_directory("demo" APERTURE_DEMO_DIR)   # demo source dir
aux_source_directory("test" APERTURE_TEST_DIR)   # for unit test

if(ANDROID)
    # Android
    set(ASSIMP_LIB_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/assimp/${ANDROID_ABI}/libassimp.so
    )

    add_library(
        ${ASSIMP_LIB_NAME}
        SHARED
        IMPORTED
    )

    set_property(TARGET assimp PROPERTY IMPORTED_LOCATION ${ASSIMP_LIB_DIR})
elseif(WIN32)
    # Windows
    message(STATUS "WIN32 Not supported yet.")
else()
    # Linux or any other Unix like system
    set(OpenGL_GL_PREFERENCE LEGACY)

    find_package(${ASSIMP_LIB_NAME} REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)

    if (assimp_FOUND)
        add_library(
            ${ASSIMP_LIB_NAME}
            SHARED
            IMPORTED
        )
        set_target_properties(
            ${ASSIMP_LIB_NAME}
            PROPERTIES
            IMPORTED_LOCATION
            "${assimp_LIBRARIES}"
        )
    endif()
endif()

add_library(
    # The name of the library.
    "aperture"
    # The library as a shared library.
    SHARED
    # source files
    ${APERTURE_SRC_DIR}
)
set_target_properties("aperture" PROPERTIES LINKER_LANGUAGE C)

if(ANDROID)
    # Link Android library
    find_library(
        log-lib
        log
    )

    add_custom_command(
        TARGET "aperture"
        POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy
        ${ASSIMP_LIB_DIR}
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    target_link_libraries(
        "aperture"

        ${log-lib}
        android
        log
        EGL
        GLESv3
        ${ASSIMP_LIB_NAME}
    )
elseif(WIN32)
    # Windows
    message(STATUS "WIN32 not supported yet...")
else()
    # Linux, or any other Unix like system

    # Build demo
    add_executable(
        "aperture_demo"
        ${APERTURE_DEMO_DIR}
    )

    # Build unit test
    add_executable(
        "test"
        ${APERTURE_TEST_DIR}
    )

    # link libraries to demo
    target_link_libraries(
        "aperture_demo"
        ${OPENGL_gl_LIBRARY}
        glfw
        "aperture"
        ${ASSIMP_LIB_NAME}
    )

    # link libraries to test
    target_link_libraries(
        "test"
        ${OPENGL_gl_LIBRARY}
        glfw
        "aperture"
        ${ASSIMP_LIB_NAME}
    )

    # link librarys to libgame_engine
    target_link_libraries(
        "aperture"
        ${OPENGL_gl_LIBRARY}
        glfw
        ${ASSIMP_LIB_NAME}
    )

    file(COPY ${CMAKE_SOURCE_DIR}/src/glsl DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/src/backpack DESTINATION ${CMAKE_BINARY_DIR})
endif()
