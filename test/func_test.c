#include "func_test.h"
#include "cvector.h"
#include "vertex.h"
#include "mesh.h"
#include "model.h"

void printVector(struct Vector *pVector);
void printVertex(struct Vertex *pVertex);
void printMesh(struct Mesh *pMesh);
void printModel(struct Model *pModel);

void printVector(struct Vector *pVector)
{
    if (!pVector) {
        return;
    }

    printf("current length:   %d\n", pVector->length);
    printf("current capacity: %d\n", pVector->capacity);
    printf("current type:     %d\n", pVector->type);
    printf("current pdata:    %p\n", pVector->data);
    printf("current data: ");

    switch (pVector->type)
    {
    case GE_VECTOR_VERTEX:
        break;
    case GE_VECTOR_TEXTURE:
        break;
    case GE_VECTOR_MESH:
        break;
    case GE_VECTOR_MODEL:
        break;
    case GE_VECTOR_CHAR:
        for (int i = 0; i < pVector->length; ++i) {
            char *p = (char*) pVector->data;
            printf("%c ", p[i]);
        }
        printf("\n");
        break;
    case GE_VECTOR_INT:
        for (int i = 0; i < pVector->length; ++i) {
            int *p = (int*) pVector->data;
            printf("%d ", p[i]);
        }
        printf("\n");
        break;
    case GE_VECTOR_UINT:
        for (int i = 0; i < pVector->length; ++i) {
            unsigned int *p = (unsigned *) pVector->data;
            printf("%u ", p[i]);
        }
        printf("\n");
        break;
    case GE_VECTOR_FLOAT:
        for (int i = 0; i < pVector->length; ++i) {
            float *p = (float*) pVector->data;
            printf("%.2f ", p[i]);
        }
        printf("\n");
        break;
    case GE_VECTOR_VOID_POINTER:
        // not support yet
        break;
    default:
        printf("\n");
        break;
    }
    printf("\n");
}

void printVertex(struct Vertex *p)
{
    if (!p) {
        return;
    }
    printf("Position:  %f %f %f\n", p->Position[0], p->Position[1], p->Position[2]);
    printf("Normal:    %f %f %f\n", p->Normal[0], p->Normal[1], p->Normal[2]);
    printf("TexCoords: %f %f\n", p->TexCoords[0], p->TexCoords[1]);
    printf("Tangent:   %f %f %f\n", p->Tangent[0], p->Tangent[1], p->Tangent[2]);
    printf("BigTangent:%f %f %f\n", p->BigTangent[0], p->BigTangent[1], p->BigTangent[2]);

    // mBoneIDs
    // mWeights
}

void printMesh(struct Mesh *p)
{
    if (!p) {
        return;
    }
    printf("pVertices:       %p\n", p->pVertices);
    printf("iVerticesLength: %d\n", p->iVerticesLength);
    printf("pIndices:        %p\n", p->pIndices);
    printf("iIndicesLength:  %d\n", p->iIndicesLength);
    printf("pTextures:       %p\n", p->pTextures);
    printf("iTextureLength:  %d\n", p->iTextureLength);

    printf("VAO: %u", p->VAO);
    printf("VBO: %u", p->VBO);
    printf("EBO: %u", p->EBO);
}

void printModel(struct Model *p)
{
    if (!p) {
        return;
    }
    printf("pTextureLoaded:       %p\n", p->pTextureLoaded);
    printf("iTextureLoadedLength: %d\n", p->iTextureLoadedLength);
    printf("pMeshes:              %p\n", p->pMeshes);
    printf("iMeshLength:          %d\n", p->iMeshLength);
    printf("pDirectory:           %p\n", p->pDirectory);
}

