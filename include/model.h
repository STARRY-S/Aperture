#ifndef __MODEL_H__
#define __MODEL_H__

#include "ge_utils.h"
#include "mesh.h"
#include "cvector.h"

// Model struct object definition
struct Model {
    struct Texture *pTextureLoaded;
    int iTextureLoadedLength;
    struct Mesh *pMeshes;
    int iMeshLength;

    char *pDirectory;   // malloc, need free
};

/**
 * Model constructor
 * @param path - file path
 * @param format - file format, or null
 * @param gamma - default to null
 * @return GE_Types
 */
int init_model(struct Model *pModel, const char *path, bool gamma);


int draw_model(struct Model *pModel, unsigned int shader);

#endif
