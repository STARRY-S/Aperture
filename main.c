#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <assimp/cfileio.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"

#ifndef __ANDROID__
void key_callback(GLFWwindow *window, int key, int s, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_cursor_callback( GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
const GLFWvidmode* mode;
#endif

#define GE_DEFAULT_BUFFER_SIZE 128

// Global error number
int GE_errorno = 0;

static AAssetManager *pLocalAAsetManager = NULL;
static char sMobileName[GE_DEFAULT_BUFFER_SIZE] = { 0 };

void *getLocalAAssetManager()
{
    return (void*) pLocalAAsetManager;
}

int setAAssetManager(void *pVoid)
{
    if (!pVoid) {
        LOGE("setAAssetManager ERROR: NULL");
        pLocalAAsetManager = NULL;
        return GE_ERROR_INVALID_POINTER;
    }

    struct AAssetManager *pManager = (struct AAssetManager*) pVoid;
    if (pLocalAAsetManager == NULL) {
        pLocalAAsetManager = pManager;
    } else {
        LOGI("Replace Local AAsetManager Pointer");
        pLocalAAsetManager = pManager;
    }
    return 0;
}

int setMobileName(const char *pName)
{
    if (pName == NULL) {
        LOGE("setMobileName ERROR: NULL");
        return GE_ERROR_INVALID_POINTER;
    }

    strncpy(sMobileName, pName, GE_DEFAULT_BUFFER_SIZE);
    sMobileName[GE_DEFAULT_BUFFER_SIZE - 1] = '0';

    return 0;
}

const char* getMobileName()
{
    return sMobileName;
}

int getMobileType(const char *pMobileName)
{
    if (pMobileName == NULL) {
        LOGE("getMobileType Failed: NULL");
        return 0;
    }
    for (int i = 0; i < GE_MOBILE_LENGTH; ++i) {
        if (strstr(pMobileName, GE_MOBILE_NAME[i]) != NULL) {
            return i;
        }
    }
    return 0;
}

#ifndef __ANDROID__
void key_callback(GLFWwindow *window, int key, int s, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        textureNum += 1;
        textureNum %= 10;
    }
}
void processInput(GLFWwindow *window)
{
    float speed = deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        speed *= 2.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_FORWARD, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_BACKWARD, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_LEFT, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_RIGHT, speed);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_DOWN, speed);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ProcessKeyboard(CAM_MOV_UP, speed);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 这里是相反的，因为y坐标从底部往顶部依次增大
    lastX = xpos;
    lastY = ypos;

    ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
#endif
