#ifndef AP_TEXTURE_H
#define AP_TEXTURE_H

#include "cglm/cglm.h"

/**
 * Define the maximum number of texture unit (纹理单元)
 */
#define AP_TEXTURE_UNIT_MAX_NUM 16

static inline int EQUAL(float a, float b)
{
        return ((a - b) < 0.001 && (a - b) > -0.001);
}

typedef enum {
        AP_TEXTURE_TYPE_UNKNOWN = 0,
        AP_TEXTURE_TYPE_DIFFUSE = 0x1001,       // texture_diffuse
        AP_TEXTURE_TYPE_SPECULAR,               // texture_specular
        AP_TEXTURE_TYPE_NORMAL,                 // texture_normal
        AP_TEXTURE_TYPE_HEIGHT                  // texture_height
} AP_Texture_types;

static inline const char* ap_texture_type_2_str(int t)
{
        switch (t)
        {
        case AP_TEXTURE_TYPE_DIFFUSE:
                return "material[%d].diffuse";
        case AP_TEXTURE_TYPE_SPECULAR:
                return "material[%d].specular";
        case AP_TEXTURE_TYPE_NORMAL:
                return "material[%d].normal";
        case AP_TEXTURE_TYPE_HEIGHT:
                return "material[%d].height";
        }

        return "texture_unknown";
}

struct AP_Texture {
        unsigned int id;
        int type;
        char *path;
        float RGBA[4];
};

/**
 * @brief Generate a texture from specific file and directory
 * and store its OpenGL texture ID in vector.
 *
 * @param texture_id [out] pointer points to the ID of texture generated
 * @param type [in] the type of the texture (AP_Texture_types)
 * @param path [in] name of the image (PNG or JPG)
 * @param directory [in] directory to the image file (UNIX format)
 * @param gamma reserve, not use currently
 * @return int AP_Types
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
int ap_texture_generate_RGBA(
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
struct AP_Texture *ap_texture_get_ptr_by_RGBA(float color[4]);

/**
 * @brief Get the pointer of struct AP_Texture by OpenGL texture ID
 *
 * @param id
 * @return struct AP_Texture*
 */
struct AP_Texture *ap_texture_get_ptr(unsigned int id);

/**
 * Load texture from file
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
unsigned int ap_texture_from_RGBA(float color[4], int size);

/**
 * Initialize texture struct object, set the memory data to zero
 * @param texture
 * @return AP_Types
 */
int ap_texture_init(struct AP_Texture *texture);

/**
 * @brief Set path to struct AP_Texture object
 *
 * @param texture pointer points to struct AP_Texture
 * @param name name of the image file (PNG, JPG)
 * @return int AP_Types
 */
int ap_texture_set_path(struct AP_Texture *texture, const char *name);

/**
 * @brief finalize vector memory
 * @return int AP_Types
 */
int ap_texture_free();

#endif // AP_TEXTURE_H
