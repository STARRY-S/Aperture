#include <GLES3/gl3.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shader.h"
#include "ge_utils.h"

// Compile shader
GLuint make_shader(GLenum type, const char *const shader_src)
{
    GLuint shader = 0;
    GLint compiled = 0;

    if (!(shader = glCreateShader(type))) {
        LOGE("Shader Create Error, type %d.\n", type);
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
        LOGE("Shader Compile Error: \n%s\n", info);
        free(info);
        return 0;
    }
    return shader;
}

// Load shader in Android
GLuint load_shader(GLenum type, const char *const shader_path)
{
    GLuint result = 0;
    int length = 0;
    char *pBuffer = NULL;

    #ifdef __ANDROID__
    AAssetManager *pLocalAAsetManager
        = (AAssetManager *) getLocalAAssetManager();
    if (!pLocalAAsetManager) {
        LOGE("pLocalAAsetManager is NULL, failed to read file.\n");
        return 0;
    }
    AAsset *mAsset = NULL;
    mAsset = AAssetManager_open(pLocalAAsetManager, shader_path, AASSET_MODE_UNKNOWN);
    if (mAsset == NULL) {
        LOGE("Read Text Failed: %s", shader_path);
        return 0;
    }
    length = AAsset_getLength(mAsset);
    if (length > 1024 * 1024 * 10) {    // 10MB
        LOGE("File too large.");
        return 0;
    }
    pBuffer = malloc(sizeof(char) * length);
    if (pBuffer == NULL) {
        LOGE("MALLOG FAILED.\n");
        return 0;
    }
    AAsset_read(mAsset, pBuffer, length);
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
        if (!(pBuffer = (char*) malloc(length))) {
            fprintf(stderr, "Malloc Error.\n");
            fclose(fp);
            return 1;
        }
        pBuffer[0] = '\0';

        char temp_line[GE_DEFAULT_BUFFER_SIZE];

        while (fgets(temp_line, GE_DEFAULT_BUFFER_SIZE, fp))
        strncat(pBuffer, temp_line, GE_DEFAULT_BUFFER_SIZE);
        fclose(fp);

    #endif  // NOT ANDROID

    pBuffer[length] = '\0';
    result = make_shader(type, pBuffer);
    free(pBuffer);
    return result;
}

// Load shader from file, then compile and attach it to program
// return program id
GLuint load_program(const char *const vshader_path,
                    const char *const fshader_path)
{
    GLint linked = 0;
    GLuint vshader = 0;
    GLuint fshader = 0;
    GLuint program = 0;

    vshader = load_shader(GL_VERTEX_SHADER, vshader_path);
    fshader = load_shader(GL_FRAGMENT_SHADER, fshader_path);
    if (!vshader || !fshader)
        return 0;
    if (!(program = glCreateProgram())) {
        LOGE("Program create error.\n");
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

void shaderSetFloat(GLuint program, const char *const name, float value)
{
    GLuint location = glGetUniformLocation(program, name);
    glUniform1f(location, value);
}

void shaderSetInt(GLuint program, const char *const name, GLuint value)
{
    GLuint location = glGetUniformLocation(program, name);
    glUniform1i(location, value);
}

void shaderSetVec3(GLuint program, const char *const name, float *vec)
{
    GLuint location = glGetUniformLocation(program, name);
    glUniform3fv(location, 1, vec);
}

void shaderSetMat4(GLuint program, const char *const name, float* mat)
{
    GLuint location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}
