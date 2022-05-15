#ifndef __ANDROID__
#endif

#include <assimp/cfileio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "ap_render.h"
#include "ap_utils.h"
#include "ap_camera.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_model.h"
#include "ap_mesh.h"
#include "ap_custom_io.h"
#include "ap_audio.h"
#include "ap_light.h"
#include "ap_physic.h"
#include "ap_math.h"

struct AP_Renderer {
        float fps;      // frame per second
        float dt;       // delta time (cft - lft)
        float lft;      // last frame time
        float cft;      // current frame time
        unsigned long long frame_count;

        // font buffer
        unsigned int font_VAO;
        unsigned int font_VBO;
        FT_Library ft_library;
        FT_Face    ft_face;
        bool font_initialized;

        unsigned int ortho_shader; // Orthographic
        unsigned int persp_shader; // Perspective

        mat4 ortho_matrix;
        mat4 persp_matrix;
        mat4 view_matrix;
        bool spot_light_enabled;
        int material_num;          // materials[material_num]
        int view_distance;

        // for cross aiming point
        unsigned int cross_aim_texture_id; // texture id of aim
        int cross_aim_width;            // the image width of cross aim
        vec4 cross_aim_color;           // color of the cross aim point

        // for dot aiming point
        unsigned int dot_aim_texture_id; // texture id of dot_aim
        int dot_aim_size;
        vec4 dot_aim_color;             // color of the dot aim

        ap_callback_func_t main_func;
};

// Aperture engine only have one renderer as the main renderer
static struct AP_Renderer renderer;
static struct AP_Vector charactor_vector = { 0, 0, 0, 0 };
static bool ap_render_initialized = false;

int ap_render_general_initialize()
{
        if (ap_render_initialized) {
                return 0;
        }
        memset(&renderer, 0, sizeof(struct AP_Renderer));
        // setup startup time
        ap_get_time();
        ap_physic_init();

#ifdef __ANDROID__
        // init for android
#else
        if (ap_get_context_ptr() == NULL) {
                LOGE("aperture general init failed");
                return AP_ERROR_INIT_FAILED;
        }

        if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
        {
                LOGE("failed to init GLAD");
                return AP_ERROR_INIT_FAILED;
        }

#endif
        ap_shader_generate(
                "ap_glsl/ap_orthographic.vs.glsl",
                "ap_glsl/ap_orthographic.fs.glsl",
                &renderer.ortho_shader
        );

        ap_shader_generate(
                "ap_glsl/ap_perspective.vs.glsl",
                "ap_glsl/ap_perspective.fs.glsl",
                &renderer.persp_shader
        );
        renderer.view_distance = 16 * 16;

        ap_render_initialized = true;
        return EXIT_SUCCESS;
}

int ap_render_init_font(const char *path, int size)
{
        if (!path) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        if (renderer.font_initialized) {
                return 0;
        }

        // init freetype
        int error = 0;
        if (!renderer.ft_library && !renderer.ft_face) {
                FT_Init_FreeType(&renderer.ft_library);
                error = FT_New_Face(
                        renderer.ft_library,
                        path,
                        0,
                        &renderer.ft_face
                );
        }
        if (error) {
                LOGE("failed to initialize freetype %d", error);
                return AP_ERROR_INIT_FAILED;
        }

        if (error == FT_Err_Unknown_File_Format) {
                // the font file could be opened and read, but it appears
                // that its font format is unsupported
                LOGE("load font: unknown format");
                return AP_ERROR_INIT_FAILED;
        } else if (error) {
                LOGE("load font: %d", error);
                return AP_ERROR_INIT_FAILED;
        }

        if (size) {
                error = FT_Set_Pixel_Sizes(renderer.ft_face, 0, size);
        } else {
                error = FT_Set_Pixel_Sizes(renderer.ft_face, 0, AP_FONT_SIZE);
        }
        if (error) {
                LOGE("failed to initialize freetype: %d", error);
                return AP_ERROR_INIT_FAILED;
        }

        glGenVertexArrays(1, &renderer.font_VAO);
        glGenBuffers(1, &renderer.font_VBO);
        glBindVertexArray(renderer.font_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, renderer.font_VBO);
        glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(float) * 6 * 4,
                NULL,
                GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        renderer.font_initialized = true;
        ap_vector_init(&charactor_vector, AP_VECTOR_CHARACTOR);
        for (char c = 0; c < 127; ++c) {
                unsigned int tid = 0;
                int width = 0, height = 0, left = 0, top = 0, advance = 0;
                ap_render_char_2_texture(
                        c, &tid, &width, &height, &left, &top, &advance
                );
                struct AP_Character charactor;
                charactor.texture_id = tid;
                charactor.c = c;
                charactor.size[0] = width;
                charactor.size[1] = height;
                charactor.bearing[0] = left;
                charactor.bearing[1] = top;
                charactor.advance = advance;
                ap_vector_push_back(&charactor_vector, (void*) &charactor);
        }

        return 0;
}

