#ifndef AP_MODEL_H
#define AP_MODEL_H

#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_cvector.h"

struct AP_Model {
        int id;

        struct AP_Texture *texture;
        int texture_length;
        struct AP_Mesh *mesh;
        int mesh_length;
        char *directory;
};

struct AP_Model_Thread_Param {
        unsigned int id;
        char *path;
        void *context;
        ap_callback_func_t cb;
};

/**
 * @brief generate a model
 * @param path [in] path to model
 * @param model_id [out] model id
 * @return int AP_Types
 */
int ap_model_generate(const char *path, unsigned int *model_id);

int ap_model_generate_async(
        const char *path,
        void *context,
        ap_callback_func_t cb
);

/**
 * @brief init a model struct object with its pointer
 * @param model pointer points to a new model
 * @param path model path
 * @param gamma reserve, default false
 * @return int AP_Types
 */
int ap_model_init_ptr(struct AP_Model *model, const char *path, bool gamma);

int ap_model_use(unsigned int model_id);

int ap_model_free();

int ap_model_draw_ptr_shader(struct AP_Model *model, unsigned int shader);

int ap_model_draw();

struct AP_Model* ap_get_current_model();

#endif // AP_MODEL_H
