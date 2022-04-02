/**
 * @file native-lib.c
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief interface for Android JNI
 * @version 1.0
 * @date 2022-03-06
 *
 * @copyright Copyright (c) 2022 - Apache 2.0
 */

#include <string.h>

#include "ap_utils.h"
#include "ap_render.h"
#include "ap_camera.h"

#include "demo_main.h"
#include "demo_utils.h"

#ifdef __ANDROID__

#include <jni.h>

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_init(JNIEnv *env, jclass clazz) {
        demo_init();
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_step(JNIEnv *env, jclass clazz) {
        demo_render();
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_setNativeAssetManager(JNIEnv *env, jclass clazz,
                                                                jobject asset_manager) {
        AAssetManager *manager = AAssetManager_fromJava(env, asset_manager);
        ap_set_asset_manager(manager);
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_updateBufferSize(JNIEnv *env, jclass clazz, jint width,
                                                           jint height) {
        ap_resize_screen_buffer(width, height);
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_setMobileName(
        JNIEnv *env, jclass clazz, jstring s_name) {
        const char *cparam = (*env)->GetStringUTFChars(env, s_name, 0);
        ap_set_mobile_name((const char*) cparam);
        (*env)->ReleaseStringUTFChars(env, s_name, cparam);
        return 0;
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_cameraViewEvent(JNIEnv *env, jclass clazz,
                                                          jfloat x, jfloat y,
                                                          jboolean constrain_pitch) {
        ap_camera_process_mouse_move(x, y, constrain_pitch);
        return 0;
}

#else // __ANDROID__

#endif