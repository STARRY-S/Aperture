#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags

#include <GLES3/gl3.h>

#include "mesh.h"
#include "main.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

// private method
int setup_mesh(struct Mesh *pMesh);

/**
 * Mesh Constructor
 * @param pMesh
 * @param pVertices
 * @param iVerticesLength
 * @param pIndices
 * @param iIndicesLength
 * @param pTexture
 * @param iTextureLength
 * @return - GE_Types
 */
int init_mesh(struct Mesh *pMesh, struct Vertex* pVertices, int iVerticesLength,
        unsigned int *pIndices, int iIndicesLength,
        struct Texture *pTexture, int iTextureLength)
{
    if (!pMesh || !pVertices || !pIndices || !pTexture) {
        return GE_ERROR_INVALID_POINTER;
    }

    memset(pMesh, 0, sizeof(struct Mesh));

    struct Vertex *pNewVertex = NULL;
    if (iVerticesLength > 0) {
        pNewVertex = (struct Vertex *) malloc(sizeof(struct Vertex) * iVerticesLength);
        if (pNewVertex == NULL) {
            return GE_ERROR_MALLOC_FAILED;
        }
        memcpy(pNewVertex, pVertices, sizeof(struct Vertex) * iVerticesLength);
    }
    pMesh->iVerticesLength = iVerticesLength;
    pMesh->pVertices = pNewVertex;

    unsigned int *pNewIndices = NULL;
    if (iIndicesLength > 0) {
        pNewIndices = (unsigned int *) malloc(sizeof(unsigned int) * iIndicesLength);
        if (pNewIndices == NULL) {
            return GE_ERROR_MALLOC_FAILED;
        }
        memcpy(pNewIndices, pIndices, sizeof(unsigned int) * iIndicesLength);
    }
    pMesh->iIndicesLength = iIndicesLength;
    pMesh->pIndices = pNewIndices;

    struct Texture *pNewTexture = NULL;
    if (iTextureLength > 0) {
        pNewTexture = (struct Texture *) malloc(sizeof(struct Texture) * iTextureLength);
        if (pNewTexture == NULL) {
            return GE_ERROR_MALLOC_FAILED;
        }
        memcpy(pNewTexture, pTexture, sizeof(struct Texture) * iTextureLength);
    }
    pMesh->iTextureLength = iTextureLength;
    pMesh->pTextures = pNewTexture;

    setup_mesh(pMesh);
    return 0;
}

/**
 * Release data in mesh struct object
 * @param pMesh
 * @return 0 if success
 */
GE_Types free_mesh(struct Mesh *pMesh)
{
    if (pMesh == NULL) {
        return 0;
    }
    if (pMesh->pIndices) {
        free(pMesh->pIndices);
        pMesh->pIndices = NULL;
    }
    pMesh->iIndicesLength = 0;

    if (pMesh->pVertices) {
        free(pMesh->pVertices);
        pMesh->pVertices = NULL;
    }
    pMesh->iVerticesLength = 0;

    if (pMesh->pTextures) {
        free(pMesh->pTextures);
        pMesh->pTextures = NULL;
    }
    pMesh->iTextureLength = 0;
    pMesh->VAO = pMesh->VBO = pMesh->EBO = 0;

    return 0;
}

/**
 * Copy mesh
 * @param pNewMesh - dest, points to an uninitialized mesh
 * @param pOldMesh - from, the mesh to be copied
 * @return GE_Types
 */
