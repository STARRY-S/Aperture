#ifndef __ANDROID__
#include "glad/glad.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ap_cvector.h"
#include "ap_shader.h"
#include "ap_utils.h"

// vector stores openGL (shader) program ID
static struct AP_Vector shader_vector = { 0, 0, 0, 0 };
// openGL (shader) program ID
static unsigned int shader_using = 0;

/**
 * Load vertex and fragment shader from file, compile and attach it to program
 * return (shader) program id
 */
GLuint ap_shader_load_program(
    const char *const vshader_path,
    const char *const fshader_path
);

/**
 * @brief return compiled openGL shader id
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
                ap_vector_init(&shader_vector, AP_VECTOR_UINT);
        }

        GLuint gl_shader_id = 0;
        gl_shader_id = ap_shader_load_program(vshader_path, fshader_path);
        if (gl_shader_id == 0) {
                return AP_ERROR_SHADER_LOAD_FAILED;
        }
        ap_vector_push_back(&shader_vector, (const char*) &gl_shader_id);
        *shader_id = gl_shader_id;

        return 0;
}

int ap_shader_use(unsigned shader_program_id)
{
        if (shader_program_id == 0) {
                glUseProgram(0);
                return 0;
        }

        unsigned int *program_id_array = (unsigned*) shader_vector.data;
        bool found = false;
        for (int i = 0; i < shader_vector.length; ++i) {
                if (program_id_array[i] == shader_program_id) {
                        found = true;
                }
        }

        if (found) {
                shader_using = shader_program_id;
                glUseProgram(shader_program_id);
        } else {
                return AP_ERROR_INVALID_PARAMETER;
        }

        return 0;
}

int ap_shader_free()
{
        shader_using = 0;    // for safety purpose
        glUseProgram(0);
        unsigned int *program_ptr = (unsigned*) shader_vector.data;
        for (int i = 0; i < shader_vector.length; ++i) {
                glDeleteProgram(program_ptr[i]);
                LOGD("deleted OpenGL program: %u", program_ptr[i]);
        }
        ap_vector_free(&shader_vector);
        LOGD("free shader programs");

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
                        LOGE("Malloc error.");
                        glDeleteShader(shader);
                        return 0;
                }
                glGetShaderInfoLog(shader, info_len, NULL, info);
                LOGE("Compiled Error: %s", info);
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

        #ifdef __ANDROID__
        AAssetManager *pLocalAssetManager =
                (AAssetManager *) ap_get_asset_manager();
        if (!pLocalAssetManager) {
                LOGE("pLocalAssetManager is NULL, failed to read file.");
                return 0;
        }
        AAsset *mAsset = NULL;
        mAsset = AAssetManager_open(
                pLocalAssetManager,
                shader_path,
                AASSET_MODE_UNKNOWN
        );
        if (mAsset == NULL) {
                LOGE("Read Text Failed: %s", shader_path);
                return 0;
        }
        length = AAsset_getLength(mAsset);
        buffer = AP_MALLOC(sizeof(char) * length);
        if (buffer == NULL) {
                LOGE("MALLOG FAILED.");
                return 0;
        }
        AAsset_read(mAsset, buffer, length);
        AAsset_close(mAsset);

        #else   // NOT ANDROID

        FILE *fp = NULL;

        if (!(fp = fopen(shader_path, "r"))) {
                LOGE("Open file %s failed", shader_path);
                return 1;
        }
        // sets the file position to end of file
        fseek(fp, 0l, SEEK_END);
        length = ftell(fp);
        rewind(fp);
        if (!(buffer = (char*) AP_MALLOC(length))) {
                LOGE("Malloc Error");
                fclose(fp);
                return 1;
        }
        buffer[0] = '\0';

        char temp_line[AP_DEFAULT_BUFFER_SIZE];

        while (fgets(temp_line, AP_DEFAULT_BUFFER_SIZE, fp))
                strncat(buffer, temp_line, AP_DEFAULT_BUFFER_SIZE);
        fclose(fp);

        #endif  // NOT ANDROID

        buffer[length] = '\0';
        result = ap_compile_shader(type, buffer);
        if (result == 0) {
                LOGE("Shader file [%s] compiled failed.", shader_path);
        }
        AP_FREE(buffer);
        return result;
}

GLuint ap_shader_load_program(
        const char *const vshader_path,
        const char *const fshader_path)
{
        GLint linked = 0;
        GLuint vshader = 0;
        GLuint fshader = 0;
        GLuint program = 0;

        vshader = ap_shader_load(GL_VERTEX_SHADER, vshader_path);
        fshader = ap_shader_load(GL_FRAGMENT_SHADER, fshader_path);
        if (!vshader || !fshader)
                return 0;
        if (!(program = glCreateProgram())) {
                LOGE("glCreateProgram failed.");
                return 0;
        }

        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);
        glDeleteShader(vshader);
        glDeleteShader(fshader);

        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
                LOGE("Link Program Error.");
                return 0;
        }
        return program;
}

void ap_shader_set_float(GLuint program, const char *const name, float value)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniform1f(location, value);
}

void ap_shader_set_int(GLuint program, const char *const name, GLuint value)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniform1i(location, value);
}

void ap_shader_set_vec3(GLuint program, const char *const name, float *vec)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniform3fv(location, 1, vec);
}

void ap_shader_set_vec4(GLuint program, const char *const name, float *vec)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniform4fv(location, 1, vec);
}

void ap_shader_set_mat4(GLuint program, const char *const name, float* mat)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

unsigned int ap_get_current_shader()
{
        return shader_using;
}
