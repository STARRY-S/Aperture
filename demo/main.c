#include <stdio.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "ap_utils.h"
#include "ap_render.h"
#include "ap_camera.h"

#define AP_DEMO_CAMERA_NUMBER 5

void key_callback(
        GLFWwindow *window, int key, int s, int action, int mods
);
void framebuffer_size_callback(
        GLFWwindow *window, int width, int height
);
void mouse_callback(
        GLFWwindow *window, double x_pos, double y_pos
);
void mouse_cursor_callback(
        GLFWwindow *window, double x_pos, double y_pos
);
void mouse_button_callback(
        GLFWwindow *window, int button, int action, int mods
);
void scroll_callback(
        GLFWwindow *window, double x_offset, double y_offset
);
void processInput(GLFWwindow *window);

const GLFWvidmode* mode;

float delta_time = 0.0f; // delta time between last frame time
float last_frame_time = 0.0f; // last frame time

float last_x = 400, last_y = 300;
bool first_mouse = true;
bool full_screen_mode = false;
bool left_button_pressed = false;

GLuint camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
int camera_use_id = 0;

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
                        full_screen_mode = true;
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
        if (full_screen_mode) {
                window = glfwCreateWindow(mode->width, mode->height,
                        WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
        } else {
                window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                        WINDOW_TITLE, NULL, NULL);
        }

        if (window == NULL)
        {
                LOGE("Failed to create GLFW window.\n");
                glfwTerminate();
                return -1;
        }

        glfwSetWindowPos(window, (1920 - SCREEN_WIDTH) / 2,
                        (1080 - SCREEN_HEIGHT) / 2 );
        glfwMakeContextCurrent(window);

        if (full_screen_mode)
                ap_resize_screen_buffer(mode->width, mode->height);
        else
                ap_resize_screen_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, key_callback);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Setup GameEngine
        ap_render_general_initialize();
        // camera
        for (int i = 0; i < AP_DEMO_CAMERA_NUMBER; ++i) {
                ap_camera_generate(&camera_ids[i]);
                ap_camera_use(camera_ids[i]);
                ap_camera_set_position(0.0f, (float) i, 10.0f);
        }

        ap_camera_use(camera_ids[camera_use_id]);

        while (!glfwWindowShouldClose(window))
        {
                // input
                processInput(window);

                // time
                float current_frame_time = glfwGetTime();
                delta_time = current_frame_time - last_frame_time;
                last_frame_time = current_frame_time;

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

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
                if (camera_use_id >= 0
                    && camera_use_id < AP_DEMO_CAMERA_NUMBER - 1) {
                        ap_camera_use(camera_ids[++camera_use_id]);
                } else {
                        camera_use_id = 0;
                        ap_camera_use(camera_ids[camera_use_id]);
                }
                LOGI("Use camera: %u\n", camera_ids[camera_use_id]);
        }
}
void processInput(GLFWwindow *window)
{
        float speed = delta_time;
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

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
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
                last_x = x_pos;
                last_y = y_pos;
                first_mouse = false;
        }

        float x_offset = x_pos - last_x;
        // it's oppisite (negative) here,
        // because y increases from bottom to top
        float yoffset = -(y_pos - last_y);
        last_x = x_pos;
        last_y = y_pos;

        ap_camera_process_movement(x_offset, yoffset, true);
}

void mouse_button_callback(
        GLFWwindow *window,
        int button,
        int action,
        int mods)
{
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (action == GLFW_PRESS) {
                        left_button_pressed = true;
                } else if (action == GLFW_RELEASE) {
                        left_button_pressed = false;
                }
        }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
        ap_camera_process_scroll(y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
        ap_resize_screen_buffer(width, height);
}