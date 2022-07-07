/**
 * @file ap_cvector.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief CVector definitions
 *
 */
#ifndef AP_CVECTOR_H
#define AP_CVECTOR_H

#include "ap_utils.h"

#ifndef AP_VECTOR_DEFAULT_CAPACITY
#define AP_VECTOR_DEFAULT_CAPACITY 8
#endif

typedef enum {
        /** uninitialized vector type */
        AP_VECTOR_UNDEFINED = 0,    //
        /** @see AP_Vertex */
        AP_VECTOR_VERTEX = 0x0101,  // 257
        /** @see AP_Shader */
        AP_VECTOR_SHADER,
        /** @see AP_Texture */
        AP_VECTOR_TEXTURE,
        /** @see AP_Mesh */
        AP_VECTOR_MESH,             // 260
        /** @see AP_Model */
        AP_VECTOR_MODEL,
        /** @see AP_Camera */
        AP_VECTOR_CAMERA,
        /** @see AP_Audio */
        AP_VECTOR_AUDIO,
        /** @see AP_PCreature */
        AP_VECTOR_PCREATURE,
        /** @see AP_PBarrier */
        AP_VECTOR_PBARRIER,         // 265
        /** char */
        AP_VECTOR_CHAR,
        /** @see AP_Character */
        AP_VECTOR_CHARACTER,
        /** @see AP_Light */
        AP_VECTOR_LIGHT,
        /** int */
        AP_VECTOR_INT,
        /** unsigned int */
        AP_VECTOR_UINT,             // 270
        /** float */
        AP_VECTOR_FLOAT,
        /** char* (pointer array) */
        AP_VECTOR_POINTER,
} AP_VECTOR_Types;

/**
 * @brief C Vector struct object definition
 */
struct AP_Vector {
        /** data length */
        int length;
        /** total capacity */
        int capacity;
        /**
         * @brief vector type
         * @see AP_VECTOR_Types
         */
        int type;
        /** pointer points to data */
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
 * @return AP_Error_Types
 */
int ap_vector_free(struct AP_Vector *vector);

/**
 * @brief Append one data to the back of the vector.
 * @param vector
 * @param data - char* , pointer points to the data,
 * @return AP_Error_Types
 */
int ap_vector_push_back(struct AP_Vector *vector, const char* data);

/**
 * @brief Insert data at the end of vector
 * @param vector
 * @param start pointer points to data
 * @param size size of data (byte)
 * @return AP_Error_Types
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