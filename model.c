#include "main.h"
#include "model.h"
#include "cvector.h"
#include "custom_io.h"
#include "texture.h"
#include "vertex.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags
#include <GLES3/gl3.h>
#include <stb_image.h>
#include <assimp/cfileio.h>

int load_model(struct Model *pModel, const char *path);
int process_node(struct Model *pModel, struct aiNode *node, const struct aiScene *scene);
int model_texture_loaded_push_back(struct Model *pModel, struct Texture *pTexture);
struct Mesh *process_mesh(struct Model *pModel, struct aiMesh *mesh, const struct aiScene *scene);
int model_mesh_push_back(struct Model *pModel, struct Mesh *pMesh);

struct Vector *load_material_textures(
    struct Model *pModel, struct aiMaterial *mat, enum aiTextureType type, const char* typeName);

/**
 * Model constructor
 * @param path - file path
 * @param format - file format, or null
 * @param gamma - default to null
 * @return GE_Types
 */
int init_model(struct Model *pModel, const char *path, bool gamma)
{
    if (pModel == NULL) {
        return GE_ERROR_INVALID_POINTER;
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
        pModel->pDirectory = pDirPath;
    }

    return load_model(pModel, path);
}

/**
 * Load model from android asset manager.
 * @param pModel
 * @param path - file path
 * @param format - can be null or empty string
 * @return GE_Types
 */
int load_model(struct Model *pModel, const char *path)
{
    if (pModel == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }

    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.

    // custom file io for assimp
    struct aiFileIO fileIo;
    fileIo.CloseProc = customFileCloseProc;
    fileIo.OpenProc = customFileOpenProc;
    fileIo.UserData = NULL;

    const struct aiScene* scene = aiImportFileEx(
            path,aiProcess_Triangulate | aiProcess_GenSmoothNormals
            | aiProcess_FlipUVs | aiProcess_CalcTangentSpace,
            &fileIo);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOGE("Assimp import failed: \n%s", aiGetErrorString());
        return GE_ERROR_ASSIMP_IMPORT_FAILED;
    }

    // Now we can access the file's contents
    GE_CHECK(process_node(pModel, scene->mRootNode, scene));

    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);

    return GE_ERROR_SUCCESS;
}

/**
 * Process node
 * @param pModel
 * @param node
 * @param scene
 * @return GE_Types
 */
int process_node(struct Model *pModel, struct aiNode *node, const struct aiScene *scene)
{
    if (pModel == NULL || node == NULL || scene == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized
        // (like relations between nodes).
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        struct Mesh *pNewMesh = process_mesh(pModel, mesh, scene);
        model_mesh_push_back(pModel, pNewMesh);
        free_mesh(pNewMesh);
    }

    // after we've processed all of the meshes (if any)
    // we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(pModel, node->mChildren[i], scene);
    }

    return 0;
}

/**
 * Process mesh,
 * @param pModel
 * @param mesh
 * @param scene
 * @return Pointer to a static mesh struct object, need call free_mesh after use.
 */
struct Mesh *process_mesh(struct Model *pModel, struct aiMesh *mesh, const struct aiScene *scene)
{
    if (pModel == NULL || mesh == NULL || scene == NULL) {
        return NULL;
    }
    // data to fill
    struct Vector vecVertices;
    struct Vector vecIndices;
    struct Vector vecTextures;

    init_vector(&vecVertices, GE_VECTOR_VERTEX);
    init_vector(&vecIndices, GE_VECTOR_UINT);
    init_vector(&vecTextures, GE_VECTOR_TEXTURE);

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        struct Vertex vertex;
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
        vector_push_back(&vecVertices, (const char *) &vertex);
    }
    // now wak through each of the mesh's faces
    // (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        struct aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            vector_push_back(&vecIndices, (const char *) &face.mIndices[j]);
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
    size_t size = sizeof(struct Texture);
    struct Vector *pVecDiffuseMaps = NULL;
    pVecDiffuseMaps = load_material_textures(
            pModel, material, aiTextureType_DIFFUSE, "texture_diffuse"
    );
    vector_insert_back(&vecTextures, pVecDiffuseMaps->data, size * pVecDiffuseMaps->length);
    free_vector(pVecDiffuseMaps);

    // 2. specular maps
    struct Vector *pVecSpecularMaps = NULL;
    pVecSpecularMaps = load_material_textures(
            pModel, material, aiTextureType_SPECULAR, "texture_specular"
    );
    vector_insert_back(&vecTextures, pVecSpecularMaps->data, size * pVecSpecularMaps->length);
    free_vector(pVecSpecularMaps);

    // 3. normal maps
    struct Vector *pVecNormalMaps = NULL;
    pVecNormalMaps = load_material_textures(
            pModel, material, aiTextureType_HEIGHT, "texture_normal"
    );
    vector_insert_back(&vecTextures, pVecNormalMaps->data, size * pVecNormalMaps->length);
    free_vector(pVecNormalMaps);

    // 4. height maps
    struct Vector *pVecHeightMaps;
    pVecHeightMaps = load_material_textures(
            pModel, material, aiTextureType_AMBIENT, "texture_height"
    );
    vector_insert_back(&vecTextures, pVecHeightMaps->data, size * pVecHeightMaps->length);
    free_vector(pVecHeightMaps);

    // return a mesh object created from the extracted mesh data
    static struct Mesh sMeshBuffer;
    init_mesh(&sMeshBuffer, (struct Vertex *) vecVertices.data, vecVertices.length,
              (unsigned int *) vecIndices.data, vecIndices.length,
              (struct Texture *) vecTextures.data, vecTextures.length);
    free_vector(&vecVertices);
    free_vector(&vecIndices);
    free_vector(&vecTextures);

    return &sMeshBuffer;
}