int ap_render_char_2_texture(
        char c, unsigned int *tid, int *w, int *h, int *l, int *t, int *a)
{
        if (!renderer.font_initialized) {
                LOGE("failed to char_2_texture: not initialized");
                return AP_ERROR_INIT_FAILED;
        }

        int error = 0;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // load character glyph
        error = FT_Load_Char(renderer.ft_face, c, FT_LOAD_RENDER);
        if (error) {
                LOGE("freetype: failed to load glyph %d", error);
                return 0;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                renderer.ft_face->glyph->bitmap.width,
                renderer.ft_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                renderer.ft_face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        *tid = texture;
        if (w) {
                *w = renderer.ft_face->glyph->bitmap.width;
        }
        if (h) {
                *h = renderer.ft_face->glyph->bitmap.rows;
        }
        if (l) {
                *l = renderer.ft_face->glyph->bitmap_left;
        }
        if (t) {
                *t = renderer.ft_face->glyph->bitmap_top;
        }
        if (a) {
                *a = renderer.ft_face->glyph->advance.x;
        }

        return 0;
}

int ap_render_text_line(
        const char *text, float x, float y, float scale, float* color)
{
        if (text == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (renderer.font_VAO == 0 || renderer.font_VBO == 0) {
                LOGE("failed to render font: buffer uninitialized");
                return AP_ERROR_INIT_FAILED;
        }

        glBindVertexArray(renderer.font_VAO);
        unsigned int old_shader = ap_get_current_shader();
        ap_shader_use(renderer.ortho_shader);
        ap_shader_set_vec4(renderer.ortho_shader, "color", color);

        // enable blend
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(renderer.font_VAO);
        int length = strlen(text);
        for (int i = 0; i < length; ++i) {
                struct AP_Character *index = NULL, *p = NULL;
                for (int j = 0; j < charactor_vector.length; ++j) {
                        index = (struct AP_Character*) charactor_vector.data;
                        if (index[j].c == text[i]) {
                                p = index + j;
                                break;
                        }
                }
                if (!p) {
                        continue;
                }
                float xpos = x + p->bearing[0] * scale;
                float ypos = y - (p->size[1] - p->bearing[1]) * scale;

                float w = p->size[0] * scale;
                float h = p->size[1] * scale;
                // update VBO for each character
                float vertices[6][4] = {
                        { xpos,     ypos + h, 0.0f, 0.0f },
                        { xpos,     ypos,     0.0f, 1.0f },
                        { xpos + w, ypos,     1.0f, 1.0f },

                        { xpos,     ypos + h, 0.0f, 0.0f },
                        { xpos + w, ypos,     1.0f, 1.0f },
                        { xpos + w, ypos + h, 1.0f, 0.0f }
                };
                 // render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, p->texture_id);
                // update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, renderer.font_VBO);
                glBufferSubData(
                        GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices
                );
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // now advance cursors for next glyph
                // (note that advance is number of 1/64 pixels)
                // bitshift by 6 to get value in pixels (2^6 = 64)
                x += (p->advance >> 6) * scale;
        }

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_set_aim_cross(int length, int width, vec4 color)
{
        // generate a GL_RED format image
        unsigned char *data = AP_MALLOC(
                length * length * sizeof(unsigned char)
        );
        memset(data, 0, length * length * sizeof(unsigned char));
        int min = (length - width) / 2;
        int max = (length + width) / 2;
        for (int i = 0; i < length; ++i) {
                for (int j = 0; j < length; ++j) {
                        if (!((i >= min && i < max) || (j >= min && j < max))) {
                                continue;
                        }
                        int offset = (i * length + j);
                        data[offset] = 255;     // red color
                }
        }
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set texture wrap parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filter parameters
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST
        );
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_NEAREST
        );

        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED, length, length,
                0, GL_RED, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        renderer.cross_aim_texture_id = texture;
        renderer.cross_aim_width = length;
        memcpy(renderer.cross_aim_color, color, VEC4_SIZE);
        AP_FREE(data);

        return 0;
}

int ap_render_aim_cross()
{
        if (renderer.font_VAO == 0 || renderer.font_VBO == 0) {
                LOGE("failed to render aim: buffer uninitialized");
                return AP_ERROR_INIT_FAILED;
        }

        glBindVertexArray(renderer.font_VAO);
        unsigned int old_shader = ap_get_current_shader();
        ap_shader_use(renderer.ortho_shader);
        ap_shader_set_vec4(
                renderer.ortho_shader,
                "color", renderer.cross_aim_color
        );

        // enable blend
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(renderer.font_VAO);

        float w, h;
        w = h = renderer.cross_aim_width;
        float xpos = (ap_get_buffer_width() - w) / 2;
        float ypos = (ap_get_buffer_height() - h) / 2;

        // LOGD("w h %f %f", w, h);
        // update VBO for each character
        float vertices[6][4] = {
                // xpos     ypos        tex coord
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, renderer.cross_aim_texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, renderer.font_VBO);
        glBufferSubData(
                GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_set_aim_dot(int size, vec4 color)
{
        // generate a GL_RED format image
        unsigned char *data = AP_MALLOC(
                size * size * sizeof(unsigned char)
        );
        memset(data, 0, size * size * sizeof(unsigned char));
        // draw a circle
        for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                        int offset = (i * size + j);
                        data[offset] = 255;     // red color
                }
        }
        unsigned int texture = 0;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // set texture wrap parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filter parameters
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST
        );
        glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_NEAREST
        );

        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED, size, size,
                0, GL_RED, GL_UNSIGNED_BYTE, data
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        renderer.dot_aim_texture_id = texture;
        renderer.dot_aim_size = size;
        memcpy(renderer.dot_aim_color, color, VEC4_SIZE);
        AP_FREE(data);
        return 0;
}

