#include <GLES3/gl3.h>

#include <assimp/cfileio.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ap_utils.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_camera.h"
#include "ap_cvector.h"

const char *AP_ERROR_NAME[AP_ERROR_LENGTH] = {
        "SUCCESS",
        "INVALID_POINTER",
        "INVALID_PARAMETER",
        "MALLOC_FAILED",
        "MESH_UNINITIALIZED",
        "ASSIMP_IMPORT_FAILED",
        "ASSET_OPEN_FAILED",
        "INIT_FAILED",
        "RENDER_FAILED",
        "TEXTURE_FAILED",
        "SHADER_LOAD_FAILED",
        "CAMERA_NOT_SET",
        "SHADER_NOT_SET",
        "MODEL_NOT_SET",
        "UNKNOWN"
};

static struct AP_Vector pointer_vector = { 0, 0, 0, 0};

#ifdef __ANDROID__

static AAssetManager *local_asset_manager = NULL;
static char mobile_name[AP_DEFAULT_BUFFER_SIZE] = { 0 };

void *ap_get_asset_manager()
{
        return (void*) local_asset_manager;
}

int ap_set_asset_manager(void *p_void)
{
        if (!p_void) {
                LOGE("ap_set_asset_manager ERROR: NULL");
                local_asset_manager = NULL;
                return AP_ERROR_INVALID_POINTER;
        }

        struct AAssetManager *manager = (struct AAssetManager*) p_void;
        if (local_asset_manager == NULL) {
                local_asset_manager = manager;
        } else {
                LOGI("Replace Local AssetManager Pointer");
                local_asset_manager = manager;
        }
        return 0;
}

int ap_set_mobile_name(const char *name)
{
        if (name == NULL) {
                LOGE("ap_set_mobile_name ERROR: NULL");
                return AP_ERROR_INVALID_POINTER;
        }

        strncpy(mobile_name, name, AP_DEFAULT_BUFFER_SIZE);
        mobile_name[AP_DEFAULT_BUFFER_SIZE - 1] = '0';

        return 0;
}

const char* ap_get_mobile_name()
{
        return mobile_name;
}

int ap_get_mobile_type(const char *name)
{
        if (name == NULL) {
                LOGE("ap_get_mobile_type Failed: NULL");
                return 0;
        }
        for (int i = 0; i < AP_MOBILE_LENGTH; ++i) {
                if (strstr(name, AP_MOBILE_NAME[i]) != NULL) {
                        return i;
                }
        }
        return 0;
}
#else   // Not Android

// Nothing...

#endif  // Not Android

static const float ap_utils_cube_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

const float* ap_get_default_cube_vertices()
{
    return ap_utils_cube_vertices;
}

int ap_get_default_cube_vertices_length()
{
    return sizeof(ap_utils_cube_vertices);
}

void *ap_get_default_vector_ptr()
{
        if (pointer_vector.data == NULL) {
                ap_vector_init(&pointer_vector, AP_VECTOR_POINTER);
        }
        return (void*) &pointer_vector;
}

void* ap_malloc(int size)
{
        char* ptr = (char*) ap_malloc(size);
        ap_vector_push_back(ap_get_default_vector_ptr(), (char*) &ptr);
        return ptr;
}

void* ap_realloc(void* ptr, int size)
{
        struct AP_Vector *vector = ap_get_default_vector_ptr();
        char **ptr_data = (char**) vector->data;
        bool found = false;
        for (int i = 0; i < vector->length; i++) {
                if (ptr_data[i] == (char*) ptr) {
                        AP_CHECK( ap_vector_remove_data(
                                vector, (char*) ptr_data + i, 
                                (char*) ptr_data + i + 1, 
                                sizeof(char*)
                        ) );
                        found = true;
                }
        }
        if (!found) {
                LOGE("AP_REALLOC: unable to find pointer in vector: %p", ptr);
                return NULL;
        }
        char *ptr_new = ap_realloc(ptr, size);
        ap_vector_push_back(ap_get_default_vector_ptr(), (char*) &ptr_new);
        return ptr_new;
}

void ap_free(void* ptr)
{
        struct AP_Vector *vector = ap_get_default_vector_ptr();
        char **ptr_data = (char**) vector->data;
        bool found = false;
        for (int i = 0; i < vector->length; i++) {
                if (ptr_data[i] == (char*) ptr) {
                        AP_CHECK( ap_vector_remove_data(
                                vector,
                                (char*) (ptr_data + i), 
                                (char*) (ptr_data + i + 1), 
                                sizeof(char*)
                        ) );
                        found = true;
                }
        }
        if (!found) {
                LOGE("AP_FREE: unable to find pointer in vector: %p", ptr);
        }
        ap_free(ptr);
        ptr = NULL;
}

int ap_unreleased_pointer_get_len()
{
        return pointer_vector.length;
}

void ap_unreleased_pointer_print()
{
        if (pointer_vector.length == 0) {
                LOGI("All pointers are released");
                return;
        }

        char **ptr_arr = (char**) pointer_vector.data;
        LOGI("Here are the pointers unreleased:");
        for (int i = 0; i < pointer_vector.length; ++i) {
                LOGI("\t%p", ptr_arr[i]);
        }
}