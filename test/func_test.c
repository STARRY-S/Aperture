#include "func_test.h"
#include "ap_cvector.h"
#include "ap_vertex.h"
#include "ap_mesh.h"
#include "ap_model.h"

void print_vector(struct AP_Vector *vector);
void print_vertex(struct AP_Vertex *pVertex);
void print_mesh(struct AP_Mesh *mesh);
void print_model(struct AP_Model *model);

void print_vector(struct AP_Vector *vector)
{
        if (!vector) {
                return;
        }

        printf("current length:   %d\n", vector->length);
        printf("current capacity: %d\n", vector->capacity);
        printf("current type:     %d\n", vector->type);
        printf("current pdata:    %p\n", vector->data);
        printf("current data: ");

        switch (vector->type)
        {
        case AP_VECTOR_VERTEX:
                break;
        case AP_VECTOR_TEXTURE:
                break;
        case AP_VECTOR_MESH:
                break;
        case AP_VECTOR_MODEL:
                break;
        case AP_VECTOR_CHAR:
                for (int i = 0; i < vector->length; ++i) {
                char *p = (char*) vector->data;
                printf("%c ", p[i]);
                }
                printf("\n");
                break;
        case AP_VECTOR_INT:
                for (int i = 0; i < vector->length; ++i) {
                int *p = (int*) vector->data;
                printf("%d ", p[i]);
                }
                printf("\n");
                break;
        case AP_VECTOR_UINT:
                for (int i = 0; i < vector->length; ++i) {
                unsigned int *p = (unsigned *) vector->data;
                printf("%u ", p[i]);
                }
                printf("\n");
                break;
        case AP_VECTOR_FLOAT:
                for (int i = 0; i < vector->length; ++i) {
                float *p = (float*) vector->data;
                printf("%.2f ", p[i]);
                }
                printf("\n");
                break;
        default:
                printf("\n");
                break;
        }
        printf("\n");
}

void print_vertex(struct AP_Vertex *p)
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

void print_mesh(struct AP_Mesh *p)
{
        if (!p) {
                return;
        }
        printf("vertices:       %p\n", p->vertices);
        printf("vertices_length: %d\n", p->vertices_length);
        printf("indices:        %p\n", p->indices);
        printf("indices_length:  %d\n", p->indices_length);
        printf("textures:       %p\n", p->textures);
        printf("texture_length:  %d\n", p->texture_length);

        printf("VAO: %u", p->VAO);
        printf("VBO: %u", p->VBO);
        printf("EBO: %u", p->EBO);
}

void print_model(struct AP_Model *p)
{
        if (!p) {
                return;
        }
        printf("texture:       %p\n", p->texture);
        printf("texture_length: %d\n", p->texture_length);
        printf("mesh:              %p\n", p->mesh);
        printf("mesh_length:          %d\n", p->mesh_length);
        printf("directory:           %p\n", p->directory);
}

void test_vector_char()
{
        LOGI("-------AP_Vector Char test-------\n");

        struct AP_Vector *vector;
        vector = malloc( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_CHAR\n");
        ap_vector_init(vector, AP_VECTOR_CHAR);
        LOGI("Finished init vector\n");

        LOGI("Push data A - Z into vector\n");
        for (char c = 'A'; c <= 'Z'; c++) {
                ap_vector_push_back(vector, &c);
        }
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Push data a - g into vector\n");
        ap_vector_insert_back(vector, "abcdefghijklmnopqrstuvwxyz", 26);
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Start free vector\n");
        ap_vector_free(vector);
        LOGI("Finished free vector\n");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR\n");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector\n");

        free(vector);
        vector = NULL;

        printf("------AP_Vector test finished--------\n\n");
}

void test_vector_float()
{
        LOGI("-------AP_Vector Float test-------\n");

        struct AP_Vector *vector;
        vector = malloc( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_FLOAT\n");
        ap_vector_init(vector, AP_VECTOR_FLOAT);
        LOGI("Finished init vector\n");

        LOGI("Push data 1.0f - 10.0f into vector\n");
        for (float f = 1.0f; (11.0f - f ) > 0.001f ; f += 1.0f) {
                ap_vector_push_back(vector, (char*) &f);
        }
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Push data 11.0f-20.0f into vector\n");
        float fArray[10] = {
                11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f,
                19.0f, 20.0f};
        ap_vector_insert_back(vector, (char*) &fArray, 10 * sizeof(float));
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Start free vector\n");
        ap_vector_free(vector);
        LOGI("Finished free vector\n");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR\n");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector\n");

        free(vector);
        vector = NULL;

        printf("------AP_Vector float finished--------\n\n");
}

void test_vector_uint()
{
        LOGI("-------AP_Vector UINT test-------\n");

        struct AP_Vector *vector;
        vector = malloc( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_UINT\n");
        ap_vector_init(vector, AP_VECTOR_UINT);
        LOGI("Finished init vector\n");

        LOGI("Push data 1 - 10 into vector\n");
        for (unsigned u = 1; u <= 10 ; u += 1) {
                ap_vector_push_back(vector, (char*) &u);
        }
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Push data 11 - 20 into vector\n");
        unsigned uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        ap_vector_insert_back(vector, (char*) &uArray, 10 * sizeof(unsigned));
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Start free vector\n");
        ap_vector_free(vector);
        LOGI("Finished free vector\n");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR\n");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector\n");

        free(vector);
        vector = NULL;

        printf("------AP_Vector UINT finished--------\n\n");
}

void test_vector_int()
{
        LOGI("-------AP_Vector INT test-------\n");

        struct AP_Vector *vector;
        vector = malloc( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_UINT\n");
        ap_vector_init(vector, AP_VECTOR_UINT);
        LOGI("Finished init vector\n");

        LOGI("Push data 1 - 10 into vector\n");
        for (int u = 1; u <= 10 ; u += 1) {
                ap_vector_push_back(vector, (char*) &u);
        }
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Push data 11 - 20 into vector\n");
        int uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        ap_vector_insert_back(vector, (char*) &uArray, 10 * sizeof(int));
        LOGI("Push finished\n");

        print_vector(vector);

        LOGI("Start free vector\n");
        ap_vector_free(vector);
        LOGI("Finished free vector\n");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR\n");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector\n");

        free(vector);
        vector = NULL;

        printf("------AP_Vector INT finished--------\n\n");
}