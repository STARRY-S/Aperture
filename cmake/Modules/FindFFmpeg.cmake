# Find FFMpeg libraries
find_library(AVCODEC_LIBRARY  avcodec)
find_library(AVFORMAT_LIBRARY avformat)
find_library(AVUTIL_LIBRARY   avutil)
find_library(AVDEVICE_LIBRARY avdevice)

# FFMpeg include directories
find_path(
    FFmpeg_INCLUDE_DIRS

    NAMES "libavcodec/avcodec.h"
    NAMES "libavformat/avformat.h"
    NAMES "libavutil/avutil.h"
    NAMES "libavdevice/avdevice.h"

    PATHS "/usr/local/include"
    PATHS "/usr/include/"
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../include/"

    PATH_SUFFIXES "ffmpeg"
)

if (FFmpeg_INCLUDE_DIRS AND AVCODEC_LIBRARY AND AVFORMAT_LIBRARY
                AND AVUTIL_LIBRARY AND AVDEVICE_LIBRARY)
    SET(FFmpeg_FOUND TRUE)
    SET(FFmpeg_LIBRARIES
        ${AVCODEC_LIBRARY}
        ${AVFORMAT_LIBRARY}
        ${AVUTIL_LIBRARY}
        ${AVDEVICE_LIBRARY}
    )
    message(STATUS "Found FFmpeg: ${FFmpeg_LIBRARIES}")
else ()
    if (FFmpeg_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find FFmpeg library")
    endif (FFmpeg_FIND_REQUIRED)
endif ()
