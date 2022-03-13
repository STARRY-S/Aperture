#ifndef AP_TEXTURE_H
#define AP_TEXTURE_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#ifndef __ANDROID__
#include <GLFW/glfw3.h>
#endif

#include <stdio.h>
#include <stdbool.h>

struct AP_Texture {
        unsigned int id;
        char *type; // malloc, need free
        char *path; // malloc, need free
};

/**
 * Load texture from file, return the texture ID.
 * @param path texture file path
 * @param format texture format
 * @return Texture ID
 */
GLuint ap_texture_load(const char *const path, int format);

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
 * @param pTexture
 * @return AP_Types
 */
int ap_texture_init(struct AP_Texture *pTexture);


int ap_texture_set_type(struct AP_Texture *pTexture, const char *typeName);


int ap_texture_set_path(struct AP_Texture *pTexture, const char *pathName);

#endif // AP_TEXTURE_H
