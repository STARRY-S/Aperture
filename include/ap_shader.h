#ifndef AP_SHADER_H
#define AP_SHADER_H

/**
 * Defination of Aperture built in GLSL (uniform) shader variable names
 * Apertrue Shader Perspective:  AP_SP_<NAME>
 *                               AP_SP_<STRUCT>_<NAME>
 * Aperture Shader Orthographic: AP_SO_<NAME>
 */

#if AP_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

// Perspective vertex shader uniform variables
#define AP_SP_MODEL             "model"
#define AP_SP_VIEW              "view"
#define AP_SP_PROJECTION        "projection"
#define AP_SP_VIEW_DISTANCE     "view_distance"
// Perspective fragment shader uniform variables
#define AP_SP_MATERIALS_NR      4
#define AP_SP_MATERIAL          "material"
#define AP_SP_MT_DIFFUSE        "material_%d.diffuse"
#define AP_SP_MT_DIFFUSE        "material_%d.diffuse"
#define AP_SP_MT_SPECULAR       "material_%d.specular"
#define AP_SP_MT_NORMAL         "material_%d.normal"
#define AP_SP_MT_HEIGHT         "material_%d.height"
#define AP_SP_MT_SHININESS      "material_%d.shininess"

// AP_LIGHT_POINT_NUM = 128
#define AP_SP_POINT_LIGHT       "point_light"
#define AP_SP_PL_POSITION       "point_light[%d].position"
#define AP_SP_PL_CONSTANT       "point_light[%d].constant"
#define AP_SP_PL_LINEAR         "point_light[%d].linear"
#define AP_SP_PL_QUADRATIC      "point_light[%d].quadratic"
#define AP_SP_PL_AMBIENT        "point_light[%d].ambient"
#define AP_SP_PL_DIFFUSE        "point_light[%d].diffuse"
#define AP_SP_PL_SPECULAR       "point_light[%d].specular"

#define AP_SP_DIRECT_LIGHT      "direct_light"
#define AP_SP_DL_DIRECTION      "direct_light.direction"
#define AP_SP_DL_AMBIENT        "direct_light.ambient"
#define AP_SP_DL_DIFFUSE        "direct_light.diffuse"
#define AP_SP_DL_SPECULAR       "direct_light.specular"

#define AP_SP_SPOT_LIGHT        "spot_light"
#define AP_SP_SL_POSITION       "spot_light.position"
#define AP_SP_SL_DIRECTION      "spot_light.direction"
#define AP_SP_SL_CUTOFF         "spot_light.cut_off"
#define AP_SP_SL_OUTER_CUTOFF   "spot_light.outer_cut_off"
#define AP_SP_SL_CONSTANT       "spot_light.constant"
#define AP_SP_SL_LINEAR         "spot_light.linear"
#define AP_SP_SL_QUADRATIC      "spot_light.quadratic"
#define AP_SP_SL_AMBIENT        "spot_light.ambient"
#define AP_SP_SL_DIFFUSE        "spot_light.diffuse"
#define AP_SP_SL_SPECULAR       "spot_light.specular"

#define AP_SP_SPOT_LIGHT_ENABLED "spot_light_enabled"
#define AP_SP_POINT_LIGHT_ENABLED "point_light_enabled"
#define AP_SP_ENV_LIGHT_ENABLED  "env_light_enabled"
#define AP_SP_MATERIAL_NUMBER    "material_number"
#define AP_SP_VIEW_POS           "view_pos"

// Orthographic vertex shader uniform variables
#define AP_SO_PROJECTION        "projection"
// Orthographic fragment shader uniform variables
#define AP_SO_TEXTURE           "texture%d"
#define AP_SO_COLOR             "color"
#define AP_SO_TEXTURE_NUM       "texture_num"

/**
 * @brief Generate a openGL program with shader
 * @param vshader_path path to vertex shader
 * @param fshader_path path to fragment shader
 * @param shader_id [out] (shader) program ID
 * @return int AP_Types
 */
int ap_shader_generate(
        const char* vshader_path,
        const char* fshader_path,
        unsigned int *shader_id
);

int ap_shader_use(unsigned shader_program_id);

int ap_shader_free();

int ap_shader_set_float(GLuint program, const char *const name, float num);

int ap_shader_set_int  (GLuint program, const char *const name, GLuint num);

int ap_shader_set_vec3 (GLuint program, const char *const name, float *vec);

int ap_shader_set_vec4 (GLuint program, const char *const name, float *vec);

int ap_shader_set_mat4 (GLuint program, const char *const name, float *mat);

unsigned int ap_get_current_shader();

#endif // AP_SHADER_H
