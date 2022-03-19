#include "ap_camera.h"
#include "ap_cvector.h"
#include "ap_utils.h"

static struct AP_Vector camera_vector = { 0, 0, 0, 0 };
static struct AP_Camera *camera_using = NULL;

int ap_camera_init_ptr(struct AP_Camera *camera);
int ap_camera_update_vectors();

int ap_camera_generate(unsigned int *camera_id)
{
        // initialize camera vector when first use
        if (camera_vector.data == NULL) {
                ap_vector_init(&camera_vector, AP_VECTOR_CAMERA);
        }

        struct AP_Camera camera;
        ap_camera_init_ptr(&camera);
        camera.id = camera_vector.length + 1;
        ap_vector_push_back(&camera_vector, (const char*) &camera);
        *camera_id = camera.id;

        return AP_ERROR_SUCCESS;
}

int ap_camera_use(unsigned int camera_id)
{
        if (camera_id > camera_vector.length) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (camera_id == 0) {
                camera_using = NULL;
                return 0;
        }

        struct AP_Camera *tmp_cam = (struct AP_Camera*) camera_vector.data;
        camera_using = tmp_cam + (camera_id - 1);

        return 0;
}

int ap_camera_init_default()
{
        return ap_camera_init_ptr(camera_using);
}

int ap_camera_init_ptr(struct AP_Camera *camera)
{
        if (camera == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }
        // init camera with default values.
        camera->position[0] = 0.0f;
        camera->position[1] = 0.0f;
        camera->position[2] = 0.0f;

        camera->front[0] = 0.0f;
        camera->front[1] = 0.0f;
        camera->front[2] = -1.0f;

        camera->up[0] = 0.0f;
        camera->up[1] = 1.0f;
        camera->up[2] = 0.0f;

        camera->yaw = -90.0f;
        camera->pitch = 0.0f;
        camera->speed = 2.5f;
        camera->sensitivity = 0.05f;
        camera->zoom = 45.0f;

        return 0;
}

int ap_camera_free()
{
        camera_using = NULL;    // for safety purpose
        ap_vector_free(&camera_vector);
        LOGD("ap_free cameras");

        return 0;
}

int ap_camera_update_vectors()
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        float sin_yaw   = (float) sin((double) glm_rad(camera_using->yaw));
        float cos_yaw   = (float) cos((double) glm_rad(camera_using->yaw));
        float sin_pitch = (float) sin((double) glm_rad(camera_using->pitch));
        float cos_pitch = (float) cos((double) glm_rad(camera_using->pitch));

        camera_using->front[0] = cos_yaw * cos_pitch;
        camera_using->front[1] = sin_pitch;
        camera_using->front[2] = sin_yaw * cos_pitch;
        glm_vec3_normalize(camera_using->front);
        return 0;
}

int ap_camera_get_view_matrix(mat4 *temp)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        vec3 cameraTarget = { 0.0f, 0.0f, 0.0f };
        glm_vec3_add(
                camera_using->position,
                camera_using->front,
                cameraTarget
        );
        glm_lookat(
                camera_using->position,
                cameraTarget,
                camera_using->up,
                *temp
        );
        return 0;
}

int ap_camera_get_position(float *vec)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        if (vec == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        vec[0] = camera_using->position[0];
        vec[1] = camera_using->position[1];
        vec[2] = camera_using->position[2];
        return 0;
}

int ap_camera_get_zoom(int *zoom)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        if (zoom == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        *zoom = camera_using->zoom;

        return 0;
}

int ap_camera_process_movement(int direction, float delta_time)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        float velocity = camera_using->speed * delta_time;
        vec3 temp = { 0.0f, 0.0f, 0.0f };
        switch (direction)
        {
        case AP_CAMERA_FORWARD:
        {
                glm_vec3_scale(camera_using->front, velocity, temp);
                glm_vec3_add(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_CAMERA_MOV_BACKWARD:
        {
                glm_vec3_scale(camera_using->front, velocity, temp);
                glm_vec3_sub(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_CAMERA_MOV_LEFT:
        {
                glm_vec3_cross(camera_using->front, camera_using->up, temp);
                glm_vec3_normalize(temp);
                glm_vec3_scale(temp, velocity, temp);
                glm_vec3_sub(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_CAMERA_MOV_RIGHT:
        {
                glm_vec3_cross(camera_using->front, camera_using->up, temp);
                glm_vec3_normalize(temp);
                glm_vec3_scale(temp, velocity, temp);
                glm_vec3_add(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_CAMERA_MOV_UP:
        {
                glm_vec3_scale(camera_using->up, velocity, temp);
                glm_vec3_add(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_CAMERA_MOV_DOWN:
        {
                glm_vec3_scale(camera_using->up, velocity, temp);
                glm_vec3_sub(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
                default:
                break;
        }
        return 0;
}

int ap_camera_process_mouse_move(float x_offset, float y_offset, bool pitch)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }
        x_offset *= camera_using->sensitivity;
        y_offset *= camera_using->sensitivity;

        camera_using->yaw += x_offset;
        camera_using->pitch += y_offset;

        if (pitch) {
                if(camera_using->pitch > 89.0f) {
                        camera_using->pitch =  89.0f;
                }
                if(camera_using->pitch < -89.0f) {
                        camera_using->pitch = -89.0f;
                }
        }

        ap_camera_update_vectors();
        return 0;
}

int ap_camera_process_scroll(float y_offset)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }
        camera_using->zoom -= y_offset;

        if (camera_using->zoom < 1.0f) {
                camera_using->zoom = 1.0f;
        } else if (camera_using->zoom > 45.0f) {
                camera_using->zoom = 45.0f;
        }
        return 0;
}

int ap_camera_set_position(float x, float y, float z)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->position[0] = x;
        camera_using->position[1] = y;
        camera_using->position[2] = z;
        return 0;
}

int ap_camera_set_front(float x, float y, float z)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->front[0] = x;
        camera_using->front[1] = y;
        camera_using->front[2] = z;
        return 0;
}

int ap_camera_set_up(float x, float y, float z)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->up[0] = x;
        camera_using->up[1] = y;
        camera_using->up[2] = z;
        return 0;
}

int ap_camera_set_yaw(float yaw)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->yaw = yaw;
        return 0;
}

int ap_camera_set_pitch(float pitch)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->pitch = pitch;
        return 0;
}

int ap_camera_set_sensitivity(float sensitivity)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->sensitivity = sensitivity;
        return 0;
}

int ap_camera_set_speed(float speed)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->speed = speed;
        return 0;
}

int ap_camera_set_zoom(float zoom)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->zoom = zoom;
        return 0;
}

struct AP_Camera* ap_get_current_camera()
{
        return camera_using;
}