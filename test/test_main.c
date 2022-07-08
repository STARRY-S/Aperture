/**
 * @file test_main.c
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief
 */
#include <stdio.h>

#include "test_main.h"
#include "func_test.h"
#include "memory_test.h"

int main(int argc, char **argv)
{
    init_opengl_context();

    // test_vector_char();

    // test_vector_float();

    // test_vector_uint();

    // test_vector_int();

    // LOGD("Debug msg");
    // LOGI("Info msg");
    // LOGW("Warn msg");
    // LOGE("Error msg");

    // test_ap_memory();

    // // test_model_async();

    // test_audio();

    // test_decode();

    // test_audio();

    // test_ap_decode();

    // test_sqlite();

    // test_model_load();

    model_memory_test();
    audio_memory_test();
    camera_memory_test();
    light_memory_test();
    physic_memory_test();
    shader_memory_test();
    texture_memory_test();

    return 0;
}