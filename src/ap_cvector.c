#include "ap_cvector.h"
#include "ap_utils.h"
#include "ap_mesh.h"
#include "ap_model.h"
#include "ap_texture.h"
#include "ap_vertex.h"
#include "ap_camera.h"

/**
 * Check whether vector is initialized or not
 * @param vector
 * @return bool
 */
bool ap_is_valid_vector(struct AP_Vector *vector);

int ap_vector_data_type_size(struct AP_Vector *vector)
{
        if (vector == NULL) {
                return 0;
        }
        int size = 0;
        switch (vector->type) {
                case AP_VECTOR_INT:
                size = sizeof(int);
                break;
                case AP_VECTOR_UINT:
                size = sizeof(unsigned int);
                break;
                case AP_VECTOR_FLOAT:
                size = sizeof(float);
                break;
                case AP_VECTOR_MESH:
                size = sizeof(struct AP_Mesh);
                break;
                case AP_VECTOR_MODEL:
                size = sizeof(struct AP_Model);
                break;
                case AP_VECTOR_CAMERA:
                size = sizeof(struct AP_Camera);
                break;
                case AP_VECTOR_CHAR:
                size = sizeof(char);
                break;
                case AP_VECTOR_TEXTURE:
                size = sizeof(struct AP_Texture);
                break;
                case AP_VECTOR_VERTEX:
                size = sizeof(struct AP_Vertex);
                break;
                case AP_VECTOR_UNDEFINED:
                default:
                LOGW("Unknow vector type: %d\n", vector->type);
                return size;
        }
        return size;
}

int ap_vector_init(struct AP_Vector *vector, int vector_type)
{
        if (vector == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }

        vector->type = vector_type;
        int size = ap_vector_data_type_size(vector);
        vector->capacity = AP_VECTOR_DEFAULT_CAPACITY;
        vector->data = malloc(size * AP_VECTOR_DEFAULT_CAPACITY);
        if (vector->data == NULL) {
                LOGE("Malloc failed for vector.");
                return AP_ERROR_MALLOC_FAILED;
        }
        memset(vector->data, 0, size * AP_VECTOR_DEFAULT_CAPACITY);
        vector->length = 0;

        return AP_ERROR_SUCCESS;
}

int ap_vector_free(struct AP_Vector *vector)
{
        if (vector == NULL) {
                return AP_ERROR_INVALID_POINTER;
        }
        free(vector->data);
        vector->data = NULL;
        vector->capacity = 0;
        vector->length = 0;
        vector->type = AP_VECTOR_UNDEFINED;
        return 0;
}

bool ap_is_valid_vector(struct AP_Vector *vector)
{
        if (vector == NULL) {
                return false;
        }

        if (vector->data == NULL) {
                return false;
        }

        return true;
}

int ap_vector_push_back(struct AP_Vector *vector, const char* data)
{
        if (!ap_is_valid_vector(vector) || data == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        int size = ap_vector_data_type_size(vector);
        if (size == 0) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (vector->length == vector->capacity) {
                vector->data = realloc(
                        vector->data,
                        size * vector->capacity * 2
                );
                if (vector->data == NULL) {
                LOGE("Failed to realloc vector memory.");
                return AP_ERROR_MALLOC_FAILED;
                }
                vector->capacity *= 2;
        }
        int offset = size * (vector->length ) / (int) sizeof(char);
        char *new_data = vector->data + offset;
        memcpy(new_data, data, size);
        vector->length++;

        return 0;
}

int ap_vector_insert_back(struct AP_Vector *vector, char *pStart, size_t size)
{
        if (vector == NULL || pStart == NULL || size <= 0) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        int type_size = ap_vector_data_type_size(vector);
        if (type_size == 0) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        while (vector->length + (size / type_size) > vector->capacity) {
                vector->data = realloc(
                        vector->data,
                        size * vector->capacity * 2
                );
                if (vector->data == NULL) {
                LOGE("Failed to realloc vector memory.");
                return AP_ERROR_MALLOC_FAILED;
                }
                vector->capacity *= 2;
                LOGI("Realloc vector capacity: %d\n", vector->capacity);
        }
        int offset = type_size * (vector->length) / (int) sizeof(char);
        char *new_data = vector->data + offset;
        memcpy(new_data, pStart, size);
        vector->length += (int) size / ap_vector_data_type_size(vector);

        return 0;
}