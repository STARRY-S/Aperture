#include "ap_utils.h"
#include "ap_model.h"
#include "ap_cvector.h"
#include "ap_custom_io.h"
#include "ap_texture.h"
#include "ap_vertex.h"
#include "ap_shader.h"

#ifndef __ANDROID__
#include "glad/glad.h"
#endif

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cfileio.h>
#include <pthread.h>

static struct AP_Vector model_vector = { 0, 0, 0, 0 };
static struct AP_Model *model_using = NULL;

/**
 * Load model from android asset manager.
 * @param model
 * @param path file path
 * @param format can be null or empty string
 * @return AP_Types
 */
int ap_model_load_ptr(struct AP_Model *model, const char *path);

/**
 * Process node
 * @param model
 * @param node
 * @param scene
 * @return AP_Types
 */
int ap_model_process_node(
        struct AP_Model *model,
        struct aiNode *node,
        const struct aiScene *scene
);

/**
 * Push a new texture struct object into model
 * @param model
 * @param texture
 * @return AP_Types
 */
int ap_model_texture_loaded_push_back(
        struct AP_Model *model,
        struct AP_Texture *texture
);

/**
 * Process mesh,
 * @param model
 * @param mesh
 * @param scene
 * @return Points to static mesh struct object, need ap_mesh_free after use.
 */
struct AP_Mesh *ap_model_process_mesh(
        struct AP_Model *model,
        struct aiMesh *mesh,
        const struct aiScene *scene
);

/**
 * Push a new mesh struct object to model
 * @param model
 * @param mesh
 * @return AP_Types
 */
int ap_model_mesh_push_back(struct AP_Model *model, struct AP_Mesh *mesh);

/**
 * checks all material textures of a given type and loads the textures if
 * they're not loaded yet. The required info is returned as a Texture struct.
 * @param model
 * @param mat
 * @param type
 * @param name
 * @return Pointer points to a static vector,
 *         need use ap_vector_free to AP_FREE its data after use.
 */
struct AP_Vector *ap_model_load_material_textures(
    struct AP_Model *model,
    struct aiMaterial *mat,
    enum aiTextureType type,
    const char* name
);

int ap_model_generate(const char *path, unsigned int *model_id)
{
        if (!path || !model_id) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        // initialize vector when first use
        if (model_vector.data == NULL) {
                ap_vector_init(&model_vector, AP_VECTOR_MODEL);
        }

        struct AP_Model model;
        AP_CHECK( ap_model_init_ptr(&model, path, false) );

        model.id = model_vector.length + 1;
        ap_vector_push_back(&model_vector, (const char*) &model);
        *model_id = model.id;
        LOGD("generated model %s id %d", path, *model_id);

        return 0;
}

int ap_model_use(unsigned int model_id)
{
        if (model_id > model_vector.length) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (model_id == 0) {
                model_using = NULL;
                return 0;
        }

        struct AP_Model *tmp_model = (struct AP_Model*) model_vector.data;
        model_using = tmp_model + (model_id - 1);

        return 0;
}

int ap_model_draw()
{
        unsigned int shader_id = ap_get_current_shader();
        if (shader_id == 0) {
                return AP_ERROR_SHADER_NOT_SET;
        }

        if (model_using == NULL) {
                return AP_ERROR_MODEL_NOT_SET;
        }

        ap_model_draw_ptr_shader(model_using, shader_id);

        return 0;
}

int ap_model_free()
{
        model_using = NULL;    // for safety purpose
        struct AP_Model *model_array = (struct AP_Model*) model_vector.data;
        for (int i = 0; i < model_vector.length; ++i) {
                AP_FREE(model_array[i].directory);
                model_array[i].directory = NULL;
                AP_FREE(model_array[i].mesh);
                model_array[i].mesh = NULL;
                AP_FREE(model_array[i].texture);
                model_array[i].texture = NULL;
        }
        ap_vector_free(&model_vector);
        LOGD("AP_FREE models");

        return 0;
}

