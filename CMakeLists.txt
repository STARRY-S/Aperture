# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("game_engine"
        LANGUAGES C
        VERSION 0.1.0
)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
set(C_FLAGS "-Wall -std=c11 -g")
set(CMAKE_C_FLAGS ${C_FLAGS})
set(ASSIMP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/assimp/${ANDROID_ABI}/libassimp.so)

# add cglm include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(
        "assimp"
        SHARED
        IMPORTED
)

set_property(TARGET assimp PROPERTY IMPORTED_LOCATION ${ASSIMP_LIB_DIR})

add_library(
        # Sets the name of the library.
        game_engine
        # Sets the library as a shared library.
        SHARED
        # source files
        native-lib.c
        main.c
        shader.c
        texture.c
        camera.c
        renderer.c
        mesh.c
        model.c
        vertex.c
        cvector.c
        custom_io.c
)
set_target_properties(game_engine PROPERTIES LINKER_LANGUAGE C)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(
        log-lib
        log
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        game_engine

        ${log-lib}
        android
        log
        EGL
        GLESv3
        assimp
)

add_custom_command(TARGET "game_engine" POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${ASSIMP_LIB_DIR} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
