#include <GLES3/gl3.h>
#include <assimp/cfileio.h>

#include "ap_render.h"
#include "ap_utils.h"
#include "ap_camera.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_model.h"
#include "ap_mesh.h"
#include "ap_custom_io.h"

#define MODEL_FILE_NAME "backpack/backpack.obj"
#define BACKPACK_MODEL_NUM 4

int screen_width = 0, screen_height = 0;

float last_x = 400, last_y = 300;
double virtual_xpos = 0.0, virtual_ypox = 0.0;
bool first_mouse = true;

GLuint light_shader = 0;
GLuint cube_shader = 0;
GLuint light_cube_VAO = 0;
GLuint VBO = 0;
GLuint backpack_model_ids[BACKPACK_MODEL_NUM] = { 0 };

vec3 light_position = { 3.0f, 3.0f, 3.0f };

int ap_render_general_initialize()
{
        ap_shader_generate(
                "glsl/model_light.vs.glsl",
                "glsl/model_light.fs.glsl",
                &light_shader
        );

        ap_shader_generate(
                "glsl/cube_light.vs.glsl",
                "glsl/cube_light.fs.glsl",
                &cube_shader
        );

        // TODO: Seperate the light setup param in individual function
        vec3 light_ambient = { 0.80f, 0.80f, 0.80f };
        vec3 light_diffuse = { 0.5f, 0.5f, 0.5f };
        vec3 light_specular = { 5.0f, 5.0f, 5.0f };

        // glUseProgram(light_shader);
        ap_shader_use(light_shader);
        ap_shader_set_int(light_shader, "material.diffuse", 0);
        ap_shader_set_int(light_shader, "material.specular", 1);

        ap_shader_set_vec3(light_shader, "light.position", light_position);
        ap_shader_set_vec3(light_shader, "light.ambient", light_ambient);
        ap_shader_set_vec3(light_shader, "light.diffuse", light_diffuse);
        ap_shader_set_vec3(light_shader, "light.specular", light_specular);
        ap_shader_set_float(light_shader, "material.shininess", 32.0f);

        ap_shader_set_float(light_shader, "light.constant", 1.0f);
        ap_shader_set_float(light_shader, "light.linear", 0.09f);
        ap_shader_set_float(light_shader, "light.quadratic", 0.032f);

        // glUseProgram(cube_shader);
        ap_shader_use(cube_shader);
        // nothing...
        // glUseProgram(0);
        ap_shader_use(0);

        #ifdef __ANDROID__
        GLuint camera_id;
        ap_camera_generate(&camera_id);
        ap_camera_use(camera_id);
        ap_camera_set_position(0.0f, 0.0f, 10.0f);
        #endif

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
                0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(0);

        // init model
        for (int i = 0; i < BACKPACK_MODEL_NUM; ++i) {
                AP_CHECK(
                        ap_model_generate(
                                MODEL_FILE_NAME,
                                &backpack_model_ids[i]
                        )
                );
                AP_CHECK( ap_model_use(backpack_model_ids[i]) );
        }

        // depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // cull face
        // glEnable(GL_CULL_FACE);

        return EXIT_SUCCESS;
}

int ap_render_main()
{
        static float current_frame = 0.0f;
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
                (float) screen_width / (float) screen_height,
                0.1f, 100.0f, projection
        );

        ap_shader_set_mat4(light_shader, "view", view[0]);
        ap_shader_set_mat4(light_shader, "projection", projection[0]);



        // optimize depth test
        bool enable_mobile_type = false;

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

        ap_shader_set_int(light_shader, "optDepth", enable_mobile_type);

        mat4 mat_model;
        for (int i = 0; i < BACKPACK_MODEL_NUM; ++i) {
                vec3 model_position = {
                        (float) i * 4,
                        (float) i * 4,
                        (float) i * -4
                };
                // render the loaded model
                glm_mat4_identity(mat_model);
                // translate it down so it's at the center of the scene
                vec3 model_scale = { 1.0f, 1.0f, 1.0f };
                // it's a bit too big for our scene, so scale it down
                glm_scale(mat_model, model_scale);
                // rotate model by time.
                vec4 axis = {0.0f, 1.0f, 0.0f};
                glm_rotate(mat_model, glm_rad((float) current_frame), axis);
                glm_translate(mat_model, model_position);
                ap_shader_set_mat4(light_shader, "model", (float *) mat_model);
                ap_model_use(backpack_model_ids[i]);
                AP_CHECK( ap_model_draw() );
        }

        current_frame += 0.2f;

        // render the lamp cube
        // glUseProgram(cube_shader);
        ap_shader_use(cube_shader);
        ap_shader_set_mat4(cube_shader, "projection", projection[0]);
        ap_shader_set_mat4(cube_shader, "view", view[0]);
        glm_mat4_identity(mat_model);
        glm_translate(mat_model, light_position);
        vec3 light_scale = { 0.50f, 0.50f, 0.50f };
        glm_scale(mat_model, light_scale);  // smaller
        ap_shader_set_mat4(light_shader, "model", mat_model[0]);
        glBindVertexArray(light_cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        ap_shader_use(0);
        return EXIT_SUCCESS;
}

int ap_render_finish()
{
        ap_camera_free();
        ap_shader_free();
        ap_model_free();
        ap_texture_free();
        ap_memory_release();

        glDeleteVertexArrays(1, &light_cube_VAO);
        glDeleteBuffers(1, &VBO);
        return EXIT_SUCCESS;
}

int ap_resize_screen_buffer(int width, int height)
{
        LOGD("Resize buffer to width: %d, height: %d", width, height);
        screen_width = width;
        screen_height = height;
        glViewport(0, 0, width, height);
        return 0;
}
