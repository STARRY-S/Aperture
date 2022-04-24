#include "demo_utils.h"
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

GLuint model_id = 0;
GLuint camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
GLuint camera_use_id = 0;

vec4 light_position = { 1.0f, 8.0f, 1.0f, 0.0f };

GLuint light_shader = 0, cube_shader = 0;
GLuint light_texture = 0;
GLuint VBO = 0;
GLuint light_cube_VAO = 0;
bool enable_mobile_type = false;

unsigned audio_buffer_id = 0;

struct AP_Audio *audio = NULL;

int demo_setup_light();

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

        ap_audio_open_file_decode("sound/c418-haggstorm.mp3", &audio);
        if (audio && audio->buffer_id > 0) {
                ap_audio_play_buffer(audio->buffer_id);
        }

        return 0;
}

int demo_setup_light()
{
        ap_shader_generate(
                "glsl/model_light.vs.glsl",
                "glsl/model_light.fs.glsl",
                &light_shader
        );

        ap_shader_generate(
                "glsl/model_loading.vs.glsl",
                "glsl/model_loading.fs.glsl",
                &cube_shader
        );

        light_texture = ap_texture_from_file("lamp.png", "mc/", false);
        ap_shader_set_int(cube_shader, "texture0", 0);

        // light cube initialize
        const float* cube_vertices = ap_get_default_cube_vertices();
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
                GL_ARRAY_BUFFER,
                ap_get_default_cube_vertices_length(),
                cube_vertices,
                GL_STATIC_DRAW
        );

        glGenVertexArrays(1, &light_cube_VAO);
        glBindVertexArray(light_cube_VAO);
        glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(
                2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                (void*)(3 * sizeof(float))
        );
        glEnableVertexAttribArray(2);

        vec4 light_ambient = { 1.0f, 0.95f, 0.9f, 1.0f };
        vec4 light_diffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
        vec4 light_specular = { 5.0f, 5.0f, 5.0f, 1.0f };

        ap_shader_use(light_shader);
        ap_shader_set_int(light_shader, "material.diffuse", 0);
        ap_shader_set_int(light_shader, "material.specular", 1);
        ap_shader_set_int(light_shader, "material.normal", 2);

        ap_shader_set_vec3(light_shader, "light.position", light_position);
        ap_shader_set_vec4(light_shader, "light.ambient", light_ambient);
        ap_shader_set_vec4(light_shader, "light.diffuse", light_diffuse);
        ap_shader_set_vec4(light_shader, "light.specular", light_specular);
        ap_shader_set_float(light_shader, "material.shininess", 32.0f);

        ap_shader_set_float(light_shader, "light.constant", 1.0f);
        ap_shader_set_float(light_shader, "light.linear", 0.09f);
        ap_shader_set_float(light_shader, "light.quadratic", 0.032f);

        return 0;
}

int demo_render()
{
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glUseProgram(light_shader);
        ap_shader_use(light_shader);

        vec3 camera_position = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_position(camera_position);
        ap_shader_set_vec3(light_shader, "viewPos", camera_position);

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
        glm_mat4_identity(mat_model);
        glm_translate(mat_model, light_position);
        vec3 cube_scale = { 0.2f, 0.2f, 0.2f };
        glm_scale(mat_model, cube_scale);  // smaller
        ap_shader_set_mat4(cube_shader, "model", mat_model[0]);
        glBindVertexArray(light_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        ap_shader_use(0);
        return EXIT_SUCCESS;
}

int demo_finished()
{
        glDeleteVertexArrays(1, &light_cube_VAO);
        glDeleteBuffers(1, &VBO);

        ap_render_finish();
        ap_audio_delete_buffer(audio_buffer_id);
        ap_audio_finish();
        return 0;
}
