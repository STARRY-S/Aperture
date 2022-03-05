#ifndef __MESH_H__
#define __MESH_H__
#include "main.h"
#include "texture.h"

struct Mesh {
    // 网格数据
    struct Vertex* pVertices;   // array, use malloc, need free.
    int iVerticesLength;
    unsigned int *pIndices;     // array, use malloc, need free.
    int iIndicesLength;
    struct Texture *pTextures;   // array, use malloc, need free.
    int iTextureLength;
    // 渲染数据
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

int init_mesh(struct Mesh *pMesh, struct Vertex* pVertices, int iVerticesLength,
             unsigned int *pIndices, int iIndicesLength,
             struct Texture *pTexture, int iTextureLength);
GE_Types free_mesh(struct Mesh *pMesh);
int copy_mesh(struct Mesh *pNewMesh, const struct Mesh *pOldMesh);
int draw_mesh(struct Mesh *pMesh, unsigned int shader);

#endif // __MESH_H__