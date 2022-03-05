#ifndef GAME_ENGINE_CAMERA_H
#define GAME_ENGINE_CAMERA_H

#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdbool.h>

#include "cglm/cglm.h"
#include "cglm/call.h"

enum Camera_Movement {
    CAM_MOV_FORWARD,
    CAM_MOV_BACKWARD,
    CAM_MOV_LEFT,
    CAM_MOV_RIGHT,
    CAM_MOV_UP,
    CAM_MOV_DOWN
};

struct Camera {
    // camera Attributes
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float mSpeed;           // movement speed
    float mSensitivity;     // mouse sensitivity
    float zoom;
};

struct Camera* initCamera();
struct Camera* getCamera();

void GetViewMatrix(mat4 *temp);
void ProcessKeyboard(int direction, float deltaTime);
void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch);
void ProcessMouseScroll(float yOffset);

int setCameraPosition(float x, float y, float z);
int setCameraFront(float x, float y, float z);
int setCameraUp(float x, float y, float z);
int setCameraYaw(float yaw);
int setCameraPitch(float pitch);
int setCameraSensitivity(float sensitivity);
int setCameraSpeed(float speed);
int setCameraZoom(float zoom);

#endif //GAME_ENGINE_CAMERA_H