/**
 * checks all material textures of a given type and loads the textures if they're not loaded yet.
 * the required info is returned as a Texture struct.
 * @param pModel
 * @param mat
 * @param type
 * @param typeName
 * @return Pointer points to a static vector, need use free_vector to free its data after use.
 */
struct Vector *load_material_textures(struct Model *pModel, struct aiMaterial *mat,
        enum aiTextureType type, const char* typeName)
{
    static struct Vector sVecTextures;
    init_vector(&sVecTextures, GE_VECTOR_TEXTURE);
    struct Vector *pVecTextures = &sVecTextures;
    if (pVecTextures == NULL) {
        return NULL;
    }

    unsigned int uMaterialTextureCount = aiGetMaterialTextureCount(mat, type);
    for(unsigned int i = 0; i < uMaterialTextureCount; i++)
    {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str,
             NULL, NULL, NULL, NULL, NULL, NULL);
        // check if texture was loaded before and if so,
        // continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < pModel->iTextureLoadedLength; j++)
        {
            // a texture with the same filepath has already been loaded,
            // continue to next one. (optimization)
            if(strcmp(pModel->pTextureLoaded[j].path, str.data) == 0)
            {
                vector_push_back(pVecTextures, (const char *) &pModel->pTextureLoaded[j]);
                skip = true;
                break;
            }
        }
        if(!skip)
        {
            // if texture hasn't been loaded already, load it
            struct Texture texture;
            memset(&texture, 0, sizeof(texture));
            texture.id = texture_from_file(str.data, pModel->pDirectory, false);

            texture_set_path(&texture, str.data);
            texture_set_type(&texture, typeName);

            // store it as texture loaded for entire model,
            // to ensure we won't unnecessary load duplicate textures.
            vector_push_back(pVecTextures, (const char*) &texture);
            GE_CHECK(model_texture_loaded_push_back(pModel, &texture));
        }
    }
    return pVecTextures;
}

/**
 * Push a new texture struct object into model
 * @param pModel
 * @param pTexture
 * @return GE_Types
 */
int model_texture_loaded_push_back(struct Model *pModel, struct Texture *pTexture)
{
    if (pModel == NULL || pTexture == NULL) {
        return GE_ERROR_INVALID_PARAMETER;
    }

    // add a new texture struct object into model
    pModel->pTextureLoaded = realloc(pModel->pTextureLoaded,
                     sizeof(struct Texture) * (pModel->iTextureLoadedLength + 1));
    if (pModel->pTextureLoaded == NULL) {
        LOGE("Realloc error.");
        return GE_ERROR_MALLOC_FAILED;
    }
    struct Texture *pNewTexture = pModel->pTextureLoaded + (pModel->iTextureLoadedLength);
    pModel->iTextureLoadedLength++;
    init_texture(pNewTexture);
    texture_set_type(pNewTexture, pTexture->type);
    texture_set_path(pNewTexture, pTexture->path);
    pNewTexture->id = pTexture->id;

    return 0;
}

/**
 * Push a new mesh struct object to model
 * @param pModel
 * @param pMesh
 * @return GE_Types
 */
int model_mesh_push_back(struct Model *pModel, struct Mesh *pMesh)
{
    if (pModel == NULL || pMesh == NULL) {
        LOGE("Model mesh push back param error.");
        return GE_ERROR_INVALID_POINTER;
    }

    // add a new mesh struct object into model
    pModel->pMeshes = realloc(pModel->pMeshes,
                              sizeof(struct Mesh) * (pModel->iMeshLength + 1));
    if (pModel->pMeshes == NULL) {
        LOGE("Realloc error.");
        return GE_ERROR_MALLOC_FAILED;
    }
    struct Mesh *pNewMesh = &pModel->pMeshes[pModel->iMeshLength];
    pModel->iMeshLength++;
    // copy old mesh data memory to new mesh
    copy_mesh(pNewMesh, pMesh);

    return 0;
}

int draw_model(struct Model *pModel, unsigned int shader)
{
    if (pModel == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }

    for (int i = 0; i < pModel->iMeshLength; ++i) {
        draw_mesh(&pModel->pMeshes[i], shader);
    }

    return 0;
}