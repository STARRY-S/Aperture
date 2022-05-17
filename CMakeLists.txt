# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10)

# Project declaration
project("aperture"
    LANGUAGES C
    VERSION 0.1.3
)

set(C_FLAGS "-Wall -std=c11 -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(ASSIMP_LIB     "assimp")
set(FREETYPE_LIB   "freetype")
set(OPENAL_LIB     "openal")

set(AVCODEC_LIB    "avcodec")
set(AVDEVICE_LIB   "avdevice")
set(AVFILTER_LIB   "avfilter")
set(AVFORMAT_LIB   "avformat")
set(AVUTIL_LIB     "avutil")
set(SWRESAMPLE_LIB "swresample")
set(SWSCALE_LIB    "swscale")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory("src"  APERTURE_SRC_DIR)    # library source dir
aux_source_directory("demo" APERTURE_DEMO_DIR)   # demo source dir
aux_source_directory("test" APERTURE_TEST_DIR)   # for unit test

if(ANDROID)
    # Android
    set(CMAKE_C_FLAGS "${C_FLAGS}")
    set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libs")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

    set(ASSIMP_LIB_DIR     ${LIBS_DIR}/assimp/${ANDROID_ABI}/libassimp.so)
    set(AVCODEC_LIB_DIR    ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libavcodec.so)
    set(AVDEVICE_LIB_DIR   ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libavdevice.so)
    set(AVFILTER_LIB_DIR   ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libavfilter.so)
    set(AVFORMAT_LIB_DIR   ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libavformat.so)
    set(AVUTIL_LIB_DIR     ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libavutil.so)
    set(SWRESAMPLE_LIB_DIR ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libswresample.so)
    set(SWSCALE_LIB_DIR    ${LIBS_DIR}/ffmpeg/${ANDROID_ABI}/libswscale.so)
    set(FREETYPE_LIB_DIR   ${LIBS_DIR}/freetype2/${ANDROID_ABI}/libfreetype.so)
    set(OPENAL_LIB_DIR     ${LIBS_DIR}/openal/${ANDROID_ABI}/libopenal.so)

    add_library(${ASSIMP_LIB}   SHARED IMPORTED)
    add_library(${AVCODEC_LIB}  SHARED IMPORTED)
    add_library(${AVDEVICE_LIB} SHARED IMPORTED)
    add_library(${AVFILTER_LIB} SHARED IMPORTED)
    add_library(${AVFORMAT_LIB} SHARED IMPORTED)
    add_library(${AVUTIL_LIB}   SHARED IMPORTED)
    add_library(${SWRESAMPLE_LIB} SHARED IMPORTED)
    add_library(${SWSCALE_LIB}  SHARED IMPORTED)
    add_library(${FREETYPE_LIB} SHARED IMPORTED)
    add_library(${OPENAL_LIB}   SHARED IMPORTED)

    set_property(TARGET ${ASSIMP_LIB}   PROPERTY IMPORTED_LOCATION ${ASSIMP_LIB_DIR})
    set_property(TARGET ${AVCODEC_LIB}  PROPERTY IMPORTED_LOCATION ${AVCODEC_LIB_DIR})
    set_property(TARGET ${AVDEVICE_LIB} PROPERTY IMPORTED_LOCATION ${AVDEVICE_LIB_DIR})
    set_property(TARGET ${AVFILTER_LIB} PROPERTY IMPORTED_LOCATION ${AVFILTER_LIB_DIR})
    set_property(TARGET ${AVFORMAT_LIB} PROPERTY IMPORTED_LOCATION ${AVFORMAT_LIB_DIR})
    set_property(TARGET ${AVUTIL_LIB}   PROPERTY IMPORTED_LOCATION ${AVUTIL_LIB_DIR})
    set_property(TARGET ${SWRESAMPLE_LIB} PROPERTY IMPORTED_LOCATION ${SWRESAMPLE_LIB_DIR})
    set_property(TARGET ${SWSCALE_LIB}  PROPERTY IMPORTED_LOCATION ${SWSCALE_LIB_DIR})
    set_property(TARGET ${FREETYPE_LIB} PROPERTY IMPORTED_LOCATION ${FREETYPE_LIB_DIR})
    set_property(TARGET ${OPENAL_LIB}   PROPERTY IMPORTED_LOCATION ${OPENAL_LIB_DIR})
else(ANDROID)
    # Linux/Unix and Win32 system
    set(CMAKE_C_FLAGS "${C_FLAGS} -lm")
    set(OpenGL_GL_PREFERENCE LEGACY)

    find_package(${ASSIMP_LIB} REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(OpenAL REQUIRED)
    find_package(ALUT REQUIRED)
    find_package(FFmpeg REQUIRED)
    find_package(Freetype REQUIRED)

    if (FFmpeg_FOUND)
        include_directories(
            ${FFmpeg_INCLUDE_DIRS}
        )
    endif(FFmpeg_FOUND)

    if (assimp_FOUND)
        add_library(
            ${ASSIMP_LIB}
            SHARED
            IMPORTED
        )
        set_target_properties(
            ${ASSIMP_LIB}
            PROPERTIES
            IMPORTED_LOCATION
            "${assimp_LIBRARIES}"
        )
        if(WIN32)
            set_target_properties(
                ${ASSIMP_LIB}
                PROPERTIES
                IMPORTED_IMPLIB
                "${assimp_LIBRARIES}"
            )
        endif(WIN32)
    endif(assimp_FOUND)

    if (FREETYPE_FOUND)
        include_directories(
            ${FREETYPE_INCLUDE_DIRS}
        )
    endif(FREETYPE_FOUND)
endif(ANDROID)

if(ANDROID)
    add_library(
        "aperture"
        SHARED
        ${APERTURE_SRC_DIR}
        "demo/demo_utils.c"
        "demo/demo_light.c"
        "demo/native-lib.c"
    )
else(ANDROID)
    add_library(
        "aperture"
        SHARED
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

    set(
        AP_LIB_DIRS
        ${ASSIMP_LIB_DIR}
        ${AVCODEC_LIB_DIR}
        ${AVDEVICE_LIB_DIR}
        ${AVFILTER_LIB_DIR}
        ${AVFORMAT_LIB_DIR}
        ${AVUTIL_LIB_DIR}
        ${SWRESAMPLE_LIB_DIR}
        ${SWSCALE_LIB_DIR}
        ${FREETYPE_LIB_DIR}
        ${OPENAL_LIB_DIR}
    )
    set(
        AP_LIBS
        ${ASSIMP_LIB}
        ${AVCODEC_LIB}
        ${AVDEVICE_LIB}
        ${AVFILTER_LIB}
        ${AVFORMAT_LIB}
        ${AVUTIL_LIB}
        ${SWRESAMPLE_LIB}
        ${SWSCALE_LIB}
        ${FREETYPE_LIB}
        ${OPENAL_LIB}
    )

    add_custom_command(
        TARGET "aperture"
        POST_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy
        ${AP_LIB_DIRS}
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )

    target_link_libraries(
        "aperture"
        ${log_lib}
        android
        log
        EGL
        GLESv3
        ${AP_LIBS}
    )
    set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/../src/main/assets/)
    file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${ASSETS_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/mc DESTINATION ${ASSETS_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/sound DESTINATION ${ASSETS_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/fonts DESTINATION ${ASSETS_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/glsl DESTINATION ${ASSETS_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/src/ap_glsl DESTINATION ${ASSETS_DIR})
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
        ${ASSIMP_LIB}
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
        ${ASSIMP_LIB}
        ${ALUT_LIBRARIES}
    )
    set_target_properties("test" PROPERTIES LINKER_LANGUAGE C)

    # link librarys to libaperture
    target_link_libraries(
        "aperture"
        ${OPENGL_gl_LIBRARY}
        glfw
        ${ASSIMP_LIB}
        ${OPENAL_LIBRARY}
        ${ALUT_LIBRARIES}
        ${FFmpeg_LIBRARIES}
        ${FREETYPE_LIBRARIES}
    )
    set_target_properties("aperture" PROPERTIES LINKER_LANGUAGE C)

    file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/mc DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/sound DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/fonts DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/demo/glsl DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/src/ap_glsl DESTINATION ${CMAKE_BINARY_DIR})
endif(ANDROID)
