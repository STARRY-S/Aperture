#ifndef AP_TEXTURE_H
#define AP_TEXTURE_H

#include "cglm/cglm.h"

struct AP_Texture {
        unsigned int id;
        char *type;
        char *path;
        vec4 diffuse;
        vec4 specular;
};

int ap_texture_preload(
        const char *name,       // texture_diffuse or other type
        const char *path,       // file name
        const char *dir,        // path to file
        bool gamma
);

int ap_texture_generate(
        unsigned int *texture_id,
        const char *name,
        const char *path,
        const char *directory,
        bool gamma
);

struct AP_Texture *ap_texture_get_ptr_from_path(const char *path);

int ap_texture_free();

struct AP_Texture *ap_texture_get_ptr(unsigned int id);

/**
 * Load texture from file, return the texture ID.
 * @param path texture file path
 * @param format texture format
 * @return Texture ID
 */
unsigned int ap_texture_load(const char *const path, int format);

/**
 * Load texture from file
 * @param path file name
 * @param directory path name
 * @param gamma reserve
 * @return texture id, 0 if failed
 */
unsigned int ap_texture_from_file(
        const char *path,
        const char *directory,
        bool gamma
);

/**
 * Initialize texture struct object, set the memory data to zero
 * @param texture
 * @return AP_Types
 */
int ap_texture_init(struct AP_Texture *texture);

int ap_texture_set_type(struct AP_Texture *texture, const char *name);

int ap_texture_set_path(struct AP_Texture *texture, const char *pathName);

#endif // AP_TEXTURE_H
