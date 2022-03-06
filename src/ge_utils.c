#include <GLES3/gl3.h>

#include <assimp/cfileio.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ge_utils.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

static const char *GE_ERROR_NAME[GE_ERROR_LENGTH] = {
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
    "UNKNOWN"
};

#ifdef __ANDROID__

static AAssetManager *pLocalAAsetManager = NULL;
static char sMobileName[GE_DEFAULT_BUFFER_SIZE] = { 0 };

void *getLocalAAssetManager()
{
    return (void*) pLocalAAsetManager;
}

int setAAssetManager(void *pVoid)
{
    if (!pVoid) {
        LOGE("setAAssetManager ERROR: NULL");
        pLocalAAsetManager = NULL;
        return GE_ERROR_INVALID_POINTER;
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

int setMobileName(const char *pName)
{
    if (pName == NULL) {
        LOGE("setMobileName ERROR: NULL");
        return GE_ERROR_INVALID_POINTER;
    }

    strncpy(sMobileName, pName, GE_DEFAULT_BUFFER_SIZE);
    sMobileName[GE_DEFAULT_BUFFER_SIZE - 1] = '0';

    return 0;
}

const char* getMobileName()
{
    return sMobileName;
}

int getMobileType(const char *pMobileName)
{
    if (pMobileName == NULL) {
        LOGE("getMobileType Failed: NULL");
        return 0;
    }
    for (int i = 0; i < GE_MOBILE_LENGTH; ++i) {
        if (strstr(pMobileName, GE_MOBILE_NAME[i]) != NULL) {
            return i;
        }
    }
    return 0;
}
#else   // Not Android

// Nothing...

#endif  // Not Android

void GE_CHECK(int i)
{
    if (i > 0 && i < GE_ERROR_LENGTH) {
        LOGE("%s\n", GE_ERROR_NAME[i]);
    }
}