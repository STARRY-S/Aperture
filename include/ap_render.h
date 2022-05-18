/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Main render functions of Game Engine (Aperture)
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_RENDERER_H
#define AP_RENDERER_H

#include "ap_utils.h"

#ifndef AP_FONT_SIZE
#define AP_FONT_SIZE 32
#endif

struct AP_Character {
        unsigned int texture_id;   // id handle of the glyph texture
        int size[2];               // size of glyph
        int bearing[2];            // offset from baseline to left/top glyph
        unsigned int advance;      // offset to advance to next glyph
        char c;           // unsigned char code
};

int ap_render_general_initialize();
int ap_render_main();
int ap_render_finish();
int ap_render_resize_buffer(int width, int height);

/**
 * @brief Init font renderer
 *
 * @param path path to the font file
 * @param size default font size
 * @return int AP_Types
 */
int ap_render_init_font(const char *path, int size);

/**
 * @brief Charactor to texture
 *
 * @param c   char code
 * @param tid [out] texture ID
 * @param w   [out] bitmap width
 * @param h   [out] bitmap height
 * @param l   [out] offset from baseline to left of glypt
 * @param t   [out] offset from baseline to right of glypt
 * @param a   [out] offset to advance to next glyph
 * @return int AP_Types
 */
int ap_render_char_2_texture(
        char c, unsigned int *tid, int *w, int *h, int *l, int *t, int *a
);

/**
 * @brief Render one line text
 *
 * @param text  string
 * @param x     position of the string (x)
 * @param y     position of the string (y)
 * @param scale scale
 * @param color RGBA
 * @return int AP_Types
 */
int ap_render_text_line(
        const char *text, float x, float y, float scale, float* color);

/**
 * @brief Flush FPS, ortho matrix, etc...
 *
 * @return int AP_Types
 */
int ap_render_flush();

/**
 * @brief Render aiming points
 *
 * @return int
 */
int ap_render_aim_cross();
int ap_render_aim_dot();

int ap_render_get_font_ptr(char c, struct AP_Character **ptr);
int ap_render_get_fps(float *p);

int ap_render_get_persp_matrix(float **mat);
int ap_render_get_ortho_matrix(float **mat);
int ap_render_get_view_matrix(float **mat);
int ap_render_get_ortho_shader(unsigned int *p);
int ap_render_get_persp_shader(unsigned int *p);
int ap_render_get_dt(float *dt);
int ap_render_get_cft(float *cft);
int ap_render_get_view_distance(int *p);

/**
 * @brief Set model matrix before render model
 *
 * @param mat
 * @return int
 */
int ap_render_set_model_mat(float *mat);

/**
 * @brief Set light enabled or not
 *
 * @param b
 * @return int
 */
int ap_render_set_spot_light_enabled(bool b);
int ap_render_set_point_light_enabled(bool b);
int ap_render_set_env_light_enabled(bool b);

/**
 * @brief set using material number in persp shader program
 *
 * @param n
 * @return int
 */
int ap_render_set_material_num(int n);

/**
 * @brief Set view distance
 *
 * @param n
 * @return int
 */
int ap_render_set_view_distance(int n);

int ap_render_set_main_func(ap_callback_func_t func);

/**
 * @brief Set cross aiming point parameters
 *
 * @param length image size: length * length pixels
 * @param width line width
 * @param color RGBA color value
 * @return int
 */
int ap_render_set_aim_cross(int length, int width, vec4 color);

/**
 * @brief Set dot (square) aiming point parameters
 *
 * @param size dot size: size * size pixels
 * @param color RGBA color value
 * @return int
 */
int ap_render_set_aim_dot(int size, vec4 color);

/**
 * @brief Use Logarithmic Depth Buffer
 *
 * @deprecated z buffer optimized is enabled by default
 * @param b enabled
 * @return int
 */
int ap_render_set_optimize_zconflict(bool b);

/**
 * @brief Render a image on orthographic shader
 *
 * @param pos ivec2 picture position
 * @param size ivec2 image size
 * @param tex_id the texture ID
 * @param tex_num which texture number should use:
 * The tex num 0 is used for GL_RED format image,
 * and it is used for rendering shapes (font, aiming point...)
 *
 * The tex num 1 is used for GL_REBA format image,
 * it is used for rendering common images.
 * @return int
 */
int ap_render_ortho_image_texture(
        ivec2 pos, ivec2 size, unsigned int tex_id, int tex_num
);

int ap_get_buffer_width();
int ap_get_buffer_height();
void* ap_get_context_ptr();

int ap_set_buffer(int w, int h);
int ap_set_context_ptr(void* ptr);

#endif // AP_RENDERER_H
