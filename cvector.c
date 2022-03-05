#include "cvector.h"
#include "main.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include "vertex.h"

#define DEFAULT_INIT_CAPACITY 8

int get_vector_data_type_size(struct Vector *pVector)
{
    if (pVector == NULL) {
        return 0;
    }
    int size = 0;
    switch (pVector->type) {
        case GE_VECTOR_INT:
            size = sizeof(int);
            break;
        case GE_VECTOR_UINT:
            size = sizeof(unsigned int);
            break;
        case GE_VECTOR_FLOAT:
            size = sizeof(float);
            break;
        case GE_VECTOR_MESH:
            size = sizeof(struct Mesh);
            break;
        case GE_VECTOR_MODEL:
            size = sizeof(struct Model);
            break;
        case GE_VECTOR_CHAR:
            size = sizeof(char);
            break;
        case GE_VECTOR_TEXTURE:
            size = sizeof(struct Texture);
            break;
        case GE_VECTOR_VERTEX:
            size = sizeof(struct Vertex);
            break;
        case GE_VECTOR_UNDEFINED:
        case GE_VECTOR_VOID_POINTER:
        default:
            return size;
    }
    return size;
}

int init_vector(struct Vector *pVector, int iVectorType)
{
    if (pVector == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }

    pVector->type = iVectorType;
    int size = get_vector_data_type_size(pVector);
    pVector->capacity = DEFAULT_INIT_CAPACITY;
    pVector->data = malloc(size * DEFAULT_INIT_CAPACITY);
    if (pVector->data == NULL) {
        LOGE("Malloc failed for vector.");
        return GE_ERROR_MALLOC_FAILED;
    }
    memset(pVector->data, 0, size * DEFAULT_INIT_CAPACITY);
    pVector->length = 0;

    return GE_ERROR_SUCCESS;
}

/**
 * Release the memory allocated by vector
 * @param pVector
 * @return - GE_Types
 */
int free_vector(struct Vector *pVector)
{
    if (pVector == NULL) {
        return GE_ERROR_INVALID_POINTER;
    }
    free(pVector->data);
    pVector->data = NULL;
    pVector->capacity = 0;
    pVector->length = 0;
    pVector->type = GE_VECTOR_UNDEFINED;
    return 0;
}

/**
 * Check whether vector is initialized or not
 * @param pVector
 * @return bool
 */
bool is_valid_vector(struct Vector *pVector)
{
    if (pVector == NULL) {
        return false;
    }

    if (pVector->data == NULL) {
        return false;
    }

    return true;
}

/**
 * Append one data to the back of the vector.
 * @param pVector
 * @param data - char* , pointer points to the data,
 * @return GE_Types
 */
int vector_push_back(struct Vector *pVector, const char* data)
{
    if (!is_valid_vector(pVector) || data == NULL) {
        return GE_ERROR_INVALID_PARAMETER;
    }

    int size = get_vector_data_type_size(pVector);
    if (size == 0) {
        return GE_ERROR_INVALID_PARAMETER;
    }

    if (pVector->length == pVector->capacity) {
        pVector->data = realloc(pVector->data, size * pVector->capacity * 2);
        if (pVector->data == NULL) {
            LOGE("Failed to realloc vector memory.");
            return GE_ERROR_MALLOC_FAILED;
        }
        pVector->capacity *= 2;
    }
    int offset = size * (pVector->length ) / (int) sizeof(char);
    char *pNewData = pVector->data + offset;
    memcpy(pNewData, data, size);
    pVector->length++;

    return 0;
}

/**
 * Insert data at the end of vector
 * @param pVector
 * @param pStart
 * @param size
 * @return GE_Types
 */
int vector_insert_back(struct Vector *pVector, char *pStart, size_t size)
{
    if (pVector == NULL || pStart == NULL || size <= 0) {
        return GE_ERROR_INVALID_PARAMETER;
    }
    int currentSize = get_vector_data_type_size(pVector);
    if (currentSize == 0) {
        return GE_ERROR_INVALID_PARAMETER;
    }

    while (currentSize + pVector->length > pVector->capacity) {
        pVector->data = realloc(pVector->data, size * pVector->capacity * 2);
        if (pVector->data == NULL) {
            LOGE("Failed to realloc vector memory.");
            return GE_ERROR_MALLOC_FAILED;
        }
        pVector->capacity *= 2;
    }
    int type_size = get_vector_data_type_size(pVector);
    int offset = type_size * (pVector->length ) / (int) sizeof(char);
    char *pNewData = pVector->data + offset;
    memcpy(pNewData, pStart, size);
    pVector->length += (int) size / get_vector_data_type_size(pVector);

    return 0;
}