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
#include "ap_physic.h"

#include "demo_main.h"
#include "demo_utils.h"

#if AP_PLATFORM_ANDROID

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
        ap_render_resize_buffer(width, height);
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
        ap_camera_process_mouse_move(x * 5, y * 5, constrain_pitch);
        return 0;
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_creatureMove(JNIEnv *env, jclass clazz, jint direction, jfloat speed) {
        // TODO: implement creatureMove()
        switch (direction) {
        case 0: // forward
                ap_creature_process_move(AP_DIRECTION_FORWARD, speed);
                break;
        case 1: // backward
                ap_creature_process_move(AP_DIRECTION_BACKWORD, speed);
                break;
        case 2: // left
                ap_creature_process_move(AP_DIRECTION_LEFT, speed);
                break;
        case 3: // right
                ap_creature_process_move(AP_DIRECTION_RIGHT, speed);
                break;
        default:
                break;
        }
        return 0;
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_creatureJump(JNIEnv *env, jclass clazz) {
        ap_creature_jump();
        return 0;
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_spotLightStatus(JNIEnv *env, jclass clazz,
                                                          jboolean status) {
        ap_render_set_spot_light_enabled(status);
        return 0;
}

#endif
