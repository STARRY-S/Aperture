#ifndef AP_SHADER_H
#define AP_SHADER_H

#include <GLES3/gl3.h>
#include <stdio.h>

GLuint ap_compile_shader(
    GLenum type,
    const char *const shader_src
);

/**
 * Load shader from file, then compile and attach it to program
 * return program id
 */
GLuint ap_shader_load_program(
    const char *const vshader_path,
    const char *const fshader_path
);

void ap_shader_set_float(GLuint program, const char *const name, float num);
void ap_shader_set_int  (GLuint program, const char *const name, GLuint num);
void ap_shader_set_vec3 (GLuint program, const char *const name, float *vec);
void ap_shader_set_mat4 (GLuint program, const char *const name, float *mat);

#endif // AP_SHADER_H
