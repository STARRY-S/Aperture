#ifndef GAME_ENGINE_UTILS_H
#define GAME_ENGINE_UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cglm/cglm.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

// Output log messages by using __android_log_print
#define  LOG_TAG    "GE_MAIN"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)

typedef enum {
    GE_MOBILE_DEFAULT = 0,  // default mobile type
    GE_MOBILE_GOOGLE,       // Google
    GE_MOBILE_ZTE,          // ZTE
    GE_MOBILE_HUAWEI,       // HUAWEI
    GE_MOBILE_COOLPAD,      // CoolPad
    GE_MOBILE_LENOVO,       // Lenovo
    GE_MOBILE_REALME,       // RealMe
    GE_MOBILE_ONEPLUS,      // OnePlus
    GE_MOBILE_XIAOMI,       // Xiaomi
    GE_MOBILE_BLACKSHARK,   // BlackShark
    GE_MOBILE_OPPO,         // OPPO
    GE_MOBILE_HTC,          // HTC
    GE_MOBILE_SONY,         // Sony
    GE_MOBILE_SAMSUNG,      // Samsung
    GE_MOBILE_NOKIA,        // NOKIA
    GE_MOBILE_X86,          // X86
    GE_MOBILE_LENGTH,       // never use this.
} GE_MOBILE_Types;

static const char *GE_MOBILE_NAME[GE_MOBILE_LENGTH] = {
    "default",
    "google",
    "zte",
    "huawei",
    "coolpad",
    "lenovo",
    "realme",
    "oneplus",
    "xiaomi",
    "blackshark",
    "oppo",
    "htc",
    "sony",
    "samsung",
    "nokia",
    "x86"
};

/**
 * @brief Get the Local A Asset Manager object
 * @return void* pointer points to asset manager
 */
void *getLocalAAssetManager();

/**
 * @brief Set Asset Manager
 * @param pManager pointer points to asset manager
 * @return int GE_Types
 */
int setAAssetManager(void *pManager);

/**
 * @brief Set the Mobile Name
 * @param pName const char *
 * @return int GE_Types
 */
int setMobileName(const char *pName);

/**
 * @brief Get the Mobile Name
 * @return const char*
 */
const char* getMobileName();

/**
 * @brief Get Mobile Type
 * @param pMobileName
 * @return int GE_MOBILE_Types
 */
int getMobileType(const char *pMobileName);

#else   // NOT ANDROID

#include <GLFW/glfw3.h>

// Output log messages to stdout/stderr
#define LOGI(...) fprintf(stdout, __VA_ARGS__);
#define LOGE(...) fprintf(stderr, __VA_ARGS__);
#define LOGW(...) fprintf(stdout, __VA_ARGS__);

#ifdef DEBUG
#define LOGD(...) fprintf(stdout, __VA_ARGS__);
#else
#define LOGD(...) ;
#endif  // DEBUG

#endif  // NOT ANDROID

// common used error types
typedef enum {
    GE_ERROR_SUCCESS = 0,
    GE_ERROR_INVALID_POINTER,       // pointer is NULL
    GE_ERROR_INVALID_PARAMETER,     // invalid param
    GE_ERROR_MALLOC_FAILED,         // malloc failed
    GE_ERROR_MESH_UNINITIALIZED,    // mesh uninitialized
    GE_ERROR_ASSIMP_IMPORT_FAILED,  // assimp import failed
    GE_ERROR_ASSET_OPEN_FAILED,     // asset manager open file failed
    GE_ERROR_INIT_FAILED,           // initialize failed
    GE_ERROR_RENDER_FAILED,         // render failed with unknown error
    GE_ERROR_TEXTURE_FAILED,        // texture load failed with unknown error
    GE_ERROR_UNKNOWN,               // unknown error
    GE_ERROR_LENGTH                 // never use this
} GE_Types;

/**
 * GE_CHECK
 */
void GE_CHECK(int i);

/**
 * @brief Get the vertices of a cude
 * @return const float*
 */
const float* getCubeVertices();

/**
 * @brief Get the length of cube vertices
 */
int getCubeVertivesLength();

#define GE_DEFAULT_BUFFER_SIZE 128

#endif //GAME_ENGINE_UTILS_H
