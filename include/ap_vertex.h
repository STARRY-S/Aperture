#ifndef AP_VERTEX_H
#define AP_VERTEX_H

#define MAX_BONE_INFLUENCE 4

#include "cglm/cglm.h"

struct AP_Vertex {
        vec3 Position;
        vec3 Normal;
        vec2 TexCoords;

        vec3 Tangent;
        vec3 BigTangent;

        //bone indexes which will influence this vertex
        int bonel_ids[MAX_BONE_INFLUENCE];

        //weights from each bone
        float weights[MAX_BONE_INFLUENCE];
};

#endif // AP_VERTEX_H