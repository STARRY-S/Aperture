#ifndef AP_MEMORY_H
#define AP_MEMORY_H

void *AP_MALLOC(int size);

void AP_FREE(void* ptr);

void* AP_REALLOC(void *ptr, int size);

/**
 * @brief Get the number of unreleased pointers.
 *
 * @return int - the number of unreleased pointers.
 */
int ap_memory_unreleased_num();

/**
 * @brief Release all of the pointers
 *
 * @return int AP_Types
 */
int ap_memory_release();

#endif