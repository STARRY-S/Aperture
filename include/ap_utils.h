#ifndef AP_UTILS_H
#define AP_UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// show debug message
// #define AP_DEBUG
#define AP_DEFAULT_BUFFER_SIZE 128

#include "cglm/cglm.h"
#include "ap_memory.h"

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

void *ap_get_asset_manager();

int ap_set_asset_manager(void *pManager);

int ap_set_mobile_name(const char *pName);

const char* ap_get_mobile_name();

int ap_get_mobile_type(const char *pMobileName);

#else   // NOT ANDROID

#include <GLFW/glfw3.h>

#define AP_COLOR_RED     "\x1b[31m"
#define AP_COLOR_GREEN   "\x1b[32m"
#define AP_COLOR_YELLOW  "\x1b[33m"
#define AP_COLOR_BLUE    "\x1b[34m"
#define AP_COLOR_MAGENTA "\x1b[35m"
#define AP_COLOR_CYAN    "\x1b[36m"
#define AP_COLOR_RESET   "\x1b[0m"

// Output log messages to stdout/stderr
#define LOGI(...) \
        fprintf(stdout, "%s[AP_MESSG] %s", AP_COLOR_GREEN, AP_COLOR_RESET); \
        fprintf(stdout, __VA_ARGS__); \
        fprintf(stdout, "\n");
#define LOGE(...) \
        fprintf(stderr, "%s[AP_ERROR] %s", AP_COLOR_RED, AP_COLOR_RESET);   \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");
#define LOGW(...) \
        fprintf(stdout, "%s[AP_WARNG] %s", AP_COLOR_MAGENTA, AP_COLOR_RESET); \
        fprintf(stdout, __VA_ARGS__); \
        fprintf(stdout, "\n");

#ifdef AP_DEBUG
#define LOGD(...) \
        fprintf(stdout, "%s[AP_DEBUG] %s", AP_COLOR_YELLOW, AP_COLOR_RESET); \
        fprintf(stdout, __VA_ARGS__); \
        fprintf(stdout, "\n");
#else
#define LOGD(...) ;
#endif  // AP_DEBUG

#endif  // NOT ANDROID

extern const char *AP_ERROR_NAME[];

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
        AP_ERROR_SHADER_LOAD_FAILED,    // shader load failed
        AP_ERROR_CAMERA_NOT_SET,        // camera not set
        AP_ERROR_SHADER_NOT_SET,        // shader not set
        AP_ERROR_MODEL_NOT_SET,         // model not set
        AP_ERROR_UNKNOWN,               // unknown error
        AP_ERROR_LENGTH
} AP_Types;

typedef int (*ap_callback_func_t)(void* param, int reserve);

inline static void AP_CHECK(int check_i)
{
        if (check_i > 0 && check_i < AP_ERROR_LENGTH) {
                LOGE("%s", AP_ERROR_NAME[check_i]);
        }
}

const float* ap_get_default_cube_vertices();

int ap_get_default_cube_vertices_length();

int ap_set_context_ptr(void* ptr);

void* ap_get_context_ptr();

int ap_get_buffer_width();

int ap_get_buffer_height();

int ap_set_buffer(int w, int h);

#endif // AP_UTILS_H
