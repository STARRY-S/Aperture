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

enum AP_Camera_Movements {
        AP_CAMERA_FORWARD,
        AP_CAMERA_MOV_BACKWARD,
        AP_CAMERA_MOV_LEFT,
        AP_CAMERA_MOV_RIGHT,
        AP_CAMERA_MOV_UP,
        AP_CAMERA_MOV_DOWN
};

struct AP_Camera {
        int id;

        vec3 position;
        vec3 front;
        vec3 up;
        vec3 right;
        vec3 worldUp;

        float yaw;
        float pitch;
        float speed;
        float sensitivity;
        float zoom;
};

int ap_camera_generate(unsigned int *camera_id);
int ap_camera_use(unsigned int camera_id);
int ap_camera_init_default();
int ap_camera_free();

int ap_camera_get_view_matrix(mat4 *temp);
int ap_camera_get_position(float *vec);
int ap_camera_get_zoom(int *zoom);

int ap_camera_process_movement(int direction, float delta_time);
int ap_camera_process_mouse_move(float x_offset, float y_offset, bool pitch);
int ap_camera_process_scroll(float y_offset);

int ap_camera_set_position(float x, float y, float z);
int ap_camera_set_front(float x, float y, float z);
int ap_camera_set_up(float x, float y, float z);
int ap_camera_set_yaw(float yaw);
int ap_camera_set_pitch(float pitch);
int ap_camera_set_sensitivity(float sensitivity);
int ap_camera_set_speed(float speed);
int ap_camera_set_zoom(float zoom);

struct AP_Camera* ap_get_current_camera();

#endif // AP_CAMERA_H
