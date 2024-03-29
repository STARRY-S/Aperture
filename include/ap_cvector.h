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
        AP_VECTOR_VERTEX = 0x0101,  // Vertex
        AP_VECTOR_TEXTURE,          // Texture
        AP_VECTOR_MESH,             // AP_Mesh
        AP_VECTOR_MODEL,            // Model
        AP_VECTOR_CAMERA,           // Camera
        AP_VECTOR_AUDIO,            // Audio
        AP_VECTOR_PCREATURE,        // Creature
        AP_VECTOR_PBARRIER,         // Barrier
        AP_VECTOR_CHAR,             // Char
        AP_VECTOR_CHARACTOR,        // Charactor
        AP_VECTOR_LIGHT,            // Light
        AP_VECTOR_INT,              // Integer
        AP_VECTOR_UINT,             // Unsigned int
        AP_VECTOR_FLOAT,            // Float
        AP_VECTOR_POINTER,          // char* pointer array
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