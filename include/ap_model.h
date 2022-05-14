#ifndef AP_MODEL_H
#define AP_MODEL_H

#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_cvector.h"

#ifndef AP_MODEL_CUBE_PATH
#define AP_MODEL_CUBE_PATH "res/cube/cube.obj"
#endif

#ifndef AP_MODEL_BALL_PATH
#define AP_MODEL_BALL_PATH "res/ball/ball.obj"
#endif

struct AP_Model {
        int id;
        float pos[3];       // position of the model
        float scale[3];     // scale of the model
        float rotate_angle;   // rotate degree
        float rotate_axis[3]; // rotate axis

        struct AP_Texture *texture;
        int texture_length;
        struct AP_Mesh *mesh;
        int mesh_length;
        char *directory;
};

/**
 * @brief Generate a model
 * @param path [in] path to the model file
 * @param model_id [out] model id
 * @return int AP_Types
 */
int ap_model_generate(const char *path, unsigned int *model_id);

/**
 * @brief Use model
 *
 * @param model_id
 * @return int
 */
int ap_model_use(unsigned int id);

/**
 * @brief Draw current model
 *
 * @return int
 */
int ap_model_draw();

int ap_model_set_scale(float scale[3]);
int ap_model_set_pos(float pos[3]);
int ap_model_set_rotate(float axis[3], float angle);

int ap_model_free();

#endif // AP_MODEL_H
