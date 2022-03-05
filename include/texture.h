#ifndef GAME_ENGINE_TEXTURE_H
#define GAME_ENGINE_TEXTURE_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#ifndef __ANDROID__
#include <GLFW/glfw3.h>
#endif

#include <stdio.h>
#include <stdbool.h>

struct Texture {
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
GLuint load_texture(const char *const path, int format);

/**
 * Load texture from file.
 * @param path file name
 * @param directory Directory of file
 * @param gamma false, unused, reserve
 * @return unsigned int, texture id
 */
unsigned int texture_from_file(const char *path, const char *directory, bool gamma);

/**
 * Initialize texture struct object, set the memory data to zero
 * @param pTexture
 * @return GE_Types
 */
int init_texture(struct Texture *pTexture);


int texture_set_type(struct Texture *pTexture, const char *typeName);


int texture_set_path(struct Texture *pTexture, const char *pathName);

#endif //GAME_ENGINE_TEXTURE_H
