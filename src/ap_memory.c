#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ap_utils.h"
#include "ap_memory.h"

#ifndef AP_MEM_VEC_CAP
#define AP_MEM_VEC_CAP 32
#endif

struct AP_Pointer_Vector {
        int length;
        int capacity;
        char *data;
};

static struct AP_Pointer_Vector pointer_vector = { 0, 0, 0 };

int ap_memory_vector_push(char *ptr)
{
        if (ptr == NULL) {
                return 0;
        }

        if (pointer_vector.data == NULL) {
                // initialize vector
                pointer_vector.capacity = AP_MEM_VEC_CAP;
                pointer_vector.data =  malloc(
                        sizeof(char*) * AP_MEM_VEC_CAP);
                if (pointer_vector.data == NULL) {
                        AP_CHECK(AP_ERROR_MALLOC_FAILED);
                        exit(1);
                }
                pointer_vector.length = 0;
        }
        if (pointer_vector.length >= pointer_vector.capacity) {
                pointer_vector.capacity *= 2;
                pointer_vector.data = (char*) realloc(
                        pointer_vector.data,
                        pointer_vector.capacity * sizeof(char*)
                );
                if (pointer_vector.data == NULL) {
                        LOGE("ap_memory: vector realloc failed");
                        exit(1);
                }
        }
        // pointer array
        char **ptr_array = (char**) pointer_vector.data;
        ptr_array[pointer_vector.length++] = ptr;
        // LOGD("ap_memory: push pointer %p", ptr);

        return 0;
}

int ap_memory_vector_popup(char* dst_ptr)
{
        if (dst_ptr == NULL) {
                return 0;
        }

        if (pointer_vector.length == 0) {
                LOGW("unable to popup from zero size vector");
                return 0;
        }

        char **ptr_arr = (char**) pointer_vector.data;
        for (int i = 0; i < pointer_vector.length; ++i) {
                if (ptr_arr[i] != dst_ptr) {
                        continue;
                }
                // LOGD("ap_memory: free pointer %p", dst_ptr);
                for (int j = i; j < pointer_vector.length - 1; j++) {
                        ptr_arr[j] = ptr_arr[j + 1];
                }
                pointer_vector.length--;
                return 0;
        }

        char *tmp = ptr_arr[pointer_vector.length - 1];
        if (tmp == dst_ptr) {
                pointer_vector.length--;
                // LOGD("ap_memory: free pointer %p", tmp);
                return 0;
        }

        LOGW("ap_memory: unable to find ptr %p", dst_ptr);
        return 0;
}

void *AP_MALLOC(int size)
{
        char* ptr = malloc(size);
        ap_memory_vector_push(ptr);
        return ptr;
}

void AP_FREE(void* ptr)
{
        if (ptr == NULL) {
                return;
        }
        ap_memory_vector_popup(ptr);
        free(ptr);
}

void* AP_REALLOC(void *ptr, int size)
{
        if (ptr != NULL) {
                ap_memory_vector_popup(ptr);
        }
        char* ptr_new = realloc(ptr, size);
        ap_memory_vector_push(ptr_new);
        return ptr_new;
}

int ap_memory_unreleased_num()
{
        return pointer_vector.length;
}

int ap_memory_print_unreleased()
{
        char **ptr_arr = (char**) pointer_vector.data;
        for (int i = 0; i < pointer_vector.length; ++i) {
                LOGI("ap_memory: unreleased ptr %p", ptr_arr[i]);
        }
        return 0;
}

int ap_memory_release()
{
        LOGI("ap_memory: there are %d pointers unreleased",
                ap_memory_unreleased_num());
        char **ptr_arr = (char**) pointer_vector.data;
        for (int i = 0; i < pointer_vector.length; ++i) {
                AP_FREE(ptr_arr[i]);
        }
        free(pointer_vector.data);
        LOGI("ap_memory: all memory released");
        return 0;
}