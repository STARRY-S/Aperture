/**
 * @file ap_model.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief model related function
 *
 */
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

/**
 * Model struct object definition
 */
struct AP_Model {
        /** model ID */
        int id;
        /** model position */
        float pos[3];
        /** scale vector */
        float scale[3];
        /** rotate angle */
        float rotate_angle;
        /** rotate axis vector */
        float rotate_axis[3];

        /** textures of the model */
        struct AP_Texture *texture;
        /** texture length of the model */
        int texture_length;
        /** meshes of the model */
        struct AP_Mesh *mesh;
        /** mesh length of the model */
        int mesh_length;
        /** model directory, used for load texture from the same directory */
        char *directory;
};

/**
 * @brief Generate a model
 * @param path [in] path to the model file
 * @param model_id [out] model id
 * @return int AP_Error_Types
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

/**
 * @brief Set scale parameters of current model
 *
 * @param float[3]
 * @return int
 */
int ap_model_set_scale(float scale[3]);
int ap_model_set_pos(float pos[3]);
int ap_model_set_rotate(float axis[3], float angle);

/**
 * @brief Release model object by its id
 *
 * @param id
 * @return int
 */
int ap_model_free(int id);

/**
 * @brief Release all generated model data
 *
 * @return int
 */
int ap_model_free_all();

/**
 * @brief Get model pointer by its ID
 *
 * @param id
 * @return struct AP_Model*
 */
struct AP_Model *ap_model_get_ptr(int id);

#endif // AP_MODEL_H
