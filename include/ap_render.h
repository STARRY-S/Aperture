/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Main render functions of Game Engine (Aperture)
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_RENDERER_H
#define AP_RENDERER_H

int ap_render_general_initialize();
int ap_render_main();
int ap_render_finish();
int ap_resize_screen_buffer(int width, int height);

#endif // AP_RENDERER_H
