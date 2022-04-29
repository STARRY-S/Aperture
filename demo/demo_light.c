#include "demo_light.h"
#include "demo_utils.h"
#include "ap_utils.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "cglm/cglm.h"

vec3 point_light_ambient = { 0.32f, 0.3f, 0.31f };
vec3 point_light_diffuse = { 0.2f, 0.2f, 0.2f };
vec3 point_light_specular = { 1.0f, 1.0f, 1.0f };

vec3 dir_light_direction = { -1.0f, -1.0f, -0.0f };
vec3 dir_light_ambient   = { 0.53f, 0.5f,  0.52f };
vec3 dir_light_diffuse   = { 0.4f,  0.4f,  0.4f  };
vec3 dir_light_specular  = { 0.2f,  0.2f,  0.2f  };

vec3 spot_light_ambient = {0.0f, 0.0f, 0.0f};
vec3 spot_light_diffuse = {1.0f, 1.0f, 1.0f};
vec3 spot_light_specular = {1.0f, 1.0f, 1.0f};

static int demo_setup_point_lights()
{
        char *buffer = AP_MALLOC(sizeof(char) * 64);
        for (int i = 0; i < DEMO_POINT_LIGHT_NUM; ++i) {
                sprintf(buffer, "point_lights[%d].position", i);
                ap_shader_set_vec3(light_shader, buffer, light_positions[i]);
                sprintf(buffer, "point_lights[%d].ambient", i);
                ap_shader_set_vec3(light_shader, buffer, point_light_ambient);
                sprintf(buffer, "point_lights[%d].diffuse", i);
                ap_shader_set_vec3(light_shader, buffer, point_light_diffuse);
                sprintf(buffer, "point_lights[%d].specular", i);
                ap_shader_set_vec3(light_shader, buffer, point_light_specular);

                sprintf(buffer, "point_lights[%d].constant", i);
                ap_shader_set_float(light_shader, buffer, 1.0f);
                sprintf(buffer, "point_lights[%d].linear", i);
                ap_shader_set_float(light_shader, buffer, 0.09f);
                sprintf(buffer, "point_lights[%d].quadratic", i);
                ap_shader_set_float(light_shader, buffer, 0.032f);
        }

        AP_FREE(buffer);
        return 0;
}

static int demo_setup_directional_light()
{
        ap_shader_set_vec3(
                light_shader, "direct_light.direction", dir_light_direction);
        ap_shader_set_vec3(
                light_shader, "direct_light.ambient", dir_light_ambient);
        ap_shader_set_vec3(
                light_shader, "direct_light.diffuse", dir_light_diffuse);
        ap_shader_set_vec3(
                light_shader, "direct_light.specular", dir_light_specular);
        return 0;
}

static int demo_setup_spot_light()
{
        ap_shader_set_vec3(light_shader,
                "spot_light.ambient", spot_light_ambient);
        ap_shader_set_vec3(light_shader,
                "spot_light.diffuse", spot_light_diffuse);
        ap_shader_set_vec3(light_shader,
                "spot_light.specular", spot_light_specular);
        ap_shader_set_float(light_shader, "spot_light.contant", 1.0f);
        ap_shader_set_float(light_shader, "spot_light.linear", 0.09f);
        ap_shader_set_float(light_shader, "spot_light.quadratic", 0.032f);

        ap_shader_set_float(light_shader,
                "spot_light.cut_off", (float) cos(glm_rad(22.5f)));
        ap_shader_set_float(light_shader,
                "spot_light.outer_cut_off", (float) cos(glm_rad(25.0f)));
        return 0;
}

int demo_setup_light()
{
        ap_shader_generate(
                "glsl/light_multiple.vs.glsl",
                "glsl/light_multiple.fs.glsl",
                &light_shader
        );

        ap_shader_generate(
                "glsl/model_loading.vs.glsl",
                "glsl/model_loading.fs.glsl",
                &cube_shader
        );

        light_texture = ap_texture_from_file(
                "redstone_lamp_on.png",
                "mc/tex/minecraft/block/",
                false
        );
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

        demo_update_light();

        return 0;
}

int demo_update_light()
{
        ap_shader_use(light_shader);
        demo_setup_directional_light();
        demo_setup_point_lights();
        ap_shader_set_float(light_shader, "material.shininess", 16.0f);
        demo_setup_spot_light();

        return 0;
}