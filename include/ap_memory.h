#ifndef AP_MEMORY_H
#define AP_MEMORY_H

void *AP_MALLOC(int size);

void AP_FREE(void* ptr);

void* AP_REALLOC(void *ptr, int size);

int ap_memory_unreleased_num();

int ap_memory_print_unreleased();

int ap_memory_release();

#endif