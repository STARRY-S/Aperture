#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include "cglm/cglm.h"

extern unsigned int light_shader, cube_shader;
extern unsigned int light_texture, VBO, light_cube_VAO;
extern unsigned int model_id, camera_use_id, camera_ids[];
extern vec3 light_positions[];
extern bool spot_light_enabled;

int demo_init();
int demo_render();
int demo_finished();

#endif
