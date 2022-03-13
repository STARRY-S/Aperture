#ifndef AP_UTILS_H
#define AP_UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cglm/cglm.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#define  LOG_TAG    "AP_MAIN"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define  LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)

typedef enum {
        AP_MOBILE_DEFAULT = 0,
        AP_MOBILE_GOOGLE,       // Google
        AP_MOBILE_ZTE,          // ZTE
        AP_MOBILE_HUAWEI,       // HUAWEI
        AP_MOBILE_COOLPAD,      // CoolPad
        AP_MOBILE_LENOVO,       // Lenovo
        AP_MOBILE_REALME,       // RealMe
        AP_MOBILE_ONEPLUS,      // OnePlus
        AP_MOBILE_XIAOMI,       // Xiaomi
        AP_MOBILE_BLACKSHARK,   // BlackShark
        AP_MOBILE_OPPO,         // OPPO
        AP_MOBILE_HTC,          // HTC
        AP_MOBILE_SONY,         // Sony
        AP_MOBILE_SAMSUNG,      // Samsung
        AP_MOBILE_NOKIA,        // NOKIA
        AP_MOBILE_X86,          // X86
        AP_MOBILE_LENGTH,
} AP_MOBILE_Types;

static const char *AP_MOBILE_NAME[AP_MOBILE_LENGTH] = {
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
void *ap_get_aaset_manager();

/**
 * @brief Set Asset Manager
 * @param pManager pointer points to asset manager
 * @return int AP_Types
 */
int ap_set_aaset_manager(void *pManager);

/**
 * @brief Set the Mobile Name
 * @param pName const char *
 * @return int AP_Types
 */
int ap_set_mobile_name(const char *pName);

/**
 * @brief Get the Mobile Name
 * @return const char*
 */
const char* ap_get_mobile_name();

/**
 * @brief Get Mobile Type
 * @param pMobileName
 * @return int AP_MOBILE_Types
 */
int ap_get_mobile_type(const char *pMobileName);

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
        AP_ERROR_SUCCESS = 0,
        AP_ERROR_INVALID_POINTER,       // pointer is NULL
        AP_ERROR_INVALID_PARAMETER,     // invalid param
        AP_ERROR_MALLOC_FAILED,         // malloc failed
        AP_ERROR_MESH_UNINITIALIZED,    // mesh uninitialized
        AP_ERROR_ASSIMP_IMPORT_FAILED,  // assimp import failed
        AP_ERROR_ASSET_OPEN_FAILED,     // asset manager open file failed
        AP_ERROR_INIT_FAILED,           // initialize failed
        AP_ERROR_RENDER_FAILED,         // render failed with unknown error
        AP_ERROR_TEXTURE_FAILED,        // texture load failed unknown error
        AP_ERROR_CAMERA_NOT_SET,        // camera not set
        AP_ERROR_UNKNOWN,               // unknown error
        AP_ERROR_LENGTH
} AP_Types;

/**
 * @brief Check return value of ap_* functions
 */
void AP_CHECK(int i);

/**
 * @brief Get the vertices of a cude
 * @return const float*
 */
const float* ap_get_default_cube_vertices();

/**
 * @brief Get the length of cube vertices
 */
int ap_get_default_cube_vertices_length();

#define AP_DEFAULT_BUFFER_SIZE 128

#endif // AP_UTILS_H
