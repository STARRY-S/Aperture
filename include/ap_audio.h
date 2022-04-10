/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Audio implemention for Aperture based on OpenAL, ALUT
 *
 * @copyright Copyright (c) 2022 Apache - 2.0
 */
#ifndef AP_AUDIO_H
#define AP_AUDIO_H

int ap_audio_init();

unsigned int ap_audio_load_buffer(const char* name);

int ap_audio_play_buffer_sync(unsigned int buffer);

int ap_audio_play_buffer(unsigned int buffer);

int ap_audio_finish();

#endif