int ap_model_init_ptr(struct AP_Model *model, const char *path, bool gamma)
{
        if (model == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        memset(model, 0, sizeof(struct AP_Model));

        int dir_char_location = 0;
        for (int i = 0; i < strlen(path); ++i) {
                if (path[i] == '/') {
                dir_char_location = i;
                }
        }
        if (dir_char_location >= 0) {
                char *dir_path = AP_MALLOC(sizeof(char) * (dir_char_location + 2));
                memcpy(dir_path, path, (dir_char_location + 1) * sizeof(char));
                dir_path[dir_char_location + 1] = '\0';
                model->directory = dir_path;
        }

        return ap_model_load_ptr(model, path);
}

int ap_model_load_ptr(struct AP_Model *model, const char *path)
{
        if (model == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        // Start the import on the given file with some example postprocessing
        // Usually if speed is not the most important aspect for you
        // you'll probably to request more postprocessing
        // than we do in this example.

        // custom file io for assimp
        struct aiFileIO fileIo;
        fileIo.CloseProc = ap_custom_file_close_proc;
        fileIo.OpenProc = ap_custom_file_open_proc;
        fileIo.UserData = NULL;

        const struct aiScene* scene = aiImportFileEx(
                path,
                aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace,
                &fileIo
        );

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
           || !scene->mRootNode)
        {
                LOGE("Assimp import failed: \n%s", aiGetErrorString());
                return AP_ERROR_ASSIMP_IMPORT_FAILED;
        }

        // Now we can access the file's contents
        AP_CHECK( ap_model_process_node(model, scene->mRootNode, scene) );

        // We're done. Release all resources associated with this import
        aiReleaseImport(scene);

        return AP_ERROR_SUCCESS;
}

int ap_model_process_node(
        struct AP_Model *model,
        struct aiNode *node,
        const struct aiScene *scene)
{
        if (model == NULL || node == NULL || scene == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
                // the node object only contains indices to index the actual
                // objects in the scene. the scene contains all the data,
                // node is just to keep stuff organized
                // (like relations between nodes).
                struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                struct AP_Mesh *mesh_new =
                        ap_model_process_mesh(model, mesh, scene);
                ap_model_mesh_push_back(model, mesh_new);
                ap_mesh_free(mesh_new);
        }

        // after we've processed all of the meshes (if any)
        // we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
                ap_model_process_node(model, node->mChildren[i], scene);
        }

        return 0;
}

struct AP_Mesh *ap_model_process_mesh(struct AP_Model *model,
                        struct aiMesh *mesh,
                        const struct aiScene *scene)
{
        if (model == NULL || mesh == NULL || scene == NULL) {
                return NULL;
        }
        // data to fill
        struct AP_Vector vec_vertices;
        struct AP_Vector vec_indices;
        struct AP_Vector vec_textures;

        ap_vector_init(&vec_vertices, AP_VECTOR_VERTEX);
        ap_vector_init(&vec_indices, AP_VECTOR_UINT);
        ap_vector_init(&vec_textures, AP_VECTOR_TEXTURE);

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
                struct AP_Vertex vertex;
                // positions
                vertex.position[0] = mesh->mVertices[i].x;
                vertex.position[1] = mesh->mVertices[i].y;
                vertex.position[2] = mesh->mVertices[i].z;

                // normals
                if (mesh->mNormals != NULL) {
                        vertex.normal[0] = mesh->mNormals[i].x;
                        vertex.normal[1] = mesh->mNormals[i].y;
                        vertex.normal[2] = mesh->mNormals[i].z;
                }
                // texture coordinates
                // does the mesh contain texture coordinates?
                if(mesh->mTextureCoords[0]) {
                        /** a vertex can contain up to 8 different texture coordinates.
                         * We thus make the assumption that we won't
                         * use models where a vertex can have multiple texture coordinates
                         * so we always take the first set (0).
                         */
                        vertex.tex_coords[0] = mesh->mTextureCoords[0][i].x;
                        vertex.tex_coords[1] = mesh->mTextureCoords[0][i].y;

                        // tangent
                        vertex.tangent[0] = mesh->mTangents[i].x;
                        vertex.tangent[1] = mesh->mTangents[i].y;
                        vertex.tangent[2] = mesh->mTangents[i].z;

                        // big tangent
                        vertex.big_tangent[0] = mesh->mBitangents[i].x;
                        vertex.big_tangent[1] = mesh->mBitangents[i].y;
                        vertex.big_tangent[2] = mesh->mBitangents[i].z;
                } else {
                        vertex.tex_coords[0] = 0.0f;
                        vertex.tex_coords[1] = 0.0f;
                }

                // vertices.push_back(vertex);
                ap_vector_push_back(&vec_vertices, (const char *) &vertex);
        }
        // now wak through each of the mesh's faces
        // (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
                struct aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for(unsigned int j = 0; j < face.mNumIndices; j++) {
                        ap_vector_push_back(&vec_indices, (const char *) &face.mIndices[j]);
                }
        }
        // process materials
        struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        /** we assume a convention for sampler names in the shaders. Each diffuse texture should be
         * named as 'texture_diffuseN' where N is a sequential number ranging
         * from 1 to MAX_SAMPLER_NUMBER.
         *
         * Same applies to other texture as the following list summarizes:
         * diffuse: texture_diffuseN
         * specular: texture_specularN
         * normal: texture_normalN
         */

        // 1. diffuse maps
        size_t size = sizeof(struct AP_Texture);
        struct AP_Vector *vec_diffuse_ptr = NULL;
        vec_diffuse_ptr = ap_model_load_material_textures(
                model, material, aiTextureType_DIFFUSE, "texture_diffuse"
        );
        if (vec_diffuse_ptr->length > 0) {
                ap_vector_insert_back(
                        &vec_textures,
                        vec_diffuse_ptr->data,
                        size * vec_diffuse_ptr->length
                );
        }
        ap_vector_free(vec_diffuse_ptr);

        // 2. specular maps
        struct AP_Vector *vec_specular_ptr = NULL;
        vec_specular_ptr = ap_model_load_material_textures(
                model, material, aiTextureType_SPECULAR, "texture_specular"
        );
        if (vec_specular_ptr->length > 0) {
                ap_vector_insert_back(
                        &vec_textures,
                        vec_specular_ptr->data,
                        size * vec_specular_ptr->length
                );
        }
        ap_vector_free(vec_specular_ptr);

        // 3. normal maps
        struct AP_Vector *vec_normal_ptr = NULL;
        vec_normal_ptr = ap_model_load_material_textures(
                model, material, aiTextureType_HEIGHT, "texture_normal"
        );
        if (vec_normal_ptr->length > 0) {
                ap_vector_insert_back(
                        &vec_textures,
                        vec_normal_ptr->data,
                        size * vec_normal_ptr->length
                );
        }
        ap_vector_free(vec_normal_ptr);

        // 4. height maps
        struct AP_Vector *vec_height_ptr;
        vec_height_ptr = ap_model_load_material_textures(
                model, material, aiTextureType_AMBIENT, "texture_height"
        );
        if (vec_height_ptr->length > 0) {
                ap_vector_insert_back(
                        &vec_textures,
                        vec_height_ptr->data,
                        size * vec_height_ptr->length
                );
        }
        ap_vector_free(vec_height_ptr);

        // return a mesh object created from the extracted mesh data
        static struct AP_Mesh mesh_buffer;
        ap_mesh_init(
                &mesh_buffer,
                (struct AP_Vertex *) vec_vertices.data,
                vec_vertices.length,
                (unsigned int *) vec_indices.data,
                vec_indices.length,
                (struct AP_Texture *) vec_textures.data,
                vec_textures.length
        );
        ap_vector_free(&vec_vertices);
        ap_vector_free(&vec_indices);
        ap_vector_free(&vec_textures);

        return &mesh_buffer;
}

