#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include "cglm/cglm.h"

#ifndef DEMO_FONT_PATH
#define DEMO_FONT_PATH "fonts/test.ttf"
#endif

extern unsigned int light_shader, cube_shader, ortho_shader;
extern unsigned int light_texture, VBO, light_cube_VAO;
extern unsigned int model_id, camera_use_id, camera_ids[];
extern vec3 light_positions[];
extern bool spot_light_enabled, material_texture_disabled;

int demo_init();
int demo_render();
int demo_finished();

#endif
