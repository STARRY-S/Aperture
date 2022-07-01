#include "ap_utils.h"
#include "ap_light.h"
#include "ap_render.h"
#include "ap_shader.h"
#include "ap_cvector.h"
#include "ap_texture.h"
#include "ap_math.h"

#define FLOAT_SIZE sizeof(float)
#define LIGHT_SIZE sizeof(struct AP_Light)

static struct AP_Vector point_light_vector = { 0, 0, 0, 0 };
static struct AP_Light direct_light;
static struct AP_Light spot_light;

bool ap_light_is_valid_type(int t)
{
        switch (t)
        {
        case AP_LIGHT_POINT:
        case AP_LIGHT_DIRECTIONAL:
        case AP_LIGHT_SPOT:
                return true;
        }
        return false;
}

int ap_light_setup_spot(
        float ambient[3],
        float diffuse[3],
        float specular[3],
        float param[AP_LIGHT_PARAM_NUM])
{
        memset(&spot_light, 0, LIGHT_SIZE);
        spot_light.type = AP_LIGHT_SPOT;
        ap_v3_copy(spot_light.ambient, ambient);
        ap_v3_copy(spot_light.diffuse, diffuse);
        ap_v3_copy(spot_light.specular, specular);
        memcpy(spot_light.param, param, AP_LIGHT_PARAM_NUM * FLOAT_SIZE);
        return 0;
}

int ap_light_setup_directional(
        float direction[3],
        float ambient[3],
        float diffuse[3],
        float specular[3])
{
        memset(&direct_light, 0, LIGHT_SIZE);
        direct_light.type = AP_LIGHT_DIRECTIONAL;
        ap_v3_copy(direct_light.direction, direction);
        ap_v3_copy(direct_light.ambient, ambient);
        ap_v3_copy(direct_light.diffuse, diffuse);
        ap_v3_copy(direct_light.specular, specular);
        return 0;
}

