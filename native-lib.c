#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "main.h"
#include "renderer.h"
#include "camera.h"

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_init(JNIEnv *env, jclass clazz) {
    setup();
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_step(JNIEnv *env, jclass clazz) {
    render();
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_setNativeAssetManager(JNIEnv *env, jclass clazz,
                                                                jobject asset_manager) {
    AAssetManager *manager = AAssetManager_fromJava(env, asset_manager);
    setAAssetManager(manager);
}

JNIEXPORT void JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_updateBufferSize(JNIEnv *env, jclass clazz, jint width,
                                                           jint height) {
    resizeBuffer(width, height);
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_setMobileName(
        JNIEnv *env, jclass clazz, jstring s_name) {
    const char *cparam = (*env)->GetStringUTFChars(env, s_name, 0);
    setMobileName((const char*) cparam);
    (*env)->ReleaseStringUTFChars(env, s_name, cparam);
    return 0;
}

JNIEXPORT jint JNICALL
Java_moe_starrys_game_1engine_GLES3JNILib_cameraViewEvent(JNIEnv *env, jclass clazz,
                                                          jfloat x, jfloat y,
                                                          jboolean constrain_pitch) {
    ProcessMouseMovement(x, y, constrain_pitch);
    return 0;
}
