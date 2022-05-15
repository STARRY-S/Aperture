#include "ap_texture.h"
#include "ap_utils.h"
#include "ap_cvector.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

struct AP_Vector texture_vector = { 0, 0, 0, 0 };

int ap_texture_generate(
        unsigned int *texture_id,
        int type,
        const char *path,
        const char *directory,
        bool gamma)
{
        *texture_id = 0;
        if (texture_vector.data == NULL) {
                ap_vector_init(&texture_vector, AP_VECTOR_TEXTURE);
        }
        unsigned int id = ap_texture_from_file(path, directory, gamma);
        if (id == 0) {
                return AP_ERROR_TEXTURE_FAILED;
        }

        struct AP_Texture texture;
        memset(&texture, 0, sizeof(struct AP_Texture));

        texture.id = id;
        texture.type = type;
        ap_texture_set_path(&texture, path);

        ap_vector_push_back(&texture_vector, (const char*) &texture);
        *texture_id = id;

        return 0;
}

int ap_texture_generate_RGBA(
        unsigned int *texture_id,
        float color[4],
        int size,
        int type)
{
        if (color == NULL || size <= 0 || size >= 10000) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (texture_vector.data == NULL) {
                ap_vector_init(&texture_vector, AP_VECTOR_TEXTURE);
        }
        unsigned int id = ap_texture_from_RGBA(color, size);
        if (id == 0) {
                LOGW("failed to load texture from color (%.1f,%.1f,%.1f,%.1f)",
                        color[0], color[1], color[2], color[3]
                );
                return AP_ERROR_TEXTURE_FAILED;
        }
        struct AP_Texture texture;
        memset(&texture, 0, sizeof(struct AP_Texture));

        texture.id = id;
        texture.type = type;
        memcpy(&texture.RGBA, color, sizeof(float) * 4);
        // texture.diffuse
        ap_vector_push_back(&texture_vector, (char*) &texture);
        *texture_id = id;

        return 0;
}

struct AP_Texture *ap_texture_get_ptr(unsigned int id)
{
        if (id == 0 || texture_vector.data == NULL) {
                // INVALID_PARAMETER
                return NULL;
        }

        struct AP_Texture *ptr = (struct AP_Texture*) texture_vector.data;
        for (int i = 0; i < texture_vector.length; ++i) {
                if (id == ptr[i].id) {
                        return ptr + i;
                }
        }

        return NULL;
}

struct AP_Texture *ap_texture_get_ptr_by_path(const char *path)
{
        if (path == NULL || texture_vector.data == NULL) {
                LOGE("ap_texture_get_ptr_by_path: INVALID PARAM");
                return NULL;
        }

        struct AP_Texture *ptr = (struct AP_Texture*) texture_vector.data;
        for (int i = 0; i < texture_vector.length; ++i) {
                if (ptr[i].path && strcmp(path, ptr[i].path) == 0) {
                        return ptr + i;
                }
        }
        return NULL;
}

struct AP_Texture *ap_texture_get_ptr_by_RGBA(float color[4])
{
        if (color == NULL || texture_vector.data == NULL) {
                LOGE("ap_texture_get_ptr_by_RGBA: INVALID PARAM");
                return NULL;
        }

        struct AP_Texture *ptr = (struct AP_Texture*) texture_vector.data;
        for (int i = 0; i < texture_vector.length; ++i) {
                if (EQUAL(color[0], ptr[i].RGBA[0])
                   && EQUAL(color[1], ptr[i].RGBA[1])
                   && EQUAL(color[2], ptr[i].RGBA[2])
                   && EQUAL(color[3], ptr[i].RGBA[3]) )
                {
                        return ptr + i;
                }
        }
        return NULL;
}

int ap_texture_get_type(unsigned int id)
{
        struct AP_Texture *ptr = ap_texture_get_ptr(id);
        if (ptr == NULL) {
                return 0;
        }

        return ptr->type;
}

unsigned int ap_texture_from_file(
        const char *path,
        const char *directory,
        bool gamma)
{
        // stbi_set_flip_vertically_on_load(true);
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        if (texture_id == 0) {
                LOGW("glGenTextures failed");
        }

        int width, height, nr_components;
        int buffer_length = strlen(path) + strlen(directory) + 1;
        char *path_buffer = AP_MALLOC(sizeof(char) * buffer_length);
        if (path_buffer == NULL) {
                LOGE("malloc failed");
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
                AP_FREE(path_buffer);
                path_buffer = NULL;
                return 0;
        }
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

        if (!data) {
                LOGE("Failed to load texture: %s", path_buffer);
                glDeleteTextures(1, &texture_id);
                AP_FREE(path_buffer);
                path_buffer = NULL;
                return 0;
        }

        int format = 0;
        if (nr_components == 1) {
                format = GL_RED;
        } else if (nr_components == 3) {
                format = GL_RGB;
        } else if (nr_components == 4) {
                format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(
                GL_TEXTURE_2D, 0, format, width, height, 0,
                format, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST
        );
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_NEAREST
        );

        stbi_image_free(data);
        AP_FREE(path_buffer);
        path_buffer = NULL;
        return texture_id;
}

unsigned int ap_texture_from_RGBA(vec4 color, int size)
{
        unsigned char* data = (unsigned char*) AP_MALLOC(
                4 * size * size * sizeof(unsigned char)
        );
        memset(data, 0, 4 * size * size * sizeof(unsigned char));
        if (data == NULL) {
                LOGE("malloc failed");
                return 0;
        }

        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set texture wrap parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filter parameters
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST
        );
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_NEAREST
        );

        for (int i = 0; i < (size * size); ++i) {
                for (int j = 0; j < 4; ++j) {
                        data[i * 4 + j] = (color[j] * 255);
                }
        }
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA, size, size,
                0, GL_RGBA, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        AP_FREE(data);

        return texture;
}

int ap_texture_init(struct AP_Texture *texture)
{
        if (texture == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        memset(texture, 0, sizeof(struct AP_Texture));
        return 0;
}

int ap_texture_free()
{
        if (texture_vector.data == NULL) {
                return 0;
        }

        struct AP_Texture *ptr = (struct AP_Texture*) texture_vector.data;
        for (int i = 0; i < texture_vector.length; ++i) {
                glDeleteTextures(1, &(ptr[i].id));
                AP_FREE(ptr[i].path);
        }

        ap_vector_free(&texture_vector);
        return 0;
}

int ap_texture_set_path(struct AP_Texture *texture, const char *pathName)
{
        if (texture == NULL || pathName == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        if (texture->path != NULL) {
                LOGD("Try to AP_FREE old texture path pointer: 0X%p", texture);
                AP_FREE(texture->path);
                texture->path = NULL;
        }
        texture->path = AP_MALLOC(sizeof(char) * (strlen(pathName) + 1) );
        strcpy(texture->path, pathName);
        return 0;
}