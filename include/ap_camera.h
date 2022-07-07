/**
 * @file ap_camera.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Camera related functions and struct objects
 *
 */
#ifndef AP_CAMERA_H
#define AP_CAMERA_H

#include <stdio.h>
#include <stdbool.h>

#include "cglm/cglm.h"
#include "cglm/call.h"

/**
 * @brief Camera struct object
 */
struct AP_Camera {
        /** camera id */
        int id;

        /** camera position */
        float position[3];
        /** front vector of the camera */
        float front[3];
        /** up vector of the camera */
        float up[3];
        /** right vector of the camera */
        float right[3];
        /** world up vector of the camera */
        float worldUp[3];

        float yaw;
        float pitch;
        /** move speed */
        float speed;
        /** sensitivity for process mouse (screen touching) event */
        float sensitivity;
        /** zoom */
        float zoom;
};

/**
 * @brief Generate a camera and get its camera id
 *
 * @param camera_id [out] camera id
 * @return int
 */
int ap_camera_generate(unsigned int *camera_id);

/**
 * @brief Switch to specific camera by ID
 *
 * @param camera_id
 * @return int
 */
int ap_camera_use(unsigned int camera_id);

/**
 * @brief Release one camera by its ID
 *
 * @param id
 * @return int
 */
int ap_camera_free(int id);

/**
 * @brief Release all generated cameras
 *
 * @return int
 */
int ap_camera_free_all();

/**
 * @brief Get view matrix of current using camera
 *
 * @param temp [out] pointer points to view matrix
 * @return int
 */
int ap_camera_get_view_matrix(mat4 *temp);

/**
 * @brief Get position of the camera
 *
 * @param vec [out]
 * @return int
 */
int ap_camera_get_position(float *vec);

/**
 * @brief Get the zoom value of the camera
 *
 * @param zoom
 * @return int
 */
int ap_camera_get_zoom(int *zoom);

/**
 * @brief Get the front vector of the camera
 *
 * @param vec
 * @return int
 */
int ap_camera_get_front(float *vec);

/**
 * @brief Move camera by direction
 *
 * @param direction should be AP_DIRECTION_*
 * @param speed_up default is 1.0f
 * @return int AP_Error_Types
 */
int ap_camera_process_movement(int direction, int speed_up);

/**
 * @brief Rotate camera view port (used for process mouse movement/screen touch
 * events)
 *
 * @param x_offset
 * @param y_offset
 * @param pitch should be true
 * @return int
 */
int ap_camera_process_mouse_move(float x_offset, float y_offset, bool pitch);

/**
 * @brief Process mouse scroll event (scale zoom)
 *
 * @param y_offset
 * @return int
 */
int ap_camera_process_scroll(float y_offset);

/**
 * @brief Set camera position
 *
 * @param x
 * @param y
 * @param z
 * @return int
 */
int ap_camera_set_position(float x, float y, float z);

/**
 * @brief Set up vector
 *
 * @param x
 * @param y
 * @param z
 * @return int
 */
int ap_camera_set_up(float x, float y, float z);
int ap_camera_set_yaw(float yaw);
int ap_camera_get_yaw(float *yaw);
int ap_camera_set_pitch(float pitch);
int ap_camera_get_pitch(float *pitch);
int ap_camera_set_sensitivity(float sensitivity);
int ap_camera_set_speed(float speed);
int ap_camera_set_zoom(float zoom);

/**
 * @brief Get the camera pointer by ID
 *
 * @param id
 * @return struct AP_Camera*
 */
struct AP_Camera* ap_camera_get_ptr(int id);

/**
 * @brief Get current using camera ID
 *
 * @return unsigned int
 */
unsigned int ap_get_current_camera_id();

/**
 * @brief Get the pointer of current using camera
 *
 * @return struct AP_Camera*
 */
struct AP_Camera* ap_get_current_camera();

#endif // AP_CAMERA_H
