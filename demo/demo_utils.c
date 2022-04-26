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
#define MODEL_FILE_NAME "mc/minecraft.obj"
#endif

#ifdef __ANDROID__
#define AP_DEMO_CAMERA_NUMBER 1
#else
#define AP_DEMO_CAMERA_NUMBER 5
#endif

unsigned int model_id = 0;
unsigned int camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
unsigned int camera_use_id = 0;

bool spot_light_enabled = false;

vec3 light_positions[DEMO_POINT_LIGHT_NUM] = {
        {1.0f, 8.0f, 1.0f},
        {5.0f, 9.0f, 1.0f},
        {10.0f, 11.0f, 1.0f},
        {-10.0f, 10.0f, 1.0f}
};

unsigned int light_shader = 0, cube_shader = 0;
unsigned int light_texture = 0;
unsigned int VBO = 0;
unsigned int light_cube_VAO = 0;
bool enable_mobile_type = false;

int demo_init()
{
        ap_audio_init();
        demo_setup_light();
        // Setup GameEngine
        // init model
        AP_CHECK(
                ap_model_generate(MODEL_FILE_NAME, &model_id)
        );

        // camera
        for (int i = 0; i < AP_DEMO_CAMERA_NUMBER; ++i) {
                ap_camera_generate(&camera_ids[i]);
                ap_camera_use(camera_ids[i]);
                ap_camera_set_position(0.0f, (float) i + 8.0f, 1.0f);
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

        // depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // cull face
        // glEnable(GL_CULL_FACE);

        // ap_audio_open_file_MP3("sound/c418-haggstorm.mp3", &audio);
        // if (audio && audio->buffer_id > 0) {
        //         // ap_audio_play_buffer(audio->buffer_id);
        // }

        unsigned int audio_id = 0;
        ap_audio_load_MP3("sound/c418-haggstorm.mp3", &audio_id);
        if (audio_id > 0) {
                ap_audio_play(audio_id, NULL);
        }

        return 0;
}

int demo_render()
{
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ap_shader_use(light_shader);

        vec3 cam_position = { 0.0f, 0.0f, 0.0f };
        vec3 cam_direction = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_position(cam_position);
        ap_camera_get_front(cam_direction);
        ap_shader_set_vec3(light_shader, "viewPos", cam_position);
        ap_shader_set_vec3(light_shader, "spot_light.position", cam_position);
        ap_shader_set_vec3(light_shader, "spot_light.direction", cam_direction);
        ap_shader_set_int(light_shader,
                "spot_light_enabled", spot_light_enabled);
        demo_update_light();

        // view/projection transformations
        mat4 view;
        mat4 projection;
        glm_mat4_identity(view);
        glm_mat4_identity(projection);
        ap_camera_get_view_matrix(&view);

        int zoom = 0;
        ap_camera_get_zoom(&zoom);
        glm_perspective(
                glm_rad(zoom),
                (float) ap_get_buffer_width() / (float) ap_get_buffer_height(),
                0.1f, 100.0f, projection
        );

        ap_shader_set_mat4(light_shader, "view", view[0]);
        ap_shader_set_mat4(light_shader, "projection", projection[0]);
        ap_shader_set_int(light_shader, "optDepth", enable_mobile_type);

        mat4 mat_model;
        vec3 model_position = { 0.0, 0.0, 0.0 };
        // render the loaded model
        glm_mat4_identity(mat_model);
        // translate it down so it's at the center of the scene
        vec3 model_scale = { 1.0f, 1.0f, 1.0f };
        // it's a bit too big for our scene, so scale it down
        glm_scale(mat_model, model_scale);
        glm_translate(mat_model, model_position);
        ap_shader_set_mat4(light_shader, "model", (float *) mat_model);
        ap_model_use(model_id);
        AP_CHECK( ap_model_draw() );

        // render the lamp cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_texture);
        ap_shader_use(cube_shader);
        ap_shader_set_mat4(cube_shader, "projection", projection[0]);
        ap_shader_set_mat4(cube_shader, "view", view[0]);

        for (int i = 0; i < DEMO_POINT_LIGHT_NUM; ++i) {
                glm_mat4_identity(mat_model);
                glm_translate(mat_model, light_positions[i]);
                vec3 cube_scale = { 0.2f, 0.2f, 0.2f };
                glm_scale(mat_model, cube_scale);  // smaller
                ap_shader_set_mat4(cube_shader, "model", mat_model[0]);
                glBindVertexArray(light_cube_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        ap_shader_use(0);
        return EXIT_SUCCESS;
}

int demo_finished()
{
        glDeleteVertexArrays(1, &light_cube_VAO);
        glDeleteBuffers(1, &VBO);

        ap_render_finish();
        ap_audio_finish();
        return 0;
}
