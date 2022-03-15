#ifndef AP_SHADER_H
#define AP_SHADER_H

#include <GLES3/gl3.h>
#include <stdio.h>

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

void ap_shader_set_mat4 (GLuint program, const char *const name, float *mat);

unsigned int ap_get_current_shader();

#endif // AP_SHADER_H
