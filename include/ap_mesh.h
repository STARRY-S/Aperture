#ifndef AP_MESH_H
#define AP_MESH_H
#include "ap_utils.h"
#include "texture.h"

struct AP_Mesh {
        struct Vertex* vertices;   // array, use malloc, need free.
        int vertices_length;
        unsigned int *indices;     // array, use malloc, need free.
        int indices_length;
        struct Texture *textures;   // array, use malloc, need free.
        int texture_length;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
};

int ap_mesh_init(
        struct AP_Mesh *mesh,
        struct Vertex* vertices,
        int vertices_length,
        unsigned int *indices,
        int indices_length,
        struct Texture *texture,
        int texture_length
);

int ap_mesh_free(struct AP_Mesh *mesh);

int ap_mesh_copy(struct AP_Mesh *mesh_new, const struct AP_Mesh *mesh_old);

int ap_mesh_setup(struct AP_Mesh *mesh);

#endif // AP_MESH_H