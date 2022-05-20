#include "ap_camera.h"
#include "ap_cvector.h"
#include "ap_render.h"
#include "ap_utils.h"
#include "ap_physic.h"

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
        for (int i = 0; i < camera_vector.length; ++i) {
                if (tmp_cam[i].id == camera_id) {
                        camera_using = tmp_cam + i;
                        return 0;
                }
        }

        LOGW("failed to use camera: id %u not found", camera_id);
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

        camera->right[0] = 0.0f;
        camera->right[0] = 0.0f;
        camera->right[0] = 0.0f;

        camera->up[0] = 0.0f;
        camera->up[1] = 1.0f;
        camera->up[2] = 0.0f;

        // default set camera direction to (1,0,0) (yaw = 0, pitch = 0)
        camera->front[0] = 1.0f;
        camera->front[1] = 0.0f;
        camera->front[2] = 0.0f;
        camera->yaw = 0.0f;
        camera->pitch = 0.0f;

        camera->speed = 1.0f;
        camera->sensitivity = 0.04f;
        camera->zoom = 65.0f;

        return 0;
}

int ap_camera_free()
{
        camera_using = NULL;    // for safety purpose
        ap_vector_free(&camera_vector);

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

        vec3 target = { 0.0f, 0.0f, 0.0f };
        glm_vec3_add(
                camera_using->position,
                camera_using->front,
                target
        );
        glm_lookat(
                camera_using->position,
                target,
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

int ap_camera_get_front(float *vec)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        if (vec == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        vec[0] = camera_using->front[0];
        vec[1] = camera_using->front[1];
        vec[2] = camera_using->front[2];
        return 0;
}

int ap_camera_process_movement(int direction, int speed_up)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        float dt = 0.0f;
        ap_render_get_dt(&dt);
        float velocity = camera_using->speed * dt * speed_up;
        vec3 temp = { 0.0f, 0.0f, 0.0f };
        switch (direction)
        {
        case AP_DIRECTION_FORWARD:
        {
                glm_vec3_scale(camera_using->front, velocity, temp);
                glm_vec3_add(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_DIRECTION_BACKWORD:
        {
                glm_vec3_scale(camera_using->front, velocity, temp);
                glm_vec3_sub(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_DIRECTION_LEFT:
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
        case AP_DIRECTION_RIGHT:
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
        case AP_DIRECTION_UP:
        {
                glm_vec3_scale(camera_using->up, velocity, temp);
                glm_vec3_add(
                        camera_using->position,
                        temp,
                        camera_using->position
                );
                break;
        }
        case AP_DIRECTION_DOWN:
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

        if (camera_using->zoom < 1.f) {
                camera_using->zoom = 1.f;
        } else if (camera_using->zoom > 100.0f) {
                camera_using->zoom = 100.0f;
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

#if 0
int ap_camera_set_front(float x, float y, float z)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        LOGD("set front %.3f, %.3f, %.3f", x, y, z);

        // camera_using->front[0] = x;
        // camera_using->front[1] = y;
        // camera_using->front[2] = z;

        vec3 front_yz = { 0, y, z };  // calc pitch
        vec3 front_xz = { x, 0, z };  // calc yaw
        vec3 x_dir = {1.0f, 0.0f, 0.0f};
        vec3 y_dir = {0.0f, 1.0f, 0.0f};
        vec3 z_dir = {0.0f, 0.0f, 1.0f};

        glm_normalize(front_yz);
        glm_normalize(front_xz);
        // PI (rad) equals to 180 (degree)
        // PI / 180 = 1 RAD / 1 DEG
        // 1 degree = 1 radius * 180 / PI
        float yaw = acosf(glm_dot(front_xz, z_dir)) * 180.0f / AP_PI;
        float pitch = acosf(glm_dot(front_yz, x_dir)) * 180.0f / AP_PI;
        camera_using->yaw = 0.0;
        camera_using->pitch = -90.0;
        ap_camera_update_vectors();

        return 0;
}
#endif

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
        ap_camera_update_vectors();
        return 0;
}

int ap_camera_get_yaw(float *yaw)
{
        if (!yaw || !camera_using) {
                return yaw ? AP_ERROR_INVALID_PARAMETER
                        : AP_ERROR_CAMERA_NOT_SET;
        }
        *yaw = camera_using->yaw;
        return 0;
}

int ap_camera_set_pitch(float pitch)
{
        if (camera_using == NULL) {
                return AP_ERROR_CAMERA_NOT_SET;
        }

        camera_using->pitch = pitch;
        ap_camera_update_vectors();
        return 0;
}

int ap_camera_get_pitch(float *pitch)
{
        if (!pitch || !camera_using) {
                return pitch ? AP_ERROR_INVALID_PARAMETER
                        : AP_ERROR_CAMERA_NOT_SET;
        }
        *pitch = camera_using->pitch;
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

unsigned int ap_get_current_camera_id()
{
        if (camera_using == NULL) {
                return 0;
        }
        return camera_using->id;
}