int ap_render_aim_dot()
{
        if (renderer.font_VAO == 0 || renderer.font_VBO == 0) {
                LOGE("failed to render aim: buffer uninitialized");
                return AP_ERROR_INIT_FAILED;
        }

        glBindVertexArray(renderer.font_VAO);
        unsigned int old_shader = ap_get_current_shader();
        ap_shader_use(renderer.ortho_shader);
        ap_shader_set_vec4(
                renderer.ortho_shader,
                "color", renderer.dot_aim_color
        );

        // enable blend
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(renderer.font_VAO);

        float w, h;
        w = h = renderer.dot_aim_size;
        float xpos = (ap_get_buffer_width() - w) / 2;
        float ypos = (ap_get_buffer_height() - h) / 2;

        // update VBO for each character
        float vertices[6][4] = {
                // xpos     ypos        tex coord
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        glBindTexture(GL_TEXTURE_2D, renderer.dot_aim_texture_id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, renderer.font_VBO);
        glBufferSubData(
                GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_get_font_ptr(char c, struct AP_Character **ptr)
{
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        *ptr = NULL;
        struct AP_Character *index = NULL;
        for (int j = 0; j < charactor_vector.length; ++j) {
                index = (struct AP_Character*) charactor_vector.data;
                if (index[j].c == c) {
                        *ptr = index + j;
                        break;
                }
        }
        return 0;
}

int ap_render_get_fps(float *p)
{
        if (p) {
                *p = renderer.fps;
        }
        return 0;
}

int ap_render_flush()
{
        ++renderer.frame_count;
        unsigned int old_shader = ap_get_current_shader();
        renderer.cft = ap_get_time();
        if (renderer.lft != 0) {
                renderer.dt = renderer.cft - renderer.lft;
        }
        renderer.lft = renderer.cft;
        static int frames = 0;
        static float since = 0.0f;
        if (since < 0.001f) {
                since = renderer.cft;
        }
        ++frames;
        double elapsed = renderer.cft - since;
        if (elapsed >= 1.0) {
                renderer.fps = frames / elapsed;
                frames = 0;
                since = renderer.cft;
        }

        ap_shader_use(renderer.persp_shader);
        glm_mat4_identity(renderer.view_matrix);
        ap_camera_get_view_matrix(&renderer.view_matrix);
        ap_shader_set_mat4(
                renderer.persp_shader, AP_SP_VIEW, renderer.view_matrix[0]);
        vec3 view_pos = {0.0f};
        ap_camera_get_position(view_pos);
        ap_shader_set_vec3(
                renderer.persp_shader, AP_SP_VIEW, view_pos);
        ap_shader_set_vec3(
                renderer.persp_shader, AP_SP_SL_POSITION, view_pos);
        vec3 cam_direction = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_front(cam_direction);
        ap_shader_set_vec3(
                renderer.persp_shader, AP_SP_SL_DIRECTION, cam_direction);
        ap_shader_set_int(
                renderer.persp_shader, AP_SP_SPOT_LIGHT_ENABLED,
                renderer.spot_light_enabled
        );
        ap_shader_set_int(
                renderer.persp_shader, AP_SP_MATERIAL_NUMBER,
                renderer.material_num
        );

        int zoom = 0;
        ap_camera_get_zoom(&zoom);
        ap_shader_use(renderer.persp_shader);
        glm_mat4_identity(renderer.persp_matrix);
        glm_perspective(
                glm_rad(zoom),
                (float) ap_get_buffer_width() / ap_get_buffer_height(),
                0.01f, (float) renderer.view_distance,
                renderer.persp_matrix
        );
        ap_shader_set_mat4(
                renderer.persp_shader,
                AP_SP_PROJECTION,
                renderer.persp_matrix[0]
        );
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_finish()
{
        ap_vector_free(&charactor_vector);
        ap_camera_free();
        ap_shader_free();
        ap_model_free();
        ap_texture_free();
        ap_audio_free();
        ap_light_free();

        FT_Done_Face(renderer.ft_face);
        FT_Done_FreeType(renderer.ft_library);

        glDeleteBuffers(1, &renderer.font_VBO);
        glDeleteVertexArrays(1, &renderer.font_VAO);

        ap_memory_release();

        return EXIT_SUCCESS;
}

int ap_render_resize_buffer(int width, int height)
{
        LOGD("Resize buffer to width: %d, height: %d", width, height);
        ap_set_buffer(width, height);
        glViewport(0, 0, width, height);

        unsigned int old_shader = ap_get_current_shader();

        // renderer ortho matrix
        ap_shader_use(renderer.ortho_shader);
        glm_mat4_identity(renderer.ortho_matrix);
        glm_ortho(
                0.0f, (float) ap_get_buffer_width(),
                0.0f, (float) ap_get_buffer_height(),
                -1.0f, 1.0f, renderer.ortho_matrix
        );
        ap_shader_set_mat4(
                renderer.ortho_shader,
                // AP_RENDER_NAME_PROJECTION,
                AP_SO_PROJECTION,
                renderer.ortho_matrix[0]
        );
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_get_persp_matrix(float **mat)
{
        if (!mat) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *mat = renderer.persp_matrix[0];
        return 0;
}

int ap_render_get_ortho_matrix(float **mat)
{
        if (!mat) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *mat = renderer.ortho_matrix[0];
        return 0;
}

int ap_render_get_view_matrix(float **mat)
{
        if (!mat) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *mat = renderer.view_matrix[0];

        return 0;
}

int ap_render_get_persp_shader(unsigned int *p)
{
        if (*p) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *p = renderer.persp_shader;

        return 0;
}

int ap_render_get_ortho_shader(unsigned int *p)
{
        if (*p) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *p = renderer.ortho_shader;

        return 0;
}

int ap_render_get_dt(float *dt)
{
        if (!dt) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *dt = renderer.dt;
        return 0;
}

int ap_render_get_cft(float *cft)
{
        if (!cft) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        *cft = renderer.cft;
        return 0;
}

int ap_render_set_model_mat(float *mat)
{
        if (renderer.persp_shader == 0) {
                return AP_ERROR_INIT_FAILED;
        }
        if (!mat) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        unsigned int old_shader = ap_get_current_shader();
        ap_shader_use(renderer.persp_shader);
        ap_shader_set_mat4(renderer.persp_shader,
                // AP_RENDER_NAME_MODEL,
                AP_SP_MODEL,
                mat
        );
        ap_shader_use(old_shader);

        return 0;
}

int ap_render_set_spot_light_open(bool b)
{
        renderer.spot_light_enabled = b;
        return 0;
}

int ap_render_set_material_num(int n)
{
        if (n > AP_TEXTURE_UNIT_MAX_NUM) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        renderer.material_num = n;
        return 0;
}

int ap_render_set_view_distance(int n)
{
        renderer.view_distance = n;
        return 0;
}

int ap_render_set_main_func(ap_callback_func_t func)
{
        if (!func) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        renderer.main_func = func;
        return 0;
}