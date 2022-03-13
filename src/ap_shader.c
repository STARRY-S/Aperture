#include <GLES3/gl3.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ap_shader.h"
#include "ap_utils.h"

GLuint ap_shader_load(GLenum type, const char *const shader_path);

GLuint ap_compile_shader(
    GLenum type,
    const char *const shader_src)
{
        GLuint shader = 0;
        GLint compiled = 0;

        if (!(shader = glCreateShader(type))) {
                LOGE("glCreateShader failed, type %d.\n", type);
                return 0;
        }

        glShaderSource(shader, 1, &shader_src, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
                GLint info_len = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
                char *info = (char*) malloc(info_len);
                if (info == NULL) {
                        LOGE("Malloc error.\n");
                        glDeleteShader(shader);
                        return 0;
                }
                glGetShaderInfoLog(shader, info_len, NULL, info);
                LOGE("Compiled Error: \n%s\n", info);
                free(info);
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
                LOGE("pLocalAssetManager is NULL, failed to read file.\n");
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
        buffer = malloc(sizeof(char) * length);
        if (buffer == NULL) {
                LOGE("MALLOG FAILED.\n");
                return 0;
        }
        AAsset_read(mAsset, buffer, length);
        AAsset_close(mAsset);

        #else   // NOT ANDROID

        FILE *fp = NULL;

        if (!(fp = fopen(shader_path, "r"))) {
                fprintf(stderr, "Open file %s failed.\n", shader_path);
                return 1;
        }
        // sets the file position to end of file
        fseek(fp, 0l, SEEK_END);
        length = ftell(fp);
        rewind(fp);
        if (!(buffer = (char*) malloc(length))) {
                fprintf(stderr, "Malloc Error.\n");
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
                LOGE("Shader file [%s] compiled failed.\n", shader_path);
        }
        free(buffer);
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
                LOGE("glCreateProgram failed.\n");
                return 0;
        }

        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);
        glDeleteShader(vshader);
        glDeleteShader(fshader);

        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
                LOGE("Link Program Error.\n");
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

void ap_shader_set_mat4(GLuint program, const char *const name, float* mat)
{
        GLuint location = glGetUniformLocation(program, name);
        glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}
