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

struct AP_Renderer {
        // FPS
        float fps;

        // font buffer
        unsigned int font_VAO;
        unsigned int font_VBO;
        unsigned int font_shader;
        FT_Library ft_library;
        FT_Face    ft_face;
        bool font_initialized;
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
        // load ortho shader, for fonts rendering
        ap_shader_generate(
                "ap_glsl/ap_ortho_font.vs.glsl",
                "ap_glsl/ap_ortho_font.fs.glsl",
                &renderer.font_shader
        );

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
        ap_shader_use(renderer.font_shader);
        ap_shader_set_vec4(renderer.font_shader, "color", color);

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
                        LOGW("unable to render char %c", text[i]);
                        continue;
                }
                float xpos = x + p->bearing[0] * scale;
                float ypos = y - (p->size[1] - p->bearing[1]) * scale;

                float w = p->size[0] * scale;
                float h = p->size[1] * scale;
                // update VBO for each character
                float vertices[6][4] = {
                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos,     ypos,       0.0f, 1.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },

                        { xpos,     ypos + h,   0.0f, 0.0f },
                        { xpos + w, ypos,       1.0f, 1.0f },
                        { xpos + w, ypos + h,   1.0f, 0.0f }
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
        ap_shader_use(0);

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
        // renderer ortho matrix
        mat4 projection;
        ap_shader_use(renderer.font_shader);
        glm_mat4_identity(projection);
        vec4 ortho_proj = {0.0f, 0.0f, 0.0f};
        glm_ortho(
                (float) 0.0,
                (float) ap_get_buffer_width(),
                (float) 0.0,
                (float) ap_get_buffer_height(),
                -1.0, 1.0, &ortho_proj
        );
        ap_shader_set_mat4(renderer.font_shader, "projection", ortho_proj);
        ap_shader_use(0);

        // calculate FPS
        static long long frames = 0;
        static double since = 0.0;
        ++frames;
        // get time, in seconds
        double now = glfwGetTime();
        double elapsed = now - since;
        if (elapsed >= 1.0) {
                renderer.fps = frames / elapsed;
                frames = 0;
                since = now;
        }

        return 0;
}

int ap_render_finish()
{
        ap_camera_free();
        ap_shader_free();
        ap_model_free();
        ap_texture_free();
        ap_memory_release();

        FT_Done_Face(renderer.ft_face);
        FT_Done_FreeType(renderer.ft_library);

        glDeleteBuffers(1, &renderer.font_VBO);
        glDeleteVertexArrays(1, &renderer.font_VAO);

        ap_vector_free(&charactor_vector);

        return EXIT_SUCCESS;
}

int ap_resize_screen_buffer(int width, int height)
{
        LOGD("Resize buffer to width: %d, height: %d", width, height);
        ap_set_buffer(width, height);
        glViewport(0, 0, width, height);
        return 0;
}
