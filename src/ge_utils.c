#include <GLES3/gl3.h>

#include <assimp/cfileio.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ge_utils.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

static const char *AP_ERROR_NAME[AP_ERROR_LENGTH] = {
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
    "CAMERA_NOT_SET",
    "UNKNOWN"
};

#ifdef __ANDROID__

static AAssetManager *pLocalAAsetManager = NULL;
static char sMobileName[AP_DEFAULT_BUFFER_SIZE] = { 0 };

void *ap_get_local_asset_manager()
{
    return (void*) pLocalAAsetManager;
}

int ap_set_local_asset_manager(void *pVoid)
{
    if (!pVoid) {
        LOGE("ap_set_local_asset_manager ERROR: NULL");
        pLocalAAsetManager = NULL;
        return AP_ERROR_INVALID_POINTER;
    }

    struct AAssetManager *pManager = (struct AAssetManager*) pVoid;
    if (pLocalAAsetManager == NULL) {
        pLocalAAsetManager = pManager;
    } else {
        LOGI("Replace Local AAsetManager Pointer");
        pLocalAAsetManager = pManager;
    }
    return 0;
}

int ap_set_mobile_name(const char *pName)
{
    if (pName == NULL) {
        LOGE("ap_set_mobile_name ERROR: NULL");
        return AP_ERROR_INVALID_POINTER;
    }

    strncpy(sMobileName, pName, AP_DEFAULT_BUFFER_SIZE);
    sMobileName[AP_DEFAULT_BUFFER_SIZE - 1] = '0';

    return 0;
}

const char* ap_get_mobile_name()
{
    return sMobileName;
}

int ap_get_mobile_type(const char *pMobileName)
{
    if (pMobileName == NULL) {
        LOGE("ap_get_mobile_type Failed: NULL");
        return 0;
    }
    for (int i = 0; i < AP_MOBILE_LENGTH; ++i) {
        if (strstr(pMobileName, AP_MOBILE_NAME[i]) != NULL) {
            return i;
        }
    }
    return 0;
}
#else   // Not Android

// Nothing...

#endif  // Not Android

void AP_CHECK(int i)
{
    if (i > 0 && i < AP_ERROR_LENGTH) {
        LOGE("%s\n", AP_ERROR_NAME[i]);
    }
}

static const float cubeVertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

const float* ap_get_default_cube_vertices()
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    return cubeVertices;
}

int ap_get_default_cube_vertices_length()
{
    return sizeof(cubeVertices);
}