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
        struct AP_Mesh *pMesh,
        struct Vertex* vertices,
        int vertices_length,
        unsigned int *indices,
        int indices_length,
        struct Texture *texture,
        int texture_length
);
int ap_mesh_free(struct AP_Mesh *pMesh);

int ap_mesh_copy(struct AP_Mesh *pNewMesh, const struct AP_Mesh *pOldMesh);

int ap_mesh_draw(struct AP_Mesh *pMesh, unsigned int shader);

#endif // AP_MESH_H