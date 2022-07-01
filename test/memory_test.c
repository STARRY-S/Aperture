#include "memory_test.h"
#include "func_test.h"
#include "aperture.h"

#include <assert.h>

#define TEST_MODEL "mc/mc-game.obj"
#define TEST_AUDIO_MP3 "sound/c418-haggstorm.mp3"

int model_memory_test()
{
        unsigned int model_id = 0;
        ap_model_generate(TEST_MODEL, &model_id);
        LOGD("generated model: %u", model_id);
        ap_model_free(model_id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_model_generate(TEST_MODEL, &model_id);
                LOGD("generated model: %u", model_id);
                ap_model_free(model_id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_model_free_all();
        ap_texture_free();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}

int audio_memory_test()
{
        ap_audio_init();
        unsigned int id = 0;
        ap_audio_load_mp3(TEST_AUDIO_MP3, &id);
        LOGD("generated audio: %u", id);
        ap_audio_free(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_audio_load_mp3(TEST_AUDIO_MP3, &id);
                LOGD("generated audio: %u", id);
                ap_audio_free(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_audio_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}
