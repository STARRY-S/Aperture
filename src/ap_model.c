#include "ap_utils.h"
#include "ap_model.h"
#include "ap_cvector.h"
#include "ap_custom_io.h"
#include "ap_texture.h"
#include "ap_vertex.h"

#include <GLES3/gl3.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cfileio.h>
#include <stb_image.h>

/**
 * Load model from android asset manager.
 * @param model
 * @param path file path
 * @param format can be null or empty string
 * @return AP_Types
 */
int ap_model_load(struct AP_Model *model, const char *path);

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
 * checks all material textures of a given type and loads the textures
 * if they're not loaded yet. The required info is returned as a Texture struct.
 * @param model
 * @param mat
 * @param type
 * @param name
 * @return Pointer points to a static vector,
 *         need use ap_vector_free to free its data after use.
 */
struct AP_Vector *ap_model_load_material_textures(
    struct AP_Model *model,
    struct aiMaterial *mat,
    enum aiTextureType type,
    const char* name
);

int ap_model_init(struct AP_Model *model, const char *path, bool gamma)
{
        if (model == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        int iDirCharLocation = 0;
        for (int i = 0; i < strlen(path); ++i) {
                if (path[i] == '/') {
                iDirCharLocation = i;
                }
        }
        if (iDirCharLocation >= 0) {
                char *pDirPath = malloc(sizeof(char) * (iDirCharLocation + 2));
                memcpy(pDirPath, path, (iDirCharLocation + 1) * sizeof(char));
                pDirPath[iDirCharLocation + 1] = '\0';
                model->directory = pDirPath;
        }

        return ap_model_load(model, path);
}

int ap_model_load(struct AP_Model *model, const char *path)
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
                LOGE("Assimp import failed: \n%s\n", aiGetErrorString());
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
                struct AP_Mesh *pNewMesh =
                        ap_model_process_mesh(model, mesh, scene);
                ap_model_mesh_push_back(model, pNewMesh);
                ap_mesh_free(pNewMesh);
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
        struct AP_Vector vecVertices;
        struct AP_Vector vecIndices;
        struct AP_Vector vecTextures;

        ap_vector_init(&vecVertices, AP_VECTOR_VERTEX);
        ap_vector_init(&vecIndices, AP_VECTOR_UINT);
        ap_vector_init(&vecTextures, AP_VECTOR_TEXTURE);

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
                struct AP_Vertex vertex;
                // positions
                vertex.Position[0] = mesh->mVertices[i].x;
                vertex.Position[1] = mesh->mVertices[i].y;
                vertex.Position[2] = mesh->mVertices[i].z;

                // normals
                if (mesh->mNormals != NULL) {
                vertex.Normal[0] = mesh->mNormals[i].x;
                vertex.Normal[1] = mesh->mNormals[i].y;
                vertex.Normal[2] = mesh->mNormals[i].z;
                }
                // texture coordinates
                // does the mesh contain texture coordinates?
                if(mesh->mTextureCoords[0]) {
                /** a vertex can contain up to 8 different texture coordinates.
                 * We thus make the assumption that we won't
                 * use models where a vertex can have multiple texture coordinates
                 * so we always take the first set (0).
                 */
                vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;

                // tangent
                vertex.Tangent[0] = mesh->mTangents[i].x;
                vertex.Tangent[1] = mesh->mTangents[i].y;
                vertex.Tangent[2] = mesh->mTangents[i].z;

                // big tangent
                vertex.BigTangent[0] = mesh->mBitangents[i].x;
                vertex.BigTangent[1] = mesh->mBitangents[i].y;
                vertex.BigTangent[2] = mesh->mBitangents[i].z;
                } else {
                vertex.TexCoords[0] = 0.0f;
                vertex.TexCoords[1] = 0.0f;
                }

                // vertices.push_back(vertex);
                ap_vector_push_back(&vecVertices, (const char *) &vertex);
        }
        // now wak through each of the mesh's faces
        // (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
                struct aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for(unsigned int j = 0; j < face.mNumIndices; j++) {
                ap_vector_push_back(&vecIndices, (const char *) &face.mIndices[j]);
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
        struct AP_Vector *pVecDiffuseMaps = NULL;
        pVecDiffuseMaps = ap_model_load_material_textures(
                model, material, aiTextureType_DIFFUSE, "texture_diffuse"
        );
        ap_vector_insert_back(
                &vecTextures,
                pVecDiffuseMaps->data,
                size * pVecDiffuseMaps->length
        );
        ap_vector_free(pVecDiffuseMaps);

        // 2. specular maps
        struct AP_Vector *pVecSpecularMaps = NULL;
        pVecSpecularMaps = ap_model_load_material_textures(
                model, material, aiTextureType_SPECULAR, "texture_specular"
        );
        ap_vector_insert_back(
                &vecTextures,
                pVecSpecularMaps->data,
                size * pVecSpecularMaps->length
        );
        ap_vector_free(pVecSpecularMaps);

        // 3. normal maps
        struct AP_Vector *pVecNormalMaps = NULL;
        pVecNormalMaps = ap_model_load_material_textures(
                model, material, aiTextureType_HEIGHT, "texture_normal"
        );
        ap_vector_insert_back(
                &vecTextures,
                pVecNormalMaps->data,
                size * pVecNormalMaps->length
        );
        ap_vector_free(pVecNormalMaps);

        // 4. height maps
        struct AP_Vector *pVecHeightMaps;
        pVecHeightMaps = ap_model_load_material_textures(
                model, material, aiTextureType_AMBIENT, "texture_height"
        );
        ap_vector_insert_back(
                &vecTextures,
                pVecHeightMaps->data,
                size * pVecHeightMaps->length
        );
        ap_vector_free(pVecHeightMaps);

        // return a mesh object created from the extracted mesh data
        static struct AP_Mesh sMeshBuffer;
        ap_mesh_init(
                &sMeshBuffer,
                (struct AP_Vertex *) vecVertices.data,
                vecVertices.length,
                (unsigned int *) vecIndices.data,
                vecIndices.length,
                (struct AP_Texture *) vecTextures.data,
                vecTextures.length
        );
        ap_vector_free(&vecVertices);
        ap_vector_free(&vecIndices);
        ap_vector_free(&vecTextures);

        return &sMeshBuffer;
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
                // check if texture was loaded before and if so,
                // continue to next iteration: skip loading a new texture
                bool skip = false;
                for(GLuint j = 0; j < model->texture_length; j++)
                {
                        // skip the texture already loaded
                        if(strcmp(model->texture[j].path, str.data) == 0)
                        {
                                ap_vector_push_back(
                                        &vec_texture,
                                        (const char *) &model->texture[j]
                                );
                                skip = true;
                                break;
                        }
                }
                if(!skip)
                {
                        struct AP_Texture texture;
                        memset(&texture, 0, sizeof(texture));
                        texture.id = ap_texture_from_file(
                                str.data,
                                model->directory,
                                false
                        );

                        ap_texture_set_path(&texture, str.data);
                        ap_texture_set_type(&texture, name);

                        // store it as texture loaded for entire model,
                        // ensure we won't unnecessary load duplicate textures.
                        ap_vector_push_back(
                                &vec_texture,
                                (const char*) &texture
                        );
                        AP_CHECK(
                                ap_model_texture_loaded_push_back(
                                        model,
                                        &texture
                                )
                        );
                }
        }
        return &vec_texture;
}

