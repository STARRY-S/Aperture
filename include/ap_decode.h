#ifndef AP_DECODE_H
#define AP_DECODE_H

#include "ap_cvector.h"

/**
 * @brief decode audio to PCM format into a file
 * only support MP3 format audio
 * @param filename name of the audio file to be decoded
 * @param out_name output file name
 * @return int AP_Error_Types
 */
int ap_decode_to_file(const char* filename, const char *out_name);

/**
 * @brief decode audio to struct AP_Vector
 * only support MP3 format audio
 * @param filename name of the audio file to be decoded
 * @param out_vec_p [out] pointer points to the pointer of vector
 * @param ap_format [out] AP_Audio_format
 * @param frequency [out] frequency
 * @param channels  [out] channels
 * @return int AP_Error_Types
 */
int ap_decode_to_memory(
        const char* filename,
        struct AP_Vector **out_vec_p,
        int *ap_format,
        float *frequency,
        int *channels
);

#endif