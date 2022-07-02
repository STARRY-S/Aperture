

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ap_cvector.h"
#include "ap_shader.h"
#include "ap_custom_io.h"
#include "ap_utils.h"

static struct AP_Vector shader_vector = { 0, 0, 0, 0 };
static struct AP_Shader *shader_using = NULL;

/**
 * Load vertex and fragment shader from file, compile and attach it to program
 * return (shader) program id
 */
GLuint ap_shader_load_program(
        const char *const vshader_path,
        const char *const fshader_path
);

/**
 * @brief return compiled OpenGL shader id
 * @param type
 * @param shader_path
 * @return GLuint
 */
GLuint ap_shader_load(GLenum type, const char *const shader_path);

/**
 * @brief Compile the shader from memory data
 * @param type shader type
 * @param shader_src memory data
 * @return GLuint shader id
 */
GLuint ap_compile_shader(
        GLenum type,
        const char *const shader_src
);

int ap_shader_generate(
        const char* vshader_path,
        const char* fshader_path,
        unsigned int *shader_id)
{
        if (!vshader_path || !fshader_path) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        // initialize vector when first use
        if (shader_vector.data == NULL) {
                ap_vector_init(&shader_vector, AP_VECTOR_SHADER);
        }

        GLuint gl_shader_id = 0;
        gl_shader_id = ap_shader_load_program(vshader_path, fshader_path);
        if (gl_shader_id == 0) {
                return AP_ERROR_SHADER_LOAD_FAILED;
        }
        struct AP_Shader shader;
        memset(&shader, 0, sizeof(struct AP_Shader));
        shader.id = shader_vector.length + 1;
        shader.opengl_program_id = gl_shader_id;
        shader.fragment_shader_path = ap_char_copy(fshader_path);
        shader.vertex_shader_path   = ap_char_copy(vshader_path);
        ap_vector_push_back(&shader_vector, (const char*) &shader);
        *shader_id = shader.id;

        return 0;
}

int ap_shader_use(unsigned id)
{
        if (id == 0) {
                shader_using = NULL;
                glUseProgram(0);
                return 0;
        }

        struct AP_Shader *data = (struct AP_Shader*) shader_vector.data;
        for (int i = 0; i < shader_vector.length; ++i) {
                if (data[i].id != id) {
                        continue;
                }
                glUseProgram(data[i].opengl_program_id);
                shader_using = data + i;
                return 0;
        }

        LOGE("ap_shader_use failed: id %u not found", id);
        return AP_ERROR_INVALID_PARAMETER;
}

int ap_shader_free(int id)
{
        if (id <= 0) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Shader *ptr = ap_shader_get_ptr(id);
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (ptr == shader_using) {
                shader_using = NULL;
                glUseProgram(0);
        }

        glDeleteProgram(ptr->opengl_program_id);
        AP_FREE(ptr->fragment_shader_path);
        AP_FREE(ptr->vertex_shader_path);
        ap_vector_remove_data(
                &shader_vector,
                (char*) (ptr),
                (char*) (ptr + 1),
                sizeof(struct AP_Shader)
        );

        return 0;
}

int ap_shader_free_all()
{
        shader_using = NULL;
        glUseProgram(0);

        struct AP_Shader *shader = (struct AP_Shader*) shader_vector.data;
        for (int i = 0; i < shader_vector.length; ++i) {
                glDeleteProgram(shader[i].opengl_program_id);
                AP_FREE(shader[i].fragment_shader_path);
                AP_FREE(shader[i].vertex_shader_path);
                shader[i].fragment_shader_path = NULL;
                shader[i].vertex_shader_path = NULL;
        }
        ap_vector_free(&shader_vector);

        return 0;
}

GLuint ap_compile_shader(
        GLenum type,
        const char *const shader_src)
{
        GLuint shader = 0;
        GLint compiled = 0;

        if (!(shader = glCreateShader(type))) {
                LOGE("glCreateShader failed, type %d.", type);
                return 0;
        }

        glShaderSource(shader, 1, &shader_src, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
                GLint info_len = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
                char *info = (char*) AP_MALLOC(info_len);
                if (info == NULL) {
                        LOGE("ap_compile_shader: malloc error");
                        glDeleteShader(shader);
                        return 0;
                }
                glGetShaderInfoLog(shader, info_len, NULL, info);
                LOGE("ap_compile_shader failed: \n%s", info);
                AP_FREE(info);
                return 0;
        }
        return shader;
}