int ap_light_generate_point(
        unsigned int *light_id,
        float position[3],
        float ambient[3],
        float diffuse[3],
        float specular[3],
        float param[AP_LIGHT_PARAM_NUM])
{
        if (!light_id || !param) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (!point_light_vector.data) {
                ap_vector_init(&point_light_vector, AP_VECTOR_LIGHT);
        }

        if (point_light_vector.length >= AP_LIGHT_POINT_NUM) {
                LOGE("unable to generate new spot lights");
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Light light;
        memset(&light, 0, sizeof(light));

        light.type = AP_LIGHT_POINT;
        ap_v3_copy(light.position, position);
        ap_v3_copy(light.ambient, ambient);
        ap_v3_copy(light.diffuse, diffuse);
        ap_v3_copy(light.specular, specular);

        memcpy(light.param, param, FLOAT_SIZE * AP_LIGHT_PARAM_NUM);

        unsigned id = point_light_vector.length + 1;
        light.id = id;
        int ret = ap_vector_push_back(&point_light_vector, (char*) &light);
        if (ret) {
                LOGE("failed to generate light");
                AP_CHECK(ret);
                return AP_ERROR_INIT_FAILED;
        }
        *light_id = id;

        return 0;
}

int ap_light_render()
{
        if (!point_light_vector.data) {
                LOGE("failed to render lights: not initialized");
                return AP_ERROR_INIT_FAILED;
        }
        char buffer[AP_DEFAULT_BUFFER_SIZE] = { 0 };

        unsigned int old_shader = ap_get_current_shader();
        unsigned int shader = 0;
        ap_render_get_persp_shader(&shader);
        if (shader == 0) {
                LOGE("failed to render lights: renderer not initialized");
                return AP_ERROR_INIT_FAILED;
        }

        ap_shader_use(shader);
        struct AP_Light *light = (struct AP_Light*) point_light_vector.data;
        int point_nr = 0;
        // send point lights data
        for (int i = 0; i < point_light_vector.length
                && point_nr < AP_LIGHT_POINT_NUM; ++i)
        {
                struct AP_Light *p = light + i;

                if (p->type != AP_LIGHT_POINT) {
                        continue;
                }
                // position
                sprintf(buffer, AP_SP_PL_POSITION, i);
                ap_shader_set_vec3(shader, buffer, p->position);
                // ambient
                sprintf(buffer, AP_SP_PL_AMBIENT, i);
                ap_shader_set_vec3(shader, buffer, p->ambient);
                // diffuse
                sprintf(buffer, AP_SP_PL_DIFFUSE, i);
                ap_shader_set_vec3(shader, buffer, p->diffuse);
                // specular
                sprintf(buffer, AP_SP_PL_SPECULAR, i);
                ap_shader_set_vec3(shader, buffer, p->specular);
                // constant
                sprintf(buffer, AP_SP_PL_CONSTANT, i);
                ap_shader_set_float(shader, buffer, p->param[0]);
                // linear
                sprintf(buffer, AP_SP_PL_LINEAR, i);
                ap_shader_set_float(shader, buffer, p->param[1]);
                // quadratic
                sprintf(buffer, AP_SP_PL_QUADRATIC, i);
                ap_shader_set_float(shader, buffer, p->param[2]);
        }

        // send directional light data
        if (direct_light.type == AP_LIGHT_DIRECTIONAL) {
                // direction
                ap_shader_set_vec3(shader, AP_SP_DL_DIRECTION,
                        direct_light.direction);
                // ambient
                ap_shader_set_vec3(shader, AP_SP_DL_AMBIENT,
                        direct_light.ambient);
                // diffuse
                ap_shader_set_vec3(shader, AP_SP_DL_DIFFUSE,
                        direct_light.diffuse);
                // specular
                ap_shader_set_vec3(shader, AP_SP_DL_SPECULAR,
                        direct_light.specular);
        }

        // send spot light data
        if (spot_light.type == AP_LIGHT_SPOT) {
                // ambient
                ap_shader_set_vec3(shader, AP_SP_SL_AMBIENT,
                        spot_light.ambient);
                // diffuse
                ap_shader_set_vec3(shader, AP_SP_SL_AMBIENT,
                        spot_light.diffuse);
                // specular
                ap_shader_set_vec3(shader, AP_SP_SL_SPECULAR,
                        spot_light.specular);
                // constant
                ap_shader_set_float(shader, AP_SP_SL_CONSTANT,
                        spot_light.param[0]);
                // linear
                ap_shader_set_float(shader, AP_SP_SL_LINEAR,
                        spot_light.param[1]);
                // quadratic
                ap_shader_set_float(shader, AP_SP_SL_QUADRATIC,
                        spot_light.param[2]);
                // cut_off
                ap_shader_set_float(shader, AP_SP_SL_CUTOFF,
                        spot_light.param[3]);
                // outer_cut_off
                ap_shader_set_float(shader,
                        AP_SP_SL_OUTER_CUTOFF,
                        spot_light.param[4]
                );
        }
        ap_shader_use(old_shader);

        return 0;
}

int ap_light_set_material_shininess(float shininess)
{
        unsigned int old_shader = ap_get_current_shader();
        unsigned int shader = 0;
        ap_render_get_persp_shader(&shader);
        if (shader == 0) {
                LOGE("failed to render lights: renderer not initialized");
        }

        ap_shader_use(shader);
        char buffer[AP_DEFAULT_BUFFER_SIZE] = {0};
        for (int i = 0; i < AP_TEXTURE_UNIT_MAX_NUM; ++i) {
                sprintf(buffer, AP_SP_MT_SHININESS, i);
                ap_shader_set_float(shader, buffer, shininess);
        }
        ap_shader_use(old_shader);

        return 0;
}

int ap_light_free_point_light(int id)
{
        if (id <= 0) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Light *data = (struct AP_Light *) point_light_vector.data;
        for (int i = 0; i < point_light_vector.length; ++i) {
                if (data[i].id != id) {
                        continue;
                }
                ap_vector_remove_data(
                        &point_light_vector,
                        (char*) (data + i),
                        (char*) (data + i + 1),
                        sizeof(struct AP_Light)
                );
                break;
        }

        return 0;
}

int ap_light_free_all()
{
        ap_vector_free(&point_light_vector);
        return 0;
}

struct AP_Light* ap_light_get_point_light_ptr(int id)
{
        if (id <= 0) {
                return NULL;
        }
        struct AP_Light *data = (struct AP_Light*) point_light_vector.data;
        for (int i = 0; i < point_light_vector.length; ++i) {
                if (data[i].id == id) {
                        return data + i;
                }
        }
        return NULL;
}

struct AP_Light* ap_light_get_direct_light_ptr()
{
        return &direct_light;
}

struct AP_Light* ap_light_get_spot_light_ptr()
{
        return &spot_light;
}
