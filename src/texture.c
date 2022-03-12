#include "texture.h"
#include "ge_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint load_texture(const char *const path, int format)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = NULL;
#ifdef __ANDROID__
    AAssetManager *pManager = ap_get_local_asset_manager();
    AAsset *pathAsset = AAssetManager_open(pManager, path, AASSET_MODE_UNKNOWN);
    off_t assetLength = AAsset_getLength(pathAsset);
    unsigned char *fileData = (unsigned char *) AAsset_getBuffer(pathAsset);
    data = stbi_load_from_memory(fileData, assetLength, &width, &height, &nrChannels, 0);
    AAsset_close(pathAsset);
    LOGD("path %s width: %d, height: %d, channel %d\n", path, width, height, nrChannels);
#else
    data = stbi_load(path, &width, &height, &nrChannels, 0);
#endif
    if (data) {
        // target, level, format,
        // w, h, 0(always), origin_format,
        // data_format, data
        glTexImage2D(GL_TEXTURE_2D, 0, format,
                     width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        LOGI("Loaded texture: %s\n", path);
    } else {
        LOGE("Failed to load texture: %s\n", path);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return texture;
}

unsigned int texture_from_file(const char *path, const char *directory, bool gamma)
{
    stbi_set_flip_vertically_on_load(true);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned int iBufferLength = strlen(path) + strlen(directory) + 1;
    char *pPathBuff = malloc(sizeof(char) * iBufferLength);
    sprintf(pPathBuff, "%s%s", directory, path);

    unsigned char *data = NULL;
    #ifdef __ANDROID__
    int fileLength = 0;
    AAssetManager *pManager = ap_get_local_asset_manager();
    AAsset *pathAsset = AAssetManager_open(pManager, pPathBuff, AASSET_MODE_UNKNOWN);
    if (pathAsset == NULL) {
        // AP_ERROR_ASSET_OPEN_FAILED;
        LOGE("Failed to load texture from file: %s", pPathBuff);
        free(pPathBuff);
        pPathBuff = NULL;
        return 0;
    }
    free(pPathBuff);
    pPathBuff = NULL;
    fileLength = AAsset_getLength(pathAsset);

    unsigned char *fileData = (unsigned char *) AAsset_getBuffer(pathAsset);
    data = stbi_load_from_memory(
        fileData, fileLength, &width, &height, &nrComponents, 0
    );
    AAsset_close(pathAsset);

    #else

    data = stbi_load(pPathBuff, &width, &height, &nrComponents, 0);

    #endif

    LOGD("path %s width: %d, height: %d, channel %d\n", pPathBuff, width, height, nrComponents);

    if (data) {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (int) format,
                     width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        LOGE("Failed to load texture: %s\n", pPathBuff);
        stbi_image_free(data);
    }
    return textureID;
}

int init_texture(struct Texture *pTexture)
{
    if (pTexture == NULL) {
        return AP_ERROR_INVALID_PARAMETER;
    }

    memset(pTexture, 0, sizeof(struct Texture));
    return 0;
}

int texture_set_type(struct Texture *pTexture, const char *typeName)
{
    if (pTexture == NULL || typeName == NULL) {
        return AP_ERROR_INVALID_POINTER;
    }

    if (pTexture->type != NULL) {
        LOGD("Try to free old texture type pointer: 0X%p", pTexture);
        free(pTexture->type);
        pTexture->type = NULL;
    }
    pTexture->type = malloc(sizeof(char) * (strlen(typeName) + 1) );
    strcpy(pTexture->type, typeName);
    return 0;
}

int texture_set_path(struct Texture *pTexture, const char *pathName)
{
    if (pTexture == NULL || pathName == NULL) {
        return AP_ERROR_INVALID_POINTER;
    }

    if (pTexture->path != NULL) {
        LOGD("Try to free old texture path pointer: 0X%p", pTexture);
        free(pTexture->path);
        pTexture->path = NULL;
    }
    pTexture->path = malloc(sizeof(char) * (strlen(pathName) + 1) );
    strcpy(pTexture->path, pathName);
    return 0;
}