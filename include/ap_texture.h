/**
 * @file ap_texture.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Texture functions definition
 */
#ifndef AP_TEXTURE_H
#define AP_TEXTURE_H

#include "ap_utils.h"
#include "ap_shader.h"
#include "cglm/cglm.h"

/**
 * Define the maximum number of texture unit (็บน็ๅๅ)
 */
#define AP_TEXTURE_UNIT_MAX_NUM 4

typedef enum {
        AP_TEXTURE_TYPE_UNKNOWN = 0,
        /** texture_diffuse */
        AP_TEXTURE_TYPE_DIFFUSE = 0x1001,
        /** texture_specular */
        AP_TEXTURE_TYPE_SPECULAR,
        /** texture_normal */
        AP_TEXTURE_TYPE_NORMAL,
        /** texture_height */
        AP_TEXTURE_TYPE_HEIGHT
} AP_Texture_types;

/**
 * @brief Convert texture type to string
 *
 * @param t
 * @return const char*
 */
static inline const char* ap_texture_type_2_str(int t)
{
        switch (t)
        {
        case AP_TEXTURE_TYPE_DIFFUSE:
                return AP_SP_MT_DIFFUSE;
        case AP_TEXTURE_TYPE_SPECULAR:
                return AP_SP_MT_SPECULAR;
        case AP_TEXTURE_TYPE_NORMAL:
                return AP_SP_MT_NORMAL;
        case AP_TEXTURE_TYPE_HEIGHT:
                return AP_SP_MT_HEIGHT;
        }

        return NULL;
}

/**
 * @brief Texture struct object definition
 *
 */
struct AP_Texture {
        /** OpenGL texture ID */
        unsigned int id;
        /**
         * texture type
         * @see AP_Texture_types
         */
        int type;
        /** path to the texture file (if the texture is an image) */
        char *path;
        /**
         * RGBA color value of the texture
         * (if the texture is generated from a single color)
         */
        float RGBA[4];
};

/**
 * @brief Generate a texture from its file name and directory
 * and store its OpenGL texture ID in vector.
 *
 * @param texture_id [out] pointer points to the ID of texture generated
 * @param type [in] the type of the texture (AP_Texture_types)
 * @param path [in] name of the image (PNG or JPG)
 * @param directory [in] directory to the image file (UNIX format)
 * @param gamma reserve, not use currently
 * @return int AP_Error_Types
 */
int ap_texture_generate(
        unsigned int *texture_id,
        int type,
        const char *path,
        const char *directory,
        bool gamma
);

/**
 * @brief Genrerate a texture from a single RGBA color value,
 * and store its OpenGL Texture ID in vector.
 *
 * @param texture_id
 * @param type
 * @return int
 */
int ap_texture_generate_rgba(
        unsigned int *texture_id,
        float color[4],
        int size,
        int type
);

/**
 * @brief Get the pointer of struct AP_Texture in vector
 *
 * @param path name of the image file (PNG or JPG)
 * @return struct AP_Texture*, NULL when not found
 */
struct AP_Texture *ap_texture_get_ptr_by_path(const char *path);

/**
 * @see ap_texture_get_ptr_by_path
 */
struct AP_Texture *ap_texture_get_ptr_by_rgba(float color[4]);

/**
 * @brief Get the pointer of struct AP_Texture by OpenGL texture ID
 *
 * @param id
 * @return struct AP_Texture*
 */
struct AP_Texture *ap_texture_get_ptr(unsigned int id);

/**
 * Load texture from file and return its OpenGL texture ID
 * @param path file name
 * @param directory path name
 * @param gamma reserve
 * @return OpenGL texture id, 0 on error
 */
unsigned int ap_texture_from_file(
        const char *path,
        const char *directory,
        bool gamma
);

/**
 * @brief Generate RGBA color to OpenGL Texture ID
 *
 * @param color vec4 RGBA color
 * @param size  image size (in pixel)
 * @return OpenGL Texture ID, 0 on error
 */
unsigned int ap_texture_from_rgba(float color[4], int size);

/**
 * @brief Generate texture from RGBA data
 *
 * @param data pointer points to rgba data
 * @param w width
 * @param h height
 * @return OpenGL Texture ID
 */
unsigned int ap_texture_from_data_rgba(const unsigned char *data, int w, int h);

/**
 * Initialize texture struct object, set the memory data to zero
 * @param texture
 * @return AP_Error_Types
 */
int ap_texture_init(struct AP_Texture *texture);

/**
 * @brief Set path to struct AP_Texture object
 *
 * @param texture pointer points to struct AP_Texture
 * @param name name of the image file (PNG, JPG)
 * @return int AP_Error_Types
 */
int ap_texture_set_path(struct AP_Texture *texture, const char *name);

/**
 * @brief free one texture ID
 *
 * @param id
 * @return int
 */
int ap_texture_free(int id);

/**
 * @brief finalize vector memory
 * @return int AP_Error_Types
 */
int ap_texture_free_all();

#endif // AP_TEXTURE_H