void vectorCharTest()
{
    LOGI("-------Vector Char test-------\n");

    struct Vector *pVector;
    pVector = malloc( sizeof(struct Vector) );

    LOGI("Start init vector GE_VECTOR_CHAR\n");
    init_vector(pVector, GE_VECTOR_CHAR);
    LOGI("Finished init vector\n");

    LOGI("Push data A - Z into vector\n");
    for (char c = 'A'; c <= 'Z'; c++) {
        vector_push_back(pVector, &c);
    }
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Push data a - g into vector\n");
    vector_insert_back(pVector, "abcdefghijklmnopqrstuvwxyz", 26);
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Start free vector\n");
    free_vector(pVector);
    LOGI("Finished free vector\n");
    printVector(pVector);

    LOGI("Start init vector GE_VECTOR_CHAR\n");
    init_vector(pVector, GE_VECTOR_INT);
    LOGI("Finished init vector\n");

    free(pVector);
    pVector = NULL;

    printf("------Vector test finished--------\n\n");
}

void vectorFloatTest()
{
    LOGI("-------Vector Float test-------\n");

    struct Vector *pVector;
    pVector = malloc( sizeof(struct Vector) );

    LOGI("Start init vector GE_VECTOR_FLOAT\n");
    init_vector(pVector, GE_VECTOR_FLOAT);
    LOGI("Finished init vector\n");

    LOGI("Push data 1.0f - 10.0f into vector\n");
    for (float f = 1.0f; (11.0f - f ) > 0.001f ; f += 1.0f) {
        vector_push_back(pVector, (char*) &f);
    }
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Push data 11.0f-20.0f into vector\n");
    float fArray[10] = { 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f,
        19.0f, 20.0f};
    vector_insert_back(pVector, (char*) &fArray, 10 * sizeof(float));
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Start free vector\n");
    free_vector(pVector);
    LOGI("Finished free vector\n");
    printVector(pVector);

    LOGI("Start init vector GE_VECTOR_CHAR\n");
    init_vector(pVector, GE_VECTOR_INT);
    LOGI("Finished init vector\n");

    free(pVector);
    pVector = NULL;

    printf("------Vector float finished--------\n\n");
}

void vectorUIntTest()
{
    LOGI("-------Vector UINT test-------\n");

    struct Vector *pVector;
    pVector = malloc( sizeof(struct Vector) );

    LOGI("Start init vector GE_VECTOR_UINT\n");
    init_vector(pVector, GE_VECTOR_UINT);
    LOGI("Finished init vector\n");

    LOGI("Push data 1 - 10 into vector\n");
    for (unsigned u = 1; u <= 10 ; u += 1) {
        vector_push_back(pVector, (char*) &u);
    }
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Push data 11 - 20 into vector\n");
    unsigned uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    vector_insert_back(pVector, (char*) &uArray, 10 * sizeof(unsigned));
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Start free vector\n");
    free_vector(pVector);
    LOGI("Finished free vector\n");
    printVector(pVector);

    LOGI("Start init vector GE_VECTOR_CHAR\n");
    init_vector(pVector, GE_VECTOR_INT);
    LOGI("Finished init vector\n");

    free(pVector);
    pVector = NULL;

    printf("------Vector UINT finished--------\n\n");
}

void vectorIntTest()
{
    LOGI("-------Vector INT test-------\n");

    struct Vector *pVector;
    pVector = malloc( sizeof(struct Vector) );

    LOGI("Start init vector GE_VECTOR_UINT\n");
    init_vector(pVector, GE_VECTOR_UINT);
    LOGI("Finished init vector\n");

    LOGI("Push data 1 - 10 into vector\n");
    for (int u = 1; u <= 10 ; u += 1) {
        vector_push_back(pVector, (char*) &u);
    }
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Push data 11 - 20 into vector\n");
    int uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    vector_insert_back(pVector, (char*) &uArray, 10 * sizeof(int));
    LOGI("Push finished\n");

    printVector(pVector);

    LOGI("Start free vector\n");
    free_vector(pVector);
    LOGI("Finished free vector\n");
    printVector(pVector);

    LOGI("Start init vector GE_VECTOR_CHAR\n");
    init_vector(pVector, GE_VECTOR_INT);
    LOGI("Finished init vector\n");

    free(pVector);
    pVector = NULL;

    printf("------Vector INT finished--------\n\n");
}