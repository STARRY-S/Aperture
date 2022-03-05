#include "camera.h"
#include "main.h"

void updateCameraVectors();

static struct Camera sCamera;

struct Camera* initCamera()
{
    // init camera with default values.
    setCameraPosition(0.0f, 0.0f, 10.0f);
    setCameraFront(0.0f, 0.0f, -1.0f);
    setCameraUp(0.0f, 1.0f, 0.0f);
    setCameraYaw(-90.0f);
    setCameraPitch(0.0f);
    setCameraSpeed(2.5f);
    setCameraSensitivity(0.05f);
    setCameraZoom(45.0f);

    return &sCamera;
}

void updateCameraVectors()
{
    float sin_yaw   = (float) sin((double) glm_rad(sCamera.yaw));
    float cos_yaw   = (float) cos((double) glm_rad(sCamera.yaw));
    float sin_pitch = (float) sin((double) glm_rad(sCamera.pitch));
    float cos_pitch = (float) cos((double) glm_rad(sCamera.pitch));

    sCamera.front[0] = cos_yaw * cos_pitch;
    sCamera.front[1] = sin_pitch;
    sCamera.front[2] = sin_yaw * cos_pitch;
    glm_vec3_normalize(sCamera.front);
}

struct Camera* getCamera()
{
    return &sCamera;
}

void GetViewMatrix(mat4 *temp)
{
    vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
    glm_vec3_add(sCamera.position, sCamera.front, cameraTarget);
    glm_lookat(sCamera.position, cameraTarget, sCamera.up, *temp);
}

void ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = sCamera.mSpeed * deltaTime;
    vec3 temp = {0.0f, 0.0f, 0.0f};
    switch (direction)
    {
        case CAM_MOV_FORWARD:
        {
            glm_vec3_scale(sCamera.front, velocity, temp);
            glm_vec3_add(sCamera.position, temp, sCamera.position);
            break;
        }
        case CAM_MOV_BACKWARD:
        {
            glm_vec3_scale(sCamera.front, velocity, temp);
            glm_vec3_sub(sCamera.position, temp, sCamera.position);
            break;
        }
        case CAM_MOV_LEFT:
        {
            glm_vec3_cross(sCamera.front, sCamera.up, temp);
            glm_vec3_normalize(temp);
            glm_vec3_scale(temp, velocity, temp);
            glm_vec3_sub(sCamera.position, temp, sCamera.position);
            break;
        }
        case CAM_MOV_RIGHT:
        {
            glm_vec3_cross(sCamera.front, sCamera.up, temp);
            glm_vec3_normalize(temp);
            glm_vec3_scale(temp, velocity, temp);
            glm_vec3_add(sCamera.position, temp, sCamera.position);
            break;
        }
        case CAM_MOV_UP:
        {
            glm_vec3_scale(sCamera.up, velocity, temp);
            glm_vec3_add(sCamera.position, temp, sCamera.position);
            break;
        }
        case CAM_MOV_DOWN:
        {
            glm_vec3_scale(sCamera.up, velocity, temp);
            glm_vec3_sub(sCamera.position, temp, sCamera.position);
            break;
        }
        default:
            break;
    }
}

void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= sCamera.mSensitivity;
    yOffset *= sCamera.mSensitivity;

    sCamera.yaw += xOffset;
    sCamera.pitch += yOffset;

    if (constrainPitch) {
        if(sCamera.pitch > 89.0f)
            sCamera.pitch =  89.0f;
        if(sCamera.pitch < -89.0f)
            sCamera.pitch = -89.0f;
    }

    updateCameraVectors();
}

void ProcessMouseScroll(float yOffset)
{
    sCamera.zoom -= yOffset;
    if (sCamera.zoom < 1.0f) {
        sCamera.zoom = 1.0f;
    } else if (sCamera.zoom > 45.0f) {
        sCamera.zoom = 45.0f;
    }
}

void ProcessMoveBlock(vec3 blockPos)
{
    // glm_vec3_distance(blockPos);
}

int setCameraPosition(float x, float y, float z)
{
    sCamera.position[0] = x;
    sCamera.position[1] = y;
    sCamera.position[2] = z;
    return 0;
}

int setCameraFront(float x, float y, float z)
{
    sCamera.front[0] = x;
    sCamera.front[1] = y;
    sCamera.front[2] = z;
    return 0;
}

int setCameraUp(float x, float y, float z)
{
    sCamera.up[0] = x;
    sCamera.up[1] = y;
    sCamera.up[2] = z;
    return 0;
}

int setCameraYaw(float yaw)
{
    sCamera.yaw = yaw;
    return 0;
}

int setCameraPitch(float pitch)
{
    sCamera.pitch = pitch;
    return 0;
}

int setCameraSensitivity(float sensitivity)
{
    sCamera.mSensitivity = sensitivity;
    return 0;
}

int setCameraSpeed(float speed)
{
    sCamera.mSpeed = speed;
    return 0;
}

int setCameraZoom(float zoom)
{
    sCamera.zoom = zoom;
    return 0;
}
