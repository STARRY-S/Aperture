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
#include "ap_physic.h"

#include <math.h>

#ifndef MODEL_FILE_NAME
#define MODEL_FILE_NAME "mc/mc-game.obj"
#endif

unsigned int model_id = 0;
unsigned int camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
unsigned int camera_use_id = 0;

bool spot_light_enabled = false;
int material_number = 0;

vec3 light_positions[DEMO_POINT_LIGHT_NUM] = {
        {0.0f, 2.0f, 0.0f},
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

struct AP_PCreature *player = NULL;
unsigned int creature_id = 0;
struct AP_Vector barrier_id_vector = { 0, 0, 0, 0 };


int demo_init()
{
        ap_render_general_initialize();
        ap_audio_init();
        demo_setup_light();
        ap_render_init_font(DEMO_FONT_PATH, 42);
        // init model
        AP_CHECK(ap_model_generate(MODEL_FILE_NAME, &model_id));
        ap_model_use(model_id);

        // camera
        vec3 player_size = { 0.8f, 1.8f, 0.8f };
        AP_CHECK( ap_physic_generate_creature(&creature_id, player_size) );
        ap_creature_use(creature_id);
        vec3 player_pos = {0.0f, 0.0f, 0.0f};
        ap_creature_set_pos(player_pos);
        vec3 eyes_offset = { 0.0f, 1.0f, 0.0f };
        ap_creature_set_camera_offset(eyes_offset);
        AP_CHECK( ap_physic_get_creature_ptr(creature_id, &player) );
        if (player == NULL || creature_id == 0) {
                LOGE("FATAL: failed to generate player");
                exit(-1);
        }

        // Setup barriers
        ap_vector_init(&barrier_id_vector, AP_VECTOR_INT);
        unsigned int barrier_id = 0;
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        if (barrier_id == 0) {
                LOGE("failed to generate ground barriar");
                exit(-1);
        }
        struct AP_PBarrier *tmp_barrier = NULL;
        ap_barrier_get_ptr(barrier_id, &tmp_barrier);
        if (tmp_barrier == NULL) {
                LOGE("failed to get ground barriar pointer");
                exit(-1);
        }
        // set ground
        // ground size (100, 1, 100)
        tmp_barrier->box.size[0] = tmp_barrier->box.size[2] = 100.0f;
        tmp_barrier->box.size[1] = 1.0f;
        // ground position (0, 0, 0)
        tmp_barrier->box.pos[0] = tmp_barrier->box.pos[1]
                = tmp_barrier->box.pos[2] = 0.0f;
        tmp_barrier->box.pos[1] = -0.5f;

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

        // update player
        ap_physic_update_creature();
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

        mat4 mat_model;
        for (int i = 0; i < DEMO_POINT_LIGHT_NUM; ++i) {
                glm_mat4_identity(mat_model);
                glm_translate(mat_model, light_positions[i]);
                ap_shader_set_mat4(cube_shader, "model", mat_model[0]);
                glBindVertexArray(light_cube_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        vec4 color = {0.9, 0.9, 0.9, 1.0};
        int screen_height = ap_get_buffer_height();
        int screen_width = ap_get_buffer_width();
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
        ap_render_text_line(buffer, 10.0, screen_height - 30.0, 1.0, color);
        sprintf(buffer, "fly: %d (%.1f, %.1f, %.1f)",
                player->floating, player->move.speed[0], player->move.speed[1],
                player->move.speed[2]);
        ap_render_text_line(buffer, 10.0, screen_height - 60.0, 1.0, color);

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
