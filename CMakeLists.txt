# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10)

# Project declaration
project("aperture"
    LANGUAGES C
    VERSION 0.1.3
)

set(C_FLAGS "-Wall -std=c11 -g -lm")
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
else(ANDROID)
    # Linux/Unix and Win32 system
    set(OpenGL_GL_PREFERENCE LEGACY)

    find_package(${ASSIMP_LIB_NAME} REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(OpenAL REQUIRED)
    find_package(ALUT REQUIRED)

    # Find FFMpeg libraries
    find_library(AVCODEC_LIBRARY avcodec)
    message(STATUS "AVCodec library: ${AVCODEC_LIBRARY}")
    find_library(AVFORMAT_LIBRARY avformat)
    message(STATUS "AVFormat library: ${AVFORMAT_LIBRARY}")
    find_library(AVUTIL_LIBRARY avutil)
    message(STATUS "AVUtil library: ${AVUTIL_LIBRARY}")
    find_library(AVDEVICE_LIBRARY avdevice)
    message(STATUS "AVDevice library: ${AVDEVICE_LIBRARY}")

    # FFMpeg include directories
    find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
    find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
    find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
    find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)

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
        if(WIN32)
            set_target_properties(
                ${ASSIMP_LIB_NAME}
                PROPERTIES
                IMPORTED_IMPLIB
                "${assimp_LIBRARIES}"
            )
        endif(WIN32)
    endif(assimp_FOUND)
endif(ANDROID)

if(ANDROID)
    add_library(
        # The name of the library.
        "aperture"
        # The library as a shared library.
        SHARED
        # source files
        ${APERTURE_SRC_DIR}
        "demo/demo_utils.c"
        "demo/demo_light.c"
        "demo/native-lib.c"
    )
else(ANDROID)
    add_library(
        # The name of the library.
        "aperture"
        # The library as a shared library.
        SHARED
        # source files
        ${APERTURE_SRC_DIR}
    )
endif(ANDROID)

set_target_properties("aperture" PROPERTIES LINKER_LANGUAGE C)

if(ANDROID)
    # Link Android library
    find_library(
        log_lib
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

        ${log_lib}
        android
        log
        EGL
        GLESv3
        ${ASSIMP_LIB_NAME}
    )
else(ANDROID)
    # Linux/Unix Win32 system

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
        ${OPENAL_LIBRARY}
        ${ALUT_LIBRARIES}
    )
    set_target_properties("aperture_demo" PROPERTIES LINKER_LANGUAGE C)

    # link libraries to test
    target_link_libraries(
        "test"
        ${OPENGL_gl_LIBRARY}
        glfw
        "aperture"
        ${ASSIMP_LIB_NAME}
        # ${OPENAL_LIBRARY}
        ${ALUT_LIBRARIES}
    )
    set_target_properties("test" PROPERTIES LINKER_LANGUAGE C)

    # link librarys to libaperture
    target_link_libraries(
        "aperture"
        ${OPENGL_gl_LIBRARY}
        glfw
        ${ASSIMP_LIB_NAME}
        ${OPENAL_LIBRARY}
        ${ALUT_LIBRARIES}

        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${AVDEVICE_LIBRARY}
    )
    set_target_properties("aperture" PROPERTIES LINKER_LANGUAGE C)

    file(COPY ${CMAKE_SOURCE_DIR}/demo/glsl DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/mc DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/sound DESTINATION ${CMAKE_BINARY_DIR})
endif(ANDROID)