struct AP_Vector *ap_model_load_material_textures(
        struct AP_Model *model,
        struct aiMaterial *mat,
        enum aiTextureType type,
        const char* name)
{
        static struct AP_Vector vec_texture;
        ap_vector_init(&vec_texture, AP_VECTOR_TEXTURE);

        GLuint mat_texture_count = aiGetMaterialTextureCount(mat, type);
        for (GLuint i = 0; i < mat_texture_count; i++)
        {
                struct aiString str;
                aiGetMaterialTexture(
                        mat, type, i, &str,
                        NULL, NULL, NULL, NULL, NULL, NULL
                );
                struct AP_Texture *ptr = NULL;
                ptr = ap_texture_get_ptr_from_path(str.data);
                if (ptr == NULL) {
                        GLuint texture_id = 0;
                        ap_texture_generate(&texture_id, name,
                                str.data, model->directory, false);
                        ptr = ap_texture_get_ptr(texture_id);
                }
                if (ptr) {
                        ap_vector_push_back(&vec_texture, (const char*) ptr);
                        ap_model_texture_loaded_push_back(model, ptr);
                }
        }
        return &vec_texture;
}

int ap_model_texture_loaded_push_back(
        struct AP_Model *model,
        struct AP_Texture *texture)
{
        if (model == NULL || texture == NULL) {
                LOGD("model %p, texture %p", model, texture);
                return AP_ERROR_INVALID_PARAMETER;
        }

        // add a new texture struct object into model
        model->texture = AP_REALLOC(model->texture,
                sizeof(struct AP_Texture) * (model->texture_length + 1));
        if (model->texture == NULL) {
                LOGE("Realloc error.");
                return AP_ERROR_MALLOC_FAILED;
        }
        struct AP_Texture *texture_new =
                model->texture + (model->texture_length);
        model->texture_length++;
        ap_texture_init(texture_new);
        ap_texture_set_type(texture_new, texture->type);
        ap_texture_set_path(texture_new, texture->path);
        texture_new->id = texture->id;

        return 0;
}

int ap_model_mesh_push_back(struct AP_Model *model, struct AP_Mesh *mesh)
{
        if (model == NULL || mesh == NULL) {
                LOGE("Model mesh push back param error.");
                return AP_ERROR_INVALID_POINTER;
        }

        // add a new mesh struct object into model
        model->mesh = AP_REALLOC(
                model->mesh,
                sizeof(struct AP_Mesh) * (model->mesh_length + 1)
        );
        if (model->mesh == NULL) {
                LOGE("Realloc error.");
                return AP_ERROR_MALLOC_FAILED;
        }
        struct AP_Mesh *mesh_ptr = &model->mesh[model->mesh_length];
        model->mesh_length++;
        // copy old mesh data memory to new mesh
        ap_mesh_copy(mesh_ptr, mesh);

        return 0;
}

int ap_model_draw_ptr_shader(struct AP_Model *model, unsigned int shader)
{
        if (model == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        for (int i = 0; i < model->mesh_length; ++i) {
                ap_mesh_draw(&model->mesh[i], shader);
        }

        return 0;
}