GLuint ap_shader_load(GLenum type, const char *const shader_path)
{
        GLuint result = 0;
        int length = 0;
        char *buffer = NULL;

        int ret = ap_read_file_to_memory(shader_path, &buffer, &length);
        if (!buffer || !length) {
                AP_CHECK(ret);
                return 0;
        }
        buffer[length] = '\0';
        result = ap_compile_shader(type, buffer);
        if (result == 0) {
                LOGE("shader file [%s] compiled failed", shader_path);
        }
        AP_FREE(buffer);
        buffer = NULL;
        return result;
}

GLuint ap_shader_load_program(
        const char *const vshader_path,
        const char *const fshader_path)
{
        GLuint vshader = ap_shader_load(GL_VERTEX_SHADER, vshader_path);
        GLuint fshader = ap_shader_load(GL_FRAGMENT_SHADER, fshader_path);
        if (!vshader || !fshader) {
                return 0;
        }
        GLuint program = 0;
        if (!(program = glCreateProgram())) {
                LOGE("glCreateProgram failed.");
                return 0;
        }

        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);
        glDeleteShader(vshader);
        glDeleteShader(fshader);

        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
                GLint log_length = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

                GLchar *buffer = AP_MALLOC(sizeof(GLchar) * (log_length + 1));
                glGetProgramInfoLog(program, log_length, &log_length, buffer);
                LOGE("ap_shader_load_program: glLinkProgram\n%s", buffer);
                glDeleteProgram(program);
                return 0;
        }
        return program;
}

int ap_shader_set_float(const char *const name, float value)
{
        if (!shader_using) {
                LOGW("ap_shader_set_float failed: shader_using == NULL");
                return 0;
        }
        int location = glGetUniformLocation(
                shader_using->opengl_program_id, name);
        if (location < 0) {
                LOGD("shader failed to set float for %s %d", name, location);
                return AP_ERROR_RENDER_FAILED;
        }
        glUniform1f(location, value);
        return 0;
}

int ap_shader_set_int(const char *const name, GLuint value)
{
        if (!shader_using) {
                LOGW("ap_shader_set_int failed: shader_using == NULL");
                return 0;
        }
        int location = glGetUniformLocation(
                shader_using->opengl_program_id, name);
        if (location < 0) {
                LOGD("shader failed to set int for %s", name);
                return AP_ERROR_RENDER_FAILED;
        }
        glUniform1i(location, value);
        return 0;
}

int ap_shader_set_vec3(const char *const name, float *vec)
{
        if (!shader_using) {
                LOGW("ap_shader_set_vec3 failed: shader_using == NULL");
                return 0;
        }
        int location = glGetUniformLocation(
                shader_using->opengl_program_id, name);
        if (location < 0) {
                LOGD("shader failed to set vec3 for %s", name);
                return AP_ERROR_RENDER_FAILED;
        }
        glUniform3fv(location, 1, vec);
        return 0;
}

int ap_shader_set_vec4(const char *const name, float *vec)
{
        if (!shader_using) {
                LOGW("ap_shader_set_vec4 failed: shader_using == NULL");
                return 0;
        }
        int location = glGetUniformLocation(
                shader_using->opengl_program_id, name);
        if (location < 0) {
                LOGD("shader failed to set vec4 for %s", name);
                return AP_ERROR_RENDER_FAILED;
        }
        glUniform4fv(location, 1, vec);
        return 0;
}

int ap_shader_set_mat4(const char *const name, float* mat)
{
        if (!shader_using) {
                LOGW("ap_shader_set_mat4 failed: shader_using == NULL");
                return 0;
        }
        int location = glGetUniformLocation(
                shader_using->opengl_program_id, name);
        if (location < 0) {
                LOGD("shader failed to set mat4 for %s", name);
                return AP_ERROR_RENDER_FAILED;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, mat);
        return 0;
}

unsigned int ap_get_current_shader()
{
        if (!shader_using) {
                return 0;
        }
        return shader_using->id;
}

struct AP_Shader* ap_shader_get_ptr(int id)
{
        if (id <= 0) {
                return NULL;
        }

        struct AP_Shader *data = (struct AP_Shader*) shader_vector.data;
        for (int i = 0; i < shader_vector.length; ++i) {
                if (data[i].id != id) {
                        continue;
                }
                return data + i;
        }
        return NULL;
}
