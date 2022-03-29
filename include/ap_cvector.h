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

typedef enum {
        AP_VECTOR_UNDEFINED = 0,    // uninitialized vector type
        AP_VECTOR_VERTEX,           // Vertex vector
        AP_VECTOR_TEXTURE,          // Texture vector
        AP_VECTOR_MESH,             // AP_Mesh vector
        AP_VECTOR_MODEL,            // Model vector
        AP_VECTOR_CAMERA,           // Camera vector
        AP_VECTOR_CHAR,             // Char vector
        AP_VECTOR_INT,              // Integer vector
        AP_VECTOR_UINT,             // Unsigned int vector
        AP_VECTOR_FLOAT,            // Float vector
        AP_VECTOR_POINTER,          // char* pointer array
        AP_VECTOR_LENGTH            // never use this
} AP_VECTOR_Types;

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
 * @param vector pointer points to vector struct object
 * @param vector_type AP_VECTOR_Types
 * @return int GE_Types
 */
int ap_vector_init(struct AP_Vector *vector, int vector_type);

/**
 * @brief Release the memory allocated by vector
 * @param vector
 * @return - AP_Types
 */
int ap_vector_free(struct AP_Vector *vector);

/**
 * @brief Append one data to the back of the vector.
 * @param vector
 * @param data - char* , pointer points to the data,
 * @return AP_Types
 */
int ap_vector_push_back(struct AP_Vector *vector, const char* data);

/**
 * @brief Insert data at the end of vector
 * @param vector
 * @param start pointer points to data
 * @param size size of data (byte)
 * @return AP_Types
 */
int ap_vector_insert_back(
        struct AP_Vector *vector,
        char *start,
        size_t size
);

/**
 * Remove data from vector data
 * @param start pointer points to the data to be removed
 * @param end pointer points to the next data
 * @param size size of data to be removed,
 *             should be divisible by `ap_vector_data_type_size()`
 */
int ap_vector_remove_data(
        struct AP_Vector *vector,
        char *start,
        char *end,
        int size
);

#endif // AP_CVECTOR_H