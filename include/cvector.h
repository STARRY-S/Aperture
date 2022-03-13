/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Vector implementation in C
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_CVECTOR_H
#define AP_CVECTOR_H

#include "ap_utils.h"

#ifndef AP_VECTOR_DEFAULT_CAPACITY
#define AP_VECTOR_DEFAULT_CAPACITY 8
#endif

/**
 * @brief vector type defination
 */
typedef enum {
    AP_VECTOR_UNDEFINED = 0,    // uninitialized vector type
    AP_VECTOR_VERTEX,           // Vertex vector
    AP_VECTOR_TEXTURE,          // Texture vector
    AP_VECTOR_MESH,             // Mesh vector
    AP_VECTOR_MODEL,            // Model vector
    AP_VECTOR_CAMERA,           // Camera vector
    AP_VECTOR_CHAR,             // Char vector
    AP_VECTOR_INT,              // Integer vector
    AP_VECTOR_UINT,             // Unsigned int vector
    AP_VECTOR_FLOAT,            // Float vector
    AP_VECTOR_LENGTH            // never use this
} AP_VECTOR_Types;

/**
 * @brief AP_Vector defination
 */
struct AP_Vector {
    // number of elements
    int length;
    // total capacity
    int capacity;
    // vector type
    int type;
    // pointer points to data
    char* data;
};

/**
 * @brief vector initialize
 * @param pVector pointer points to vector struct object
 * @param iVectorType AP_VECTOR_Types
 * @return int GE_Types
 */
int ap_vector_init(struct AP_Vector *pVector, int iVectorType);

/**
 * @brief Release the memory allocated by vector
 * @param pVector
 * @return - AP_Types
 */
int ap_vector_free(struct AP_Vector *pVector);

/**
 * @brief Append one data to the back of the vector.
 * @param pVector
 * @param data - char* , pointer points to the data,
 * @return AP_Types
 */
int ap_vector_push_back(struct AP_Vector *pVector, const char* data);

/**
 * @brief Insert data at the end of vector
 * @param pVector
 * @param pStart pointer points to data
 * @param size size of data (byte)
 * @return AP_Types
 */
int ap_vector_insert_back(struct AP_Vector *pVector, char *pStart, size_t size);

#endif // AP_CVECTOR_H