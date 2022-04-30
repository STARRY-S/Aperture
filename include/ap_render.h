/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Main render functions of Game Engine (Aperture)
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_RENDERER_H
#define AP_RENDERER_H

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
int ap_resize_screen_buffer(int width, int height);

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

int ap_render_get_fps(float *p);

#endif // AP_RENDERER_H
