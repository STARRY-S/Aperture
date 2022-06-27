#ifndef AP_MESH_H
#define AP_MESH_H
#include "ap_utils.h"
#include "ap_texture.h"
#include <stdbool.h>

struct AP_Mesh {
        struct AP_Vertex* vertices;
        int vertices_length;
        unsigned int *indices;
        int indices_length;
        struct AP_Texture *textures;
        int texture_length;

        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
};

/**
 * @brief Initialize a mesh struct object by giving its parameters
 *
 * @param mesh
 * @param vertices pointer points to AP_Vertex
 * @param vertices_length length of vertices
 * @param indices pointer points to indices
 * @param indices_length length of indices
 * @param texture pointer points to AP_Texture
 * @param texture_length length of textures
 * @return int
 */
int ap_mesh_init(
        struct AP_Mesh *mesh,
        struct AP_Vertex* vertices,
        int vertices_length,
        unsigned int *indices,
        int indices_length,
        struct AP_Texture *texture,
        int texture_length
);

/**
 * @brief Release all pointers in a mesh
 * this function won't release VAO/VBP/EBO of the mesh
 * need to release these buffers on GPU manually if needed.
 *
 * @param mesh
 * @return int
 */
int ap_mesh_free(struct AP_Mesh *mesh);

/**
 * @brief Copy data from old mesh to a new mesh object
 * this function won't create new VAO/VBO/EBO for new mesh object,
 *
 * @param mesh_new
 * @param mesh_old
 * @return int
 */
int ap_mesh_copy(struct AP_Mesh *mesh_new, const struct AP_Mesh *mesh_old);

/**
 * @brief Draw mesh and its texture(s)
 *
 * @param mesh pointer points to AP_Mesh
 * @param shader shader program ID
 * @return int AP_Types
 */
int ap_mesh_draw(struct AP_Mesh *mesh, unsigned int shader);

#endif // AP_MESH_H