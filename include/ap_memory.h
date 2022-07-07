/**
 * @file ap_memory.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief memory related functions
 *
 */
#ifndef AP_MEMORY_H
#define AP_MEMORY_H

/**
 * @brief AP_MALLOC with trace log
 *
 * @param size memory size
 * @param func_name use __func__ to show the name of the function
 * @return void* pointer of the memory
 */
void *AP_MALLOCT(int size, const char *func_name);

#define AP_MALLOC(i) AP_MALLOCT(i, __func__)

void AP_FREE(void* ptr);

void* AP_REALLOC(void *ptr, int size);

/**
 * @brief Get the number of unreleased pointers.
 *
 * @return int the number of unreleased pointers.
 */
int ap_memory_unreleased_num();

/**
 * @brief Release all of the pointers
 *
 * @return int AP_Types
 */
int ap_memory_release();

#endif