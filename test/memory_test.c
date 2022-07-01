#include "memory_test.h"
#include "func_test.h"
#include "aperture.h"

#include <assert.h>

#define TEST_MODEL "mc/mc-game.obj"
#define TEST_AUDIO_MP3 "sound/c418-haggstorm.mp3"
#define TEST_VSHADER "ap_glsl/ap_orthographic.vs.glsl"
#define TEST_FSHADER "ap_glsl/ap_orthographic.fs.glsl"

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
        ap_texture_free_all();
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

int camera_memory_test()
{
        unsigned int id = 0;
        ap_camera_generate(&id);
        assert(id != 0);
        LOGD("generated camera: %u", id);
        ap_camera_free(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_camera_generate(&id);
                assert(id != 0);
                LOGD("generated camera: %u", id);
                ap_camera_free(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_camera_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}

int light_memory_test()
{
        unsigned int id = 0;
        float f3[3] = { 0.0f };
        float f8[8] = { 0.0f };
        ap_light_generate_point(&id, f3, f3, f3, f3, f8);
        assert(id != 0);
        LOGD("generated point light: %u", id);
        ap_light_free_point_light(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_light_generate_point(&id, f3, f3, f3, f3, f8);
                assert(id != 0);
                LOGD("generated point light: %u", id);
                ap_light_free_point_light(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_light_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}

int physic_memory_test()
{
        unsigned int id = 0;
        AP_CHECK( ap_physic_generate_barrier(&id, AP_BARRIER_TYPE_BOX) );
        LOGD("generated box barrier: %u", id);
        assert(id != 0);
        ap_physic_free_barrier(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_physic_generate_barrier(&id, AP_BARRIER_TYPE_BOX);
                assert(id != 0);
                LOGD("generated box barrier: %u", id);
                ap_physic_free_barrier(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_physic_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        float f3[3] = { 0.f };
        ap_physic_generate_creature(&id, f3);
        LOGD("generated greature: %u", id);
        ap_physic_free_creature(id);
        unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_physic_generate_creature(&id, f3);
                LOGD("generated greature: %u", id);
                ap_physic_free_creature(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_physic_free_all();
        ap_camera_free_all();
        ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}

int shader_memory_test()
{
        unsigned int id = 0;
        ap_shader_generate(TEST_VSHADER, TEST_FSHADER, &id);
        assert(id != 0);
        LOGD("generated shader: %u", id);
        ap_shader_free(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_shader_generate(TEST_VSHADER, TEST_FSHADER, &id);
                assert(id != 0);
                LOGD("generated shader: %u", id);
                ap_shader_free(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_shader_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}

int texture_memory_test()
{
        unsigned int id = 0;
        float rgba[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
        ap_texture_generate_rgba(&id, rgba, 128, AP_TEXTURE_TYPE_DIFFUSE);
        assert(id != 0);
        LOGD("generated rgba texture: %u", id);
        ap_texture_free(id);
        int unreleased_ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", unreleased_ptrs);
        for (int i = 0; i < 3; ++i) {
                ap_texture_generate_rgba(
                        &id, rgba, 128, AP_TEXTURE_TYPE_DIFFUSE
                );
                assert(id != 0);
                LOGD("generated rgba texture: %u", id);
                ap_texture_free(id);
                int ptrs = ap_memory_unreleased_num();
                LOGD("unreleased ptrs: %d", ptrs);
                assert(ptrs == unreleased_ptrs);
        }

        ap_texture_free_all();
        int ptrs = ap_memory_unreleased_num();
        LOGD("unreleased ptrs: %d", ptrs);
        assert(ptrs == 0);

        return 0;
}
