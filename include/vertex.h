#ifndef AP_VERTEX_H
#define AP_VERTEX_H

#define MAX_BONE_INFLUENCE 4

struct Vertex { // points
        vec3 Position;
        vec3 Normal;
        vec2 TexCoords;

        vec3 Tangent;
        vec3 BigTangent;
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
};

#endif // AP_VERTEX_H