/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Camera struct object defination and function definations
 * of Game Engine (Aperture)
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_CAMERA_H
#define AP_CAMERA_H

#include <stdio.h>
#include <stdbool.h>

#include "cglm/cglm.h"
#include "cglm/call.h"

struct AP_Camera {
        int id;

        float position[3];
        float front[3];
        float up[3];
        float right[3];
        float worldUp[3];

        float yaw;
        float pitch;
        float speed;
        float sensitivity;
        float zoom;
};

int ap_camera_generate(unsigned int *camera_id);
int ap_camera_use(unsigned int camera_id);
int ap_camera_free(int id);
int ap_camera_free_all();

/**
 * @brief Get view matrix of current using camera
 *
 * @param temp [out] pointer points to view matrix
 * @return int
 */
int ap_camera_get_view_matrix(mat4 *temp);
int ap_camera_get_position(float *vec);
int ap_camera_get_zoom(int *zoom);
int ap_camera_get_front(float *vec);

/**
 * @brief Move camera directly with its direction
 *
 * @param direction should be AP_DIRECTION_*
 * @param speed_up default is 1.0f
 * @return int AP_Error_Types
 */
int ap_camera_process_movement(int direction, int speed_up);
int ap_camera_process_mouse_move(float x_offset, float y_offset, bool pitch);
int ap_camera_process_scroll(float y_offset);

int ap_camera_set_position(float x, float y, float z);
int ap_camera_set_up(float x, float y, float z);
int ap_camera_set_yaw(float yaw);
int ap_camera_get_yaw(float *yaw);
int ap_camera_set_pitch(float pitch);
int ap_camera_get_pitch(float *pitch);
int ap_camera_set_sensitivity(float sensitivity);
int ap_camera_set_speed(float speed);
int ap_camera_set_zoom(float zoom);

struct AP_Camera* ap_camera_get_ptr(int id);

unsigned int ap_get_current_camera_id();
struct AP_Camera* ap_get_current_camera();

#endif // AP_CAMERA_H
