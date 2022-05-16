#include <assimp/cfileio.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

#include "ap_utils.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_camera.h"
#include "ap_cvector.h"

const char *AP_ERROR_NAME[AP_ERROR_LENGTH] = {
        "SUCCESS",
        "INVALID_POINTER",
        "INVALID_PARAMETER",
        "MALLOC_FAILED",
        "MESH_UNINITIALIZED",
        "ASSIMP_IMPORT_FAILED",
        "ASSET_OPEN_FAILED",
        "INIT_FAILED",
        "RENDER_FAILED",
        "TEXTURE_FAILED",
        "SHADER_LOAD_FAILED",
        "CAMERA_NOT_SET",
        "SHADER_NOT_SET",
        "MODEL_NOT_SET",
        "BUFFER_GEN_FAILED",
        "DECODE_FAILED",
        "DECODE_NOT_INIT",
        "DECODE_FMT_NSUPPORT",
        "UNKNOWN"
};

#ifdef __ANDROID__

static AAssetManager *local_asset_manager = NULL;
static char mobile_name[AP_DEFAULT_BUFFER_SIZE] = { 0 };

void *ap_get_asset_manager()
{
        return (void*) local_asset_manager;
}

int ap_set_asset_manager(void *p_void)
{
        if (!p_void) {
                LOGE("ap_set_asset_manager ERROR: NULL");
                local_asset_manager = NULL;
                return AP_ERROR_INVALID_POINTER;
        }

        struct AAssetManager *manager = (struct AAssetManager*) p_void;
        if (local_asset_manager == NULL) {
                local_asset_manager = manager;
        } else {
                LOGI("Replace Local AssetManager Pointer");
                local_asset_manager = manager;
        }
        return 0;
}

int ap_set_mobile_name(const char *name)
{
        if (name == NULL) {
                LOGE("ap_set_mobile_name ERROR: NULL");
                return AP_ERROR_INVALID_POINTER;
        }

        strncpy(mobile_name, name, AP_DEFAULT_BUFFER_SIZE);
        mobile_name[AP_DEFAULT_BUFFER_SIZE - 1] = '0';

        return 0;
}

const char* ap_get_mobile_name()
{
        return mobile_name;
}

int ap_get_mobile_type(const char *name)
{
        if (name == NULL) {
                LOGE("ap_get_mobile_type Failed: NULL");
                return 0;
        }
        for (int i = 0; i < AP_MOBILE_LENGTH; ++i) {
                if (strstr(name, AP_MOBILE_NAME[i]) != NULL) {
                        return i;
                }
        }
        return 0;
}
#else   // Not Android

// Nothing...

#endif  // Not Android

static const float ap_utils_cube_vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f
};

const float* ap_get_default_cube_vertices()
{
    return ap_utils_cube_vertices;
}

int ap_get_default_cube_vertices_length()
{
    return sizeof(ap_utils_cube_vertices);
}

double ap_get_time()
{
        static struct timeval now, start;
        static bool started = false;
        if (!started) {
                gettimeofday(&start, NULL);
                started = true;
        }
        gettimeofday(&now, NULL);
        double time = (now.tv_sec - start.tv_sec) + (now.tv_usec) / 1e6;
        return time;
}
