#ifndef AP_VERTEX_H
#define AP_VERTEX_H

#define MAX_BONE_INFLUENCE 4

#include "cglm/cglm.h"

struct AP_Vertex {
        vec3 position;
        vec3 normal;
        vec2 tex_coords;

        vec3 tangent;
        vec3 big_tangent;

        //bone indexes which will influence this vertex
        int bonel_ids[MAX_BONE_INFLUENCE];

        //weights from each bone
        float weights[MAX_BONE_INFLUENCE];
};

#endif // AP_VERTEX_H