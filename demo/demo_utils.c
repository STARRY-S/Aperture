#include "demo_utils.h"
#include "demo_light.h"
#include "ap_utils.h"
#include "ap_render.h"
#include "ap_model.h"
#include "ap_camera.h"
#include "ap_cvector.h"
#include "ap_shader.h"
#include "cglm/cglm.h"
#include "ap_audio.h"

#include <math.h>

#ifndef MODEL_FILE_NAME
#define MODEL_FILE_NAME "mc/spawn.obj"
#endif

unsigned int model_id = 0;
unsigned int camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
unsigned int camera_use_id = 0;

bool spot_light_enabled = false;
int material_number = 0;

vec3 light_positions[DEMO_POINT_LIGHT_NUM] = {
        {0.0f, 55.0f, 6.0f},
        {10.0f, 55.0f, 6.0f},
        {-30.0f, 55.0f, 6.0f},
        {30.0f, 55.0f, 6.0f}
};

vec3 ortho_cube_pos = { 0, 0, 0 };

unsigned int cube_shader = 0;
unsigned int light_texture = 0;
unsigned int light_cube_VAO = 0;
bool enable_mobile_type = false;

static char buffer[AP_DEFAULT_BUFFER_SIZE] = { 0 };

int demo_init()
{
        ap_render_general_initialize();
        ap_audio_init();
        demo_setup_light();
        ap_render_init_font(DEMO_FONT_PATH, 42);
        // init model
        AP_CHECK(
                ap_model_generate(MODEL_FILE_NAME, &model_id)
        );

        // camera
        for (int i = 0; i < AP_DEMO_CAMERA_NUMBER; ++i) {
                ap_camera_generate(&camera_ids[i]);
                ap_camera_use(camera_ids[i]);
                ap_camera_set_position(0.0f, (float) i + 55.0f, 0.0f);
                ap_camera_set_speed(5.0f);
        }
        ap_camera_use(camera_ids[camera_use_id]);

        #ifdef __ANDROID__
        int iMobileType = ap_get_mobile_type(ap_get_mobile_name());
        switch (iMobileType) {
                case AP_MOBILE_GOOGLE:
                case AP_MOBILE_X86:
                enable_mobile_type = false;
                break;
                default:
                enable_mobile_type = true;
        }
        #endif // __ANDROID__

        unsigned int audio_id = 0;
        ap_audio_load_MP3("sound/c418-haggstorm.mp3", &audio_id);
        if (audio_id > 0) {
                ap_audio_play(audio_id, NULL);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        return 0;
}

int demo_render()
{
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // depth test
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // cull face
        glEnable(GL_CULL_FACE);

        // Setup model matrix
        mat4 mat_model;
        vec3 model_position = { 0.0, 0.0, 0.0 };
        glm_mat4_identity(mat_model);
        vec3 model_scale = { 1.0f, 1.0f, 1.0f };
        glm_scale(mat_model, model_scale);
        glm_translate(mat_model, model_position);
        ap_render_set_model_mat((float *) mat_model);
        // render the model
        ap_model_use(model_id);
        AP_CHECK( ap_model_draw() );

        // render the lamp cube
        ap_shader_use(cube_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_texture);
        // ap_render_get
        float *projection = NULL;
        ap_render_get_persp_matrix(&projection);
        ap_shader_set_mat4(cube_shader, "projection", projection);
        float *view = NULL;
        ap_render_get_view_matrix(&view);
        ap_shader_set_mat4(cube_shader, "view", view);

        for (int i = 0; i < DEMO_POINT_LIGHT_NUM; ++i) {
                glm_mat4_identity(mat_model);
                glm_translate(mat_model, light_positions[i]);
                ap_shader_set_mat4(cube_shader, "model", mat_model[0]);
                glBindVertexArray(light_cube_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        vec4 color = {0.9, 0.9, 0.9, 1.0};
        int screen_height = ap_get_buffer_height();
        // render text on the top left
        float fps = 0;
        ap_render_get_fps(&fps);
        vec3 cam_position = { 0.0f, 0.0f, 0.0f };
        vec3 cam_direction = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_position(cam_position);
        ap_camera_get_front(cam_direction);
        sprintf(buffer, "(%.1f, %.1f, %.1f) (%.1f, %.1f, %.1f) %4.1ffps",
                cam_position[0], cam_position[1], cam_position[2],
                cam_direction[0], cam_direction[1], cam_direction[2], fps);
        ap_render_text_line(buffer, 5.0, screen_height - 26.0, 1.0, color);

        glBindVertexArray(0);
        ap_shader_use(0);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        return EXIT_SUCCESS;
}

int demo_finished()
{
        glDeleteVertexArrays(1, &light_cube_VAO);

        return 0;
}
