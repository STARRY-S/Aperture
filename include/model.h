#ifndef AP_MODEL_H
#define AP_MODEL_H

#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_cvector.h"

// Model struct object definition
struct Model {
        struct Texture *pTextureLoaded;
        int iTextureLoadedLength;
        struct AP_Mesh *pMeshes;
        int iMeshLength;

        char *pDirectory;   // malloc, need free
};

/**
 * Model constructor
 * @param path - file path
 * @param format - file format, or null
 * @param gamma - default to null
 * @return AP_Types
 */
int init_model(struct Model *pModel, const char *path, bool gamma);

int draw_model(struct Model *pModel, unsigned int shader);

#endif // AP_MODEL_H
