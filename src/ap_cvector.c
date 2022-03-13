#include "ap_cvector.h"
#include "ap_utils.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include "vertex.h"
#include "ap_camera.h"

/**
 * Check whether vector is initialized or not
 * @param pVector
 * @return bool
 */
bool ap_is_valid_vector(struct AP_Vector *pVector);

int ap_vector_data_type_size(struct AP_Vector *pVector)
{
    if (pVector == NULL) {
        return 0;
    }
    int size = 0;
    switch (pVector->type) {
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
            size = sizeof(struct Mesh);
            break;
        case AP_VECTOR_MODEL:
            size = sizeof(struct Model);
            break;
        case AP_VECTOR_CAMERA:
            size = sizeof(struct AP_Camera);
            break;
        case AP_VECTOR_CHAR:
            size = sizeof(char);
            break;
        case AP_VECTOR_TEXTURE:
            size = sizeof(struct Texture);
            break;
        case AP_VECTOR_VERTEX:
            size = sizeof(struct Vertex);
            break;
        case AP_VECTOR_UNDEFINED:
        default:
            LOGW("Unknow vector type: %d\n", pVector->type);
            return size;
    }
    return size;
}

int ap_vector_init(struct AP_Vector *pVector, int iVectorType)
{
    if (pVector == NULL) {
        return AP_ERROR_INVALID_POINTER;
    }

    pVector->type = iVectorType;
    int size = ap_vector_data_type_size(pVector);
    pVector->capacity = AP_VECTOR_DEFAULT_CAPACITY;
    pVector->data = malloc(size * AP_VECTOR_DEFAULT_CAPACITY);
    if (pVector->data == NULL) {
        LOGE("Malloc failed for vector.");
        return AP_ERROR_MALLOC_FAILED;
    }
    memset(pVector->data, 0, size * AP_VECTOR_DEFAULT_CAPACITY);
    pVector->length = 0;

    return AP_ERROR_SUCCESS;
}

int ap_vector_free(struct AP_Vector *pVector)
{
    if (pVector == NULL) {
        return AP_ERROR_INVALID_POINTER;
    }
    free(pVector->data);
    pVector->data = NULL;
    pVector->capacity = 0;
    pVector->length = 0;
    pVector->type = AP_VECTOR_UNDEFINED;
    return 0;
}

bool ap_is_valid_vector(struct AP_Vector *pVector)
{
    if (pVector == NULL) {
        return false;
    }

    if (pVector->data == NULL) {
        return false;
    }

    return true;
}

int ap_vector_push_back(struct AP_Vector *pVector, const char* data)
{
    if (!ap_is_valid_vector(pVector) || data == NULL) {
        return AP_ERROR_INVALID_PARAMETER;
    }

    int size = ap_vector_data_type_size(pVector);
    if (size == 0) {
        return AP_ERROR_INVALID_PARAMETER;
    }

    if (pVector->length == pVector->capacity) {
        pVector->data = realloc(pVector->data, size * pVector->capacity * 2);
        if (pVector->data == NULL) {
            LOGE("Failed to realloc vector memory.");
            return AP_ERROR_MALLOC_FAILED;
        }
        pVector->capacity *= 2;
    }
    int offset = size * (pVector->length ) / (int) sizeof(char);
    char *pNewData = pVector->data + offset;
    memcpy(pNewData, data, size);
    pVector->length++;

    return 0;
}

int ap_vector_insert_back(struct AP_Vector *pVector, char *pStart, size_t size)
{
    if (pVector == NULL || pStart == NULL || size <= 0) {
        return AP_ERROR_INVALID_PARAMETER;
    }
    int type_size = ap_vector_data_type_size(pVector);
    if (type_size == 0) {
        return AP_ERROR_INVALID_PARAMETER;
    }

    while (pVector->length + (size / type_size) > pVector->capacity) {
        pVector->data = realloc(pVector->data, size * pVector->capacity * 2);
        if (pVector->data == NULL) {
            LOGE("Failed to realloc vector memory.");
            return AP_ERROR_MALLOC_FAILED;
        }
        pVector->capacity *= 2;
        LOGI("Realloc vector capacity: %d\n", pVector->capacity);
    }
    int offset = type_size * (pVector->length) / (int) sizeof(char);
    char *pNewData = pVector->data + offset;
    memcpy(pNewData, pStart, size);
    pVector->length += (int) size / ap_vector_data_type_size(pVector);

    return 0;
}