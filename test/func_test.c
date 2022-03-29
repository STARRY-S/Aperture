#include "func_test.h"
#include "ap_cvector.h"
#include "ap_vertex.h"
#include "ap_mesh.h"
#include "ap_model.h"
#include "ap_memory.h"
#include "ap_utils.h"
#include "ap_model.h"
#include <pthread.h>

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
        case AP_VECTOR_POINTER:
                char **ptr_arr = (char**) vector->data;
                for (int i = 0; i < vector->length; ++i) {
                        printf("%p \n", ptr_arr[i]);
                }
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
        printf("position:  %f %f %f\n", p->position[0], p->position[1], p->position[2]);
        printf("normal:    %f %f %f\n", p->normal[0], p->normal[1], p->normal[2]);
        printf("tex_coords: %f %f\n", p->tex_coords[0], p->tex_coords[1]);
        printf("tangent:   %f %f %f\n", p->tangent[0], p->tangent[1], p->tangent[2]);
        printf("big_tangent:%f %f %f\n", p->big_tangent[0], p->big_tangent[1], p->big_tangent[2]);

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
        LOGI("-------AP_Vector Char test-------");

        struct AP_Vector *vector;
        vector = AP_MALLOC( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_CHAR");
        ap_vector_init(vector, AP_VECTOR_CHAR);
        LOGI("Finished init vector");

        LOGI("Push data A - Z into vector");
        for (char c = 'A'; c <= 'Z'; c++) {
                ap_vector_push_back(vector, &c);
        }
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Push data a - g into vector");
        ap_vector_insert_back(vector, "abcdefghijklmnopqrstuvwxyz", 26);
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Start AP_FREE vector");
        ap_vector_free(vector);
        LOGI("Finished AP_FREE vector");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector");

        AP_FREE(vector);
        vector = NULL;

        printf("------AP_Vector test finished--------\n\n");
}

void test_vector_float()
{
        LOGI("-------AP_Vector Float test-------");

        struct AP_Vector *vector;
        vector = AP_MALLOC( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_FLOAT");
        ap_vector_init(vector, AP_VECTOR_FLOAT);
        LOGI("Finished init vector");

        LOGI("Push data 1.0f - 10.0f into vector");
        for (float f = 1.0f; (11.0f - f ) > 0.001f ; f += 1.0f) {
                ap_vector_push_back(vector, (char*) &f);
        }
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Push data 11.0f-20.0f into vector");
        float fArray[10] = {
                11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f,
                19.0f, 20.0f};
        ap_vector_insert_back(vector, (char*) &fArray, 10 * sizeof(float));
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Start AP_FREE vector");
        ap_vector_free(vector);
        LOGI("Finished AP_FREE vector");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector");

        AP_FREE(vector);
        vector = NULL;

        printf("------AP_Vector float finished--------\n\n");
}

void test_vector_uint()
{
        LOGI("-------AP_Vector UINT test-------\n");

        struct AP_Vector *vector;
        vector = AP_MALLOC( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_UINT");
        ap_vector_init(vector, AP_VECTOR_UINT);
        LOGI("Finished init vector");

        LOGI("Push data 1 - 10 into vector");
        for (unsigned u = 1; u <= 10 ; u += 1) {
                ap_vector_push_back(vector, (char*) &u);
        }
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Push data 11 - 20 into vector");
        unsigned uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        ap_vector_insert_back(vector, (char*) &uArray, 10 * sizeof(unsigned));
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Start AP_FREE vector");
        ap_vector_free(vector);
        LOGI("Finished AP_FREE vector");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector");

        AP_FREE(vector);
        vector = NULL;

        printf("------AP_Vector UINT finished--------\n\n");
}

void test_vector_int()
{
        LOGI("-------AP_Vector INT test-------");

        struct AP_Vector *vector;
        vector = AP_MALLOC( sizeof(struct AP_Vector) );

        LOGI("Start init vector AP_VECTOR_UINT");
        ap_vector_init(vector, AP_VECTOR_UINT);
        LOGI("Finished init vector");

        LOGI("Push data 1 - 10 into vector");
        for (int u = 1; u <= 10 ; u += 1) {
                ap_vector_push_back(vector, (char*) &u);
        }
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Push data 11 - 20 into vector");
        int uArray[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
        ap_vector_insert_back(vector, (char*) &uArray, 10 * sizeof(int));
        LOGI("Push finished");

        print_vector(vector);

        LOGI("Start AP_FREE vector");
        ap_vector_free(vector);
        LOGI("Finished AP_FREE vector");
        print_vector(vector);

        LOGI("Start init vector AP_VECTOR_CHAR");
        ap_vector_init(vector, AP_VECTOR_INT);
        LOGI("Finished init vector");

        AP_FREE(vector);
        vector = NULL;

        printf("------AP_Vector INT finished--------\n\n");
}

void test_ap_memory()
{
        char *str = AP_MALLOC(10 * sizeof(char));
        strcpy(str, "hello");
        LOGI("%s", str);
        // print_vector(vector);

        str = AP_REALLOC(str, 20 * sizeof(char));
        LOGI("%s", str);
        // print_vector(vector);

        char *str2 = AP_MALLOC(10);
        // print_vector(vector);

        AP_FREE(str);
        // print_vector(vector);

        AP_FREE(str2);
        // print_vector(vector);

        return;
}

// int model_generated = 0;
// unsigned model_id = 0;

// static int ap_model_thread_cb(void* data, int reserve)
// {
//         model_generated = 1;
//         struct AP_Model_Thread_Param *param = data;

//         LOGD("generated model %s", param->path);
//         LOGD("model id %u", param->id);
//         model_generated = param->id;
//         AP_FREE(param->path);
//         AP_FREE(param);
//         return 0;
// }

// void test_model_async()
// {
//         glfwInit();
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//         #ifdef __APPLE__
//         glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//         #endif

//         glfwWindowHint( GLFW_VISIBLE, GL_FALSE );
//         GLFWwindow* threadWin = glfwCreateWindow(
//                 1, 1, "Thread Window", NULL, NULL );

//         glfwWindowHint( GLFW_VISIBLE, GL_TRUE );
//         GLFWwindow* window = NULL;
//         window = glfwCreateWindow(400, 400,
//                 "test", NULL, threadWin);

//         if (window == NULL)
//         {
//                 LOGE("Failed to create GLFW window.");
//                 glfwTerminate();
//                 return;
//         }

//         glfwMakeContextCurrent(window);

//         ap_set_context_ptr(window);
//         const char *test_path = "backpack/backpack.obj";
//         ap_model_generate_async(test_path, ap_model_thread_cb);
//         while (!glfwWindowShouldClose(window)) {
//                 if (!model_generated) {
//                         LOGD("waiting");
//                 }
//                 // refresh
//                 glfwSwapBuffers(window);
//                 glfwPollEvents();
//         }
//         LOGI("generated model id %u", model_id);
//         ap_memory_release();
// }