#ifndef AP_SHADER_H
#define AP_SHADER_H

/**
 * Defination of Aperture built in GLSL (uniform) shader variable names
 * Apertrue Shader Perspective:  AP_SP_<NAME>
 *                               AP_SP_<STRUCT>_<NAME>
 * Aperture Shader Orthographic: AP_SO_<NAME>
 */
typedef enum {
        // Perspective vertex shader uniform variables
        AP_SP_MODEL = 0x3101,
        AP_SP_VIEW,
        AP_SP_PROJECTION,
        // Perspective fragment shader uniform variables
        AP_SP_MATERIALS = 0x3201,
        AP_SP_MT_DIFFUSE,
        AP_SP_MT_SPECULAR,
        AP_SP_MT_SHININESS,

        AP_SP_POINT_LIGHT,
        AP_SP_PL_POSITION,
        AP_SP_PL_CONSTANT,
        AP_SP_PL_LINEAR,
        AP_SP_PL_QUADRATIC,
        AP_SP_PL_AMBIENT,
        AP_SP_PL_DIFFUSE,
        AP_SP_PL_SPECULAR,

        AP_SP_DIRECT_LIGHT,
        AP_SP_DL_DIRECTION,
        AP_SP_DL_AMBIENT,
        AP_SP_DL_DIFFUSE,
        AP_SP_DL_SPECULAR,

        AP_SP_SPOT_LIGHT,
        AP_SP_SL_POSITION,
        AP_SP_SL_DIRECTION,
        AP_SP_SL_CUTOFF,
        AP_SP_SL_OUTER_CUTOFF,
        AP_SP_SL_CONSTANT,
        AP_SP_SL_LINEAR,
        AP_SP_SL_QUADRATIC,
        AP_SP_SL_AMBIENT,
        AP_SP_SL_DIFFUSE,
        AP_SP_SL_SPECULAR,

        AP_SP_SPOT_LIGHT_ENABLED,
        AP_SP_MATERIAL_NUMBER,
        AP_SP_VIEW_POS,

        // Orthographic vertex shader uniform variables
        AP_SO_PROJECTION = 0x3401,
        // Orthographic fragment shader uniform variables
        AP_SO_TEXTURE1 = 0x3801,
        AP_SO_COLOR,

        AP_SP_UNKNOWN = 0,
        AP_SO_UNKNOWN = 0
} AP_Shader_types;

static inline const char* ap_shader_name(AP_Shader_types i)
{
        switch (i)
        {
        // Perspective vertex shader uniform variables
        case AP_SP_MODEL:
                return "model";
        case AP_SP_VIEW:
                return "view";
        case AP_SP_PROJECTION:
                return "projection";

        // Perspective fragment shader uniform variables
        case AP_SP_MATERIALS:
                return "materials";
        case AP_SP_MT_DIFFUSE:
                return "materials.diffuse";
        case AP_SP_MT_SPECULAR:
                return "materials.specular";
        case AP_SP_MT_SHININESS:
                return "materials.shininess";

        case AP_SP_POINT_LIGHT:
                return "point_light";
        case AP_SP_PL_POSITION:
                return "point_light.position";
        case AP_SP_PL_CONSTANT:
                return "point_light.constant";
        case AP_SP_PL_LINEAR:
                return "point_light.linear";
        case AP_SP_PL_QUADRATIC:
                return "point_light.quadratic";
        case AP_SP_PL_AMBIENT:
                return "point_light.ambient";
        case AP_SP_PL_DIFFUSE:
                return "point_light.diffuse";
        case AP_SP_PL_SPECULAR:
                return "point_light.specular";

        case AP_SP_DIRECT_LIGHT:
                return "direct_light";
        case AP_SP_DL_DIRECTION:
                return "direct_light.direction";
        case AP_SP_DL_AMBIENT:
                return "direct_light.ambient";
        case AP_SP_DL_DIFFUSE:
                return "direct_light.diffuse";
        case AP_SP_DL_SPECULAR:
                return "direct_light.specular";

        case AP_SP_SPOT_LIGHT:
                return "spot_light";
        case AP_SP_SL_POSITION:
                return "spot_light.position";
        case AP_SP_SL_DIRECTION:
                return "spot_light.direction";
        case AP_SP_SL_CUTOFF:
                return "spot_light.cut_off";
        case AP_SP_SL_OUTER_CUTOFF:
                return "spot_light.outer_cut_off";
        case AP_SP_SL_CONSTANT:
                return "spot_light.constant";
        case AP_SP_SL_LINEAR:
                return "spot_light.linear";
        case AP_SP_SL_QUADRATIC:
                return "spot_light.quadratic";
        case AP_SP_SL_AMBIENT:
                return "spot_light.ambient";
        case AP_SP_SL_DIFFUSE:
                return "spot_light.diffuse";
        case AP_SP_SL_SPECULAR:
                return "spot_light.specular";

        case AP_SP_SPOT_LIGHT_ENABLED:
                return "spot_light_enabled";
        case AP_SP_MATERIAL_NUMBER:
                return "material_number";
        case AP_SP_VIEW_POS:
                return "view_pos";

        // Orthographic vertex shader uniform variables
        case AP_SO_PROJECTION:
                return "projection";
        // Orthographic fragment shader uniform variables
        case AP_SO_TEXTURE1:
                return "texture1";
        case AP_SO_COLOR:
                return "color";
        default:
                break;
        }
        return "";
}

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

void ap_shader_set_float(GLuint program, const char *const name, float num);

void ap_shader_set_int  (GLuint program, const char *const name, GLuint num);

void ap_shader_set_vec3 (GLuint program, const char *const name, float *vec);

void ap_shader_set_vec4 (GLuint program, const char *const name, float *vec);

void ap_shader_set_mat4 (GLuint program, const char *const name, float *mat);

unsigned int ap_get_current_shader();

#endif // AP_SHADER_H
