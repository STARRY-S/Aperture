#ifndef AP_MODEL_H
#define AP_MODEL_H

#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_cvector.h"

struct AP_Model {
        struct AP_Texture *texture;
        int texture_length;
        struct AP_Mesh *mesh;
        int mesh_length;
        char *directory;
};

int ap_model_init(struct AP_Model *model, const char *path, bool gamma);

int ap_model_draw_ptr(struct AP_Model *model, unsigned int shader);

#endif // AP_MODEL_H