int ap_model_texture_loaded_push_back(
        struct AP_Model *model,
        struct AP_Texture *texture)
{
        if (model == NULL || texture == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        // add a new texture struct object into model
        model->texture = realloc(model->texture,
                sizeof(struct AP_Texture) * (model->texture_length + 1));
        if (model->texture == NULL) {
                LOGE("Realloc error.");
                return AP_ERROR_MALLOC_FAILED;
        }
        struct AP_Texture *pNewTexture =
                model->texture + (model->texture_length);
        model->texture_length++;
        ap_texture_init(pNewTexture);
        ap_texture_set_type(pNewTexture, texture->type);
        ap_texture_set_path(pNewTexture, texture->path);
        pNewTexture->id = texture->id;

        return 0;
}

int ap_model_mesh_push_back(struct AP_Model *model, struct AP_Mesh *mesh)
{
        if (model == NULL || mesh == NULL) {
                LOGE("Model mesh push back param error.");
                return AP_ERROR_INVALID_POINTER;
        }

        // add a new mesh struct object into model
        model->mesh = realloc(model->mesh,
                                sizeof(struct AP_Mesh) * (model->mesh_length + 1));
        if (model->mesh == NULL) {
                LOGE("Realloc error.");
                return AP_ERROR_MALLOC_FAILED;
        }
        struct AP_Mesh *pNewMesh = &model->mesh[model->mesh_length];
        model->mesh_length++;
        // copy old mesh data memory to new mesh
        ap_mesh_copy(pNewMesh, mesh);

        return 0;
}

int ap_model_draw_ptr(struct AP_Model *model, unsigned int shader)
{
        if (model == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        for (int i = 0; i < model->mesh_length; ++i) {
                ap_mesh_draw(&model->mesh[i], shader);
        }

        return 0;
}