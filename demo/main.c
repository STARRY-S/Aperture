#include <stdio.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "ge_utils.h"
#include "renderer.h"
#include "camera.h"

void key_callback(
    GLFWwindow *window, int key, int s, int action, int mods
);
void framebuffer_size_callback(
    GLFWwindow* window, int width, int height
);
void mouse_callback(
    GLFWwindow* window, double xpos, double ypos
);
void mouse_cursor_callback(
    GLFWwindow * window, double xpos, double ypos
);
void scroll_callback(
    GLFWwindow* window, double xoffset, double yoffset
);
void processInput(GLFWwindow *window);

const GLFWvidmode* mode;

float deltaTime = 0.0f; // delta time between last frame time
float lastFrame = 0.0f; // last frame time

float lastX = 400, lastY = 300;
bool firstMouse = true;
bool fullScreenMode = false;

int main(int argc, char **argv)
{
    if (argc == 2) {
        if (!strcmp(argv[1], "-h")) {
            printf("Usage: \n");
            printf("\t%s -f -- Full Screen Mode\n", argv[0]);
            printf("\tW S A D -- Camea movement\n");
            printf("\tLCtrl   -- Speedup movement\n");
            printf("\tESC / Q -- Exit\n");

            return EXIT_SUCCESS;
        }

        if (!strcmp(argv[1], "-f")) {
            fullScreenMode = true;
        }
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);


    GLFWwindow* window = NULL;
    if (fullScreenMode) {
        window = glfwCreateWindow(mode->width, mode->height,
            WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
    } else {
        window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
            WINDOW_TITLE, NULL, NULL);
    }

    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, (1920 - SCREEN_WIDTH) / 2,
                (1080 - SCREEN_HEIGHT) / 2 );
    glfwMakeContextCurrent(window);

    if (fullScreenMode)
        resizeBuffer(mode->width, mode->height);
    else
        resizeBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup GameEngine
    setup();

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // GameEngine main renderer
        render();

        // refresh
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    finish();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int s, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    //    textureNum += 1;
    //    textureNum %= 10;
    // }
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
    // it's oppisite (negative) here,
    // because y increases from bottom to top
    float yoffset = -(ypos - lastY);
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
    // glViewport(0, 0, width, height);
    resizeBuffer(width, height);
}