int copy_mesh(struct Mesh *pNewMesh, const struct Mesh *pOldMesh)
{
    if (pNewMesh == NULL || pOldMesh == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }

    memset(pNewMesh, 0, sizeof(struct Mesh));

    pNewMesh->iTextureLength = pOldMesh->iTextureLength;
    if (pOldMesh->iTextureLength > 0) {
        pNewMesh->pTextures = malloc(pNewMesh->iTextureLength * sizeof(struct Texture));
        memcpy(pNewMesh->pTextures, pOldMesh->pTextures,
               pNewMesh->iTextureLength * sizeof(struct Texture));
    }

    pNewMesh->iIndicesLength = pOldMesh->iIndicesLength;
    if (pOldMesh->iIndicesLength > 0) {
        pNewMesh->pIndices = malloc(pNewMesh->iIndicesLength * sizeof(unsigned int));
        memcpy(pNewMesh->pIndices, pOldMesh->pIndices,
               pNewMesh->iIndicesLength * sizeof(unsigned int));
    }

    pNewMesh->iVerticesLength = pOldMesh->iVerticesLength;
    if (pOldMesh->iVerticesLength > 0) {
        pNewMesh->pVertices = malloc(pNewMesh->iVerticesLength * sizeof(struct Vertex));
        memcpy(pNewMesh->pVertices, pOldMesh->pVertices,
               pNewMesh->iVerticesLength * sizeof(struct Vertex));
    }

    pNewMesh->VAO = pOldMesh->VAO;
    pNewMesh->VBO = pOldMesh->VBO;
    pNewMesh->EBO = pOldMesh->EBO;

    return 0;
}

/**
 * private, setup mesh, generate GL buffers.
 * @param pMesh
 * @return GE_Types
 */
int setup_mesh(struct Mesh *pMesh)
{
    if (!pMesh) {
        return GE_ERROR_INVALID_POINTER;
    }
    glGenVertexArrays(1, &pMesh->VAO);
    glGenBuffers(1, &pMesh->VBO);
    glGenBuffers(1, &pMesh->EBO);

    glBindVertexArray(pMesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pMesh->VBO);

    glBufferData(GL_ARRAY_BUFFER,pMesh->iVerticesLength * sizeof(struct Vertex),
            pMesh->pVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->iIndicesLength * sizeof(unsigned int),
                 pMesh->pIndices, GL_STATIC_DRAW);

    // 顶点位置 (position)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex), (void*)0);
    // 顶点法线 (normal)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex), (void*)offsetof(struct Vertex, Normal));
    // 顶点纹理坐标 (tex coords)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(struct Vertex), (void*)offsetof(struct Vertex, TexCoords));

//    // vertex tangent
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
//                      sizeof(struct Vertex), (void*)offsetof(struct Vertex, Tangent));
//    // vertex big tangent
//    glEnableVertexAttribArray(4);
//    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
//                      sizeof(struct Vertex), (void*)offsetof(struct Vertex, BigTangent));

    glBindVertexArray(0);
    return 0;
}

/**
 * Draw Mesh
 * @param pMesh - the pointer to the mesh
 * @param shader - shader id
 * @return GE_Types
 */
int draw_mesh(struct Mesh *pMesh, unsigned int shader)
{
    if (pMesh == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }

    // bind appropriate texture
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(int i = 0; i < pMesh->iTextureLength; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture before binding
        // retrieve texture number
        char sNumber[32] = { 0 };
        const char *spName = pMesh->pTextures[i].type;
        if (strcmp(spName, "texture_diffuse") == 0)
            sprintf(sNumber, "%u", diffuseNr++);
        else if (strcmp(spName, "texture_specular") == 0)
            sprintf(sNumber, "%u", specularNr++);
        else if (strcmp(spName, "texture_normal") == 0)
            sprintf(sNumber, "%u", normalNr++);
        else if (strcmp(spName, "texture_height") == 0)
            sprintf(sNumber, "%u", heightNr++);

        char buffer[32];
        sprintf(buffer, "%s%s", spName, sNumber);
        // now set the sampler to the correct texture unit
        GLint iLocation = glGetUniformLocation(shader, buffer);
        glUniform1i(iLocation, i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, pMesh->pTextures[i].id);
    }

    // draw mesh
    glBindVertexArray(pMesh->VAO);
    glDrawElements(GL_TRIANGLES, pMesh->iIndicesLength, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured :)
    glActiveTexture(GL_TEXTURE0);
    return GE_ERROR_SUCCESS;
}
