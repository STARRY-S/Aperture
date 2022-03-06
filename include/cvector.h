/**
 * Vector implementation in C
 */

#ifndef GAME_ENGINE_CVECTOR_H
#define GAME_ENGINE_CVECTOR_H

#include "ge_utils.h"

// vector types definition
typedef enum {
    GE_VECTOR_UNDEFINED = 0,    // uninitialized vector type
    GE_VECTOR_VERTEX,           // Vertex vector
    GE_VECTOR_TEXTURE,          // Texture vector
    GE_VECTOR_MESH,             // Mesh vector
    GE_VECTOR_MODEL,            // Model vector
    GE_VECTOR_CHAR,             // Char vector
    GE_VECTOR_INT,              // Integer vector
    GE_VECTOR_UINT,             // Unsigned int vector
    GE_VECTOR_FLOAT,            // Float vector
    GE_VECTOR_VOID_POINTER,     // (void*) vector
    GE_VECTOR_LENGTH            // never use this
} GE_VECTOR_Types;

struct Vector {
    int length;     // number of elements
    int capacity;   // total capacity
    int type;       // vector type
    char* data;
};

int init_vector(struct Vector *pVector, int iVectorType);

/**
 * Release the memory allocated by vector
 * @param pVector
 * @return - GE_Types
 */
int free_vector(struct Vector *pVector);

/**
 * Append one data to the back of the vector.
 * @param pVector
 * @param data - char* , pointer points to the data,
 * @return GE_Types
 */
int vector_push_back(struct Vector *pVector, const char* data);

/**
 * Insert data at the end of vector
 * @param pVector
 * @param pStart pointer points to data
 * @param size size of data (byte)
 * @return GE_Types
 */
int vector_insert_back(struct Vector *pVector, char *pStart, size_t size);

#endif // GAME_ENGINE_CVECTOR_H