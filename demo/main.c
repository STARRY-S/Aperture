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
    GLFWwindow *window, int width, int height
);
void mouse_callback(
    GLFWwindow *window, double xpos, double ypos
);
void mouse_cursor_callback(
    GLFWwindow *window, double xpos, double ypos
);
void mouse_button_callback(
    GLFWwindow *window, int button, int action, int mods
);
void scroll_callback(
    GLFWwindow *window, double xoffset, double yoffset
);
void processInput(GLFWwindow *window);

const GLFWvidmode* mode;

float deltaTime = 0.0f; // delta time between last frame time
float lastFrame = 0.0f; // last frame time

float last_x = 400, last_y = 300;
bool first_mouse = true;
bool fullScreenMode = false;
bool left_button_pressed = false;

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
        ap_resize_screen_buffer(mode->width, mode->height);
    else
        ap_resize_screen_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetKeyCallback(window, key_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup GameEngine
    ap_render_general_initialize();

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // GameEngine main renderer
        ap_render_main();

        // refresh
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ap_render_finish();
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
        ap_camera_process_key(AP_CAMERA_FORWARD, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ap_camera_process_key(AP_CAMERA_MOV_BACKWARD, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ap_camera_process_key(AP_CAMERA_MOV_LEFT, speed);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ap_camera_process_key(AP_CAMERA_MOV_RIGHT, speed);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        ap_camera_process_key(AP_CAMERA_MOV_DOWN, speed);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        ap_camera_process_key(AP_CAMERA_MOV_UP, speed);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    // Don't rotate viewport when mouse left button isn't pressed
    if (!left_button_pressed) {
        if (!first_mouse) {
            first_mouse = true;
        }
        return;
    }

    if(first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_x;
    // it's oppisite (negative) here,
    // because y increases from bottom to top
    float yoffset = -(ypos - last_y);
    last_x = xpos;
    last_y = ypos;

    ap_camera_process_movement(xoffset, yoffset, true);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            left_button_pressed = true;
        } else if (action == GLFW_RELEASE) {
            left_button_pressed = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ap_camera_process_scroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    ap_resize_screen_buffer(width, height);
}