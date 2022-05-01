#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags

#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_vertex.h"

/**
 * private, setup mesh, generate GL buffers.
 * @param mesh
 * @return AP_Types
 */
int ap_mesh_setup(struct AP_Mesh *mesh);

int ap_mesh_init(
        struct AP_Mesh *mesh,
        struct AP_Vertex* vertices,
        int vertices_length,
        unsigned int *indices,
        int indices_length,
        struct AP_Texture *texture,
        int texture_length)
{
        if (!mesh || !vertices || !indices || !texture) {
                return AP_ERROR_INVALID_POINTER;
        }

        memset(mesh, 0, sizeof(struct AP_Mesh));

        struct AP_Vertex *vertex_new = NULL;
        if (vertices_length > 0) {
                vertex_new = (struct AP_Vertex *) AP_MALLOC(
                                sizeof(struct AP_Vertex) * vertices_length);
                if (vertex_new == NULL) {
                        LOGE("MALLOC FAILED");
                        return AP_ERROR_MALLOC_FAILED;
                }
                memcpy(vertex_new, vertices,
                        sizeof(struct AP_Vertex) * vertices_length);
        }
        mesh->vertices_length = vertices_length;
        mesh->vertices = vertex_new;

        unsigned int *indices_new = NULL;
        if (indices_length > 0) {
                indices_new = (unsigned int *) AP_MALLOC(
                        sizeof(unsigned int) * indices_length);
                if (indices_new == NULL) {
                        return AP_ERROR_MALLOC_FAILED;
                }
                memcpy(indices_new, indices,
                        sizeof(unsigned int) * indices_length);
        }
        mesh->indices_length = indices_length;
        mesh->indices = indices_new;

        struct AP_Texture *texture_new = NULL;
        if (texture_length > 0) {
                texture_new = (struct AP_Texture *) AP_MALLOC(
                        sizeof(struct AP_Texture) * texture_length);
                if (texture_new == NULL) {
                        return AP_ERROR_MALLOC_FAILED;
                }
                memcpy(texture_new, texture,
                        sizeof(struct AP_Texture) * texture_length);
        }
        mesh->texture_length = texture_length;
        mesh->textures = texture_new;

        ap_mesh_setup(mesh);
        return 0;
}

int ap_mesh_free(struct AP_Mesh *mesh)
{
        if (mesh == NULL) {
                return 0;
        }
        if (mesh->indices) {
                AP_FREE(mesh->indices);
                mesh->indices = NULL;
        }
        mesh->indices_length = 0;

        if (mesh->vertices) {
                AP_FREE(mesh->vertices);
                mesh->vertices = NULL;
        }
        mesh->vertices_length = 0;

        if (mesh->textures) {
                AP_FREE(mesh->textures);
                mesh->textures = NULL;
        }
        mesh->texture_length = 0;
        mesh->VAO = mesh->VBO = mesh->EBO = 0;

        return 0;
}

int ap_mesh_copy(struct AP_Mesh *mesh_new, const struct AP_Mesh *mesh_old)
{
        if (mesh_new == NULL || mesh_old == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        memset(mesh_new, 0, sizeof(struct AP_Mesh));

        mesh_new->texture_length = mesh_old->texture_length;
        if (mesh_old->texture_length > 0) {
                mesh_new->textures = AP_MALLOC(
                        mesh_new->texture_length * sizeof(struct AP_Texture));
                memcpy(mesh_new->textures, mesh_old->textures,
                        mesh_new->texture_length * sizeof(struct AP_Texture));
        }

        mesh_new->indices_length = mesh_old->indices_length;
        if (mesh_old->indices_length > 0) {
                mesh_new->indices = AP_MALLOC(
                        mesh_new->indices_length * sizeof(unsigned int));
                memcpy(mesh_new->indices, mesh_old->indices,
                        mesh_new->indices_length * sizeof(unsigned int));
        }

        mesh_new->vertices_length = mesh_old->vertices_length;
        if (mesh_old->vertices_length > 0) {
                mesh_new->vertices = AP_MALLOC(
                        mesh_new->vertices_length * sizeof(struct AP_Vertex));
                memcpy(mesh_new->vertices, mesh_old->vertices,
                        mesh_new->vertices_length * sizeof(struct AP_Vertex));
        }

        mesh_new->VAO = mesh_old->VAO;
        mesh_new->VBO = mesh_old->VBO;
        mesh_new->EBO = mesh_old->EBO;

        return 0;
}

int ap_mesh_setup(struct AP_Mesh *mesh)
{
        if (!mesh) {
                return AP_ERROR_INVALID_POINTER;
        }
        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);
        glGenBuffers(1, &mesh->EBO);

        glBindVertexArray(mesh->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        glBufferData(
                GL_ARRAY_BUFFER,
                mesh->vertices_length * sizeof(struct AP_Vertex),
                mesh->vertices,
                GL_STATIC_DRAW
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                mesh->indices_length * sizeof(unsigned int),
                mesh->indices,
                GL_STATIC_DRAW
        );

        // position (顶点位置)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(struct AP_Vertex),
                (void*)0
        );
        // normal (顶点法线)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(struct AP_Vertex),
                (void*) offsetof(struct AP_Vertex, normal)
        );
        // texture coords (顶点纹理坐标)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
                2,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(struct AP_Vertex),
                (void*) offsetof(struct AP_Vertex, tex_coords)
        );

        // // vertex tangent
        // glEnableVertexAttribArray(3);
        // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
        //                 sizeof(struct AP_Vertex),
        //                 (void*)offsetof(struct AP_Vertex, tangent));
        // // vertex big tangent
        // glEnableVertexAttribArray(4);
        // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
        //                 sizeof(struct AP_Vertex),
        //                 (void*)offsetof(struct AP_Vertex, big_tangent));

        glBindVertexArray(0);
        return 0;
}

int ap_mesh_draw(struct AP_Mesh *mesh, unsigned int shader)
{
        if (mesh == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        // bind appropriate texture
        unsigned int diffuse_nr  = 0;
        unsigned int specular_nr = 0;
        unsigned int normal_nr   = 0;
        unsigned int height_nr   = 0;
        static char buffer[AP_DEFAULT_BUFFER_SIZE];

        for(int i = 0; i < mesh->texture_length
                && i < AP_TEXTURE_UNIT_MAX_NUM; i++)
        {
                // active proper texture before binding
                glActiveTexture(GL_TEXTURE0 + i);
                // retrieve texture number
                int texture_num = 0;
                int ap_type = mesh->textures[i].type;
                if (ap_type == AP_TEXTURE_TYPE_DIFFUSE) {
                        texture_num = diffuse_nr++;
                } else if (ap_type == AP_TEXTURE_TYPE_SPECULAR) {
                        texture_num = specular_nr++;
                } else if (ap_type == AP_TEXTURE_TYPE_NORMAL) {
                        texture_num = normal_nr++;
                } else if (ap_type == AP_TEXTURE_TYPE_HEIGHT) {
                        texture_num = height_nr++;
                }
                const char* name = ap_texture_type_2_str(ap_type);
                sprintf(buffer, name, texture_num);
                // now set the sampler to the correct texture unit
                int location = glGetUniformLocation(shader, buffer);
                glUniform1i(location, i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
        }

        // draw mesh
        glBindVertexArray(mesh->VAO);
        glDrawElements(GL_TRIANGLES, mesh->indices_length, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        return AP_ERROR_SUCCESS;
}
