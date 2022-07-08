/**
 * @file ap_render.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Renderer and Character definitions
 *
 */
#ifndef AP_RENDERER_H
#define AP_RENDERER_H

#include "ap_utils.h"

#ifndef AP_FONT_SIZE
#define AP_FONT_SIZE 32
#endif

/**
 * @brief Character struct object definition
 */
struct AP_Character {
        /** id handle of the glyph texture */
        unsigned int texture_id;
        /** size of glyph, (w, h) */
        int size[2];
        /** offset from baseline to left/top glyph */
        int bearing[2];
        /** offset to advance to next glyph */
        unsigned int advance;
        /** unsigned char code */
        char c;
};

/**
 * @brief Initialize renderer, and initialize all resources required by engine
 * (such as audio, camera, cvector, light, model, physics, etc...)
 *
 * @see ap_render_finish
 * @return int
 */
int ap_render_general_initialize();

/** @deprecated */
int ap_render_main();

/**
 * @brief Release all engine resources,
 * (such as audio, camera, cvector, light, model, physics, etc...),
 * @see ap_render_general_initialize
 *
 * @return int
 */
int ap_render_finish();

/**
 * @brief Resize window buffer
 *
 * @param width
 * @param height
 * @return int
 */
int ap_render_resize_buffer(int width, int height);

/**
 * @brief Setup freetype2, load 1-255 ASCII characters from font file to memory
 *
 * @param path path to the font file
 * @param size default font size
 * @return int AP_Error_Types
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
 * @return int AP_Error_Types
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
 * @return int AP_Error_Types
 */
int ap_render_text_line(
        const char *text, float x, float y, float scale, float* color
);

/**
 * @brief should be called in render main loop,
 * used for calculate FPS, engine startup time, view matrix,
 * collition detection and other calculations
 *
 * @return int AP_Error_Types
 */
int ap_render_flush();

/**
 * @brief Render aiming points
 *
 * @return int
 */
int ap_render_aim_cross();

/**
 * @brief Render dot (square) points
 *
 * @return int
 */
int ap_render_aim_dot();

/**
 * @brief Get struct AP_Character* pointer by character
 *
 * @param c [in] 8 bit non-unicode character
 * @param ptr [out] pointer
 * @return int
 */
int ap_render_get_font_ptr(char c, struct AP_Character **ptr);

/**
 * @brief Get render FPS
 *
 * @param p [out] fps
 * @return int
 */
int ap_render_get_fps(float *p);

/**
 * @brief get perspective projection matrix
 *
 * @param mat
 * @return int
 */
int ap_render_get_persp_matrix(float **mat);

/**
 * @brief get orthographic projection matrix
 *
 * @param mat
 * @return int
 */
int ap_render_get_ortho_matrix(float **mat);

/**
 * @brief Get camera view matrix
 *
 * @param mat
 * @return int
 */
int ap_render_get_view_matrix(float **mat);

/**
 * @brief Get built-in orthographic Aperture shader ID
 *
 * @param p
 * @return int
 */
int ap_render_get_ortho_shader(unsigned int *p);

/**
 * @brief Get built-in perspective Aperture shader ID
 *
 * @param p
 * @return int
 */
int ap_render_get_persp_shader(unsigned int *p);

/**
 * @brief Get render delta time (last frame time - current frame time)
 *
 * @param dt
 * @return int
 */
int ap_render_get_dt(float *dt);

/**
 * @brief Get current frame time
 *
 * @param cft
 * @return int
 */
int ap_render_get_cft(float *cft);

/**
 * @brief Get view distance
 *
 * @param p
 * @return int
 */
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

/** @see ap_render_set_spot_light_enabled */
int ap_render_set_point_light_enabled(bool b);

/** @see ap_render_set_spot_light_enabled */
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

/** @deprecated */
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
 * The tex num 1 is used for GL_RGBA format image,
 * it is used for rendering common images.
 * @return int
 */
int ap_render_ortho_image_texture(
        ivec2 pos, ivec2 size, unsigned int tex_id, int tex_num
);

int ap_get_buffer_width();
int ap_get_buffer_height();

/**
 * @brief Get GLFW window context pointer
 *
 * @return void*
 */
void* ap_get_context_ptr();

int ap_set_buffer(int w, int h);
int ap_set_context_ptr(void* ptr);

#endif // AP_RENDERER_H
