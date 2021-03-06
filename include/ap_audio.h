/**
 * @file ap_audio.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Audio function definitions
 *
 */

#ifndef AP_AUDIO_H
#define AP_AUDIO_H

#include "ap_utils.h"

/**
 * Audio types supported by ap_audio
 */
typedef enum {
        AP_AUDIO_FMT_UNKNOWN = 0,
        /** unsigned 8 bits */
        AP_AUDIO_FMT_U8,
        /** signed 16 bits */
        AP_AUDIO_FMT_S16,
        /** signed 32 bits */
        AP_AUDIO_FMT_S32,
        /** signed 64bit */
        AP_AUDIO_FMT_S64,
        /** float */
        AP_AUDIO_FMT_FLT,
        /** double */
        AP_AUDIO_FMT_DBL,
        AP_AUDIO_FMT_LENGTH
} AP_Audio_FMT;

/**
 * Audio struct definition
 */
struct AP_Audio {
        /** audio id */
        unsigned int id;
        /** name of the audio file, for avoid reopen/decode audios */
        char *name;
        /** the PCM audio data decoded into the memory */
        char *data;
        /** data size */
        int data_size;

        /** channels: 1 (mono) or 2 (setro) */
        int channels;
        /** format: format of the PCM audio, should be AP_Audio_FMT */
        int format;
        /** frequency: e.g. 44100.0Hz */
        float frequency;
        /** the source id generated by OpenAL */
        unsigned int source_id;

        /** callback function if play audio in asynchronized mode */
        ap_callback_func_t cb;
};

/**
 * @brief Convert aperture audio type to OpenAL audio format
 *
 * @param ap_audio_fmt
 * @param channel
 * @return int OpenAL audio format, 0 on error
 */
int ap_audio_fmt_ap_2_al(int ap_audio_fmt, int channel);
/**
 * @brief Convert libavformat audio format to aperture audio format
 *
 * @param av_fmt
 * @param s libavformat format name, can be null, only used for show warning
 * message if format not found
 * @return int aperture audio format, 0 on error
 */
int ap_audio_fmt_av_2_ap(int av_fmt, const char *s);
/**
 * @brief Convert OpenAL audio format to aperture audio format
 *
 * @param al_fmt
 * @return int aperture audio format, 0 on error
 */
int ap_audio_fmt_al_2_ap(int al_fmt);

/**
 * @brief Initialize OpenAL, create OpenAL Context
 * @return AP_Error_Types
 */
int ap_audio_init();

/**
 * @brief release audio by its audio ID
 *
 * @param id
 * @return int AP_Error_Types
 */
int ap_audio_free(int id);

/**
 * @brief Close OpenAL device, destroy context.
 * @return AP_Error_Types
 */
int ap_audio_free_all();

/**
 * @brief Load MPEG-3 format audio into memory and get its audio ID
 *
 * @param name [in] file name
 * @param id   [out] audio ID
 * @see ap_audio_load_wav
 * @return int AP_Error_Types
 */
int ap_audio_load_mp3(const char *name, unsigned int *id);

/**
 * @brief Load WAV format audio into memory and get its audio ID
 *
 * @param name [in] file name
 * @param id   [out] audio ID
 * @see ap_audio_load_mp3
 * @return int AP_Error_Types
 */
int ap_audio_load_wav(const char *name, unsigned int *id);

/**
 * @brief Play audio in asynchronized mode
 *
 * @param id audio ID
 * @param cb callback function, can be NULL
 * @return int
 */
int ap_audio_play(unsigned int id, ap_callback_func_t cb);

/**
 * @brief Pause the audio
 *
 * @param id
 * @return int
 */
int ap_audio_pause(unsigned int id);

/**
 * @brief Stop the audio
 *
 * @param id
 * @return int
 */
int ap_audio_stop(unsigned int id);

/**
 * @brief Get audio struct object pointer
 *
 * @param id
 * @return struct AP_Audio*
 */
struct AP_Audio* ap_audio_get_ptr(unsigned int id);

#endif