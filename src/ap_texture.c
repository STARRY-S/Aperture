#include "ap_texture.h"
#include "ap_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint ap_texture_load(const char *const path, int format)
{
        int width, height, nr_channels;
        unsigned int texture;
        unsigned char *data = NULL;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_set_flip_vertically_on_load(true);

        #ifdef __ANDROID__
        AAssetManager *manager = ap_get_asset_manager();
        AAsset *path_asset = AAssetManager_open(
                manager, path, AASSET_MODE_UNKNOWN);
        off_t assetLength = AAsset_getLength(path_asset);
        unsigned char *file_data =
                (unsigned char *) AAsset_getBuffer(path_asset);
        data = stbi_load_from_memory(
                file_data, assetLength, &width, &height, &nr_channels, 0);
        AAsset_close(path_asset);
        LOGD("path %s width: %d, height: %d, channel %d\n",
                path, width, height, nr_channels);
        #else

        data = stbi_load(path, &width, &height, &nr_channels, 0);

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

unsigned int ap_texture_from_file(
        const char *path,
        const char *directory,
        bool gamma)
{
        stbi_set_flip_vertically_on_load(true);

        unsigned int texture_id;
        glGenTextures(1, &texture_id);

        int width, height, nr_components;
        int buffer_length = strlen(path) + strlen(directory) + 1;
        char *path_buffer = malloc(sizeof(char) * buffer_length);
        if (path_buffer == NULL) {
                LOGE("Malloc failed.\n");
                return 0;
        }
        sprintf(path_buffer, "%s%s", directory, path);

        unsigned char *data = NULL;
        #ifdef __ANDROID__
        int file_length = 0;
        AAssetManager *manager = ap_get_asset_manager();
        AAsset *path_asset = AAssetManager_open(
                manager, path_buffer, AASSET_MODE_UNKNOWN);
        if (path_asset == NULL) {
                // AP_ERROR_ASSET_OPEN_FAILED;
                LOGE("Failed to load texture from file: %s", path_buffer);
                free(path_buffer);
                path_buffer = NULL;
                return 0;
        }
        free(path_buffer);
        path_buffer = NULL;
        file_length = AAsset_getLength(path_asset);

        unsigned char *file_data =
                (unsigned char *) AAsset_getBuffer(path_asset);
        data = stbi_load_from_memory(
                file_data, file_length, &width, &height, &nr_components, 0
        );
        AAsset_close(path_asset);

        #else

        data = stbi_load(path_buffer, &width, &height, &nr_components, 0);

        #endif

        LOGD("path %s width: %d, height: %d, channel %d\n",
                path_buffer, width, height, nr_components);

        if (data) {
                GLenum format = 0;
                if (nr_components == 1)
                format = GL_RED;
                else if (nr_components == 3)
                format = GL_RGB;
                else if (nr_components == 4)
                format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexImage2D(
                        GL_TEXTURE_2D, 0, (int) format, width, height, 0,
                        format, GL_UNSIGNED_BYTE, data
                );
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(
                        GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR
                );
                glTexParameteri(
                        GL_TEXTURE_2D,
                        GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR
                );

                stbi_image_free(data);
        } else {
                LOGE("Failed to load texture: %s\n", path_buffer);
                stbi_image_free(data);
        }
        return texture_id;
}

int ap_texture_init(struct AP_Texture *texture)
{
        if (texture == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        memset(texture, 0, sizeof(struct AP_Texture));
        return 0;
}

int ap_texture_set_type(struct AP_Texture *texture, const char *name)
{
        if (texture == NULL || name == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        if (texture->type != NULL) {
                LOGD("Try to free old texture type pointer: 0X%p", texture);
                free(texture->type);
                texture->type = NULL;
        }
        texture->type = malloc(sizeof(char) * (strlen(name) + 1) );
        strcpy(texture->type, name);
        return 0;
}

int ap_texture_set_path(struct AP_Texture *texture, const char *pathName)
{
        if (texture == NULL || pathName == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        if (texture->path != NULL) {
                LOGD("Try to free old texture path pointer: 0X%p", texture);
                free(texture->path);
                texture->path = NULL;
        }
        texture->path = malloc(sizeof(char) * (strlen(pathName) + 1) );
        strcpy(texture->path, pathName);
        return 0;
}