/**
 * @file ap_custom_io.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Custom I/O functions
 *
 */
#ifndef AP_CUSTOM_IO_H
#define  AP_CUSTOM_IO_H

#include <assimp/cfileio.h>

struct aiFile* ap_custom_ai_file_open_proc(
        C_STRUCT aiFileIO* custom_io,
        const char* file_name,
        const char* mode
);

void ap_custom_ai_file_close_proc(
        C_STRUCT aiFileIO* ai_file_io,
        C_STRUCT aiFile* ai_file
);

size_t ap_custom_ai_file_read_proc(
        C_STRUCT aiFile* ai_file,
        char* buffer,
        size_t size,
        size_t count
);

size_t ap_custom_ai_file_write_proc(
        C_STRUCT aiFile* ai_file,
        const char* buffer,
        size_t size,
        size_t count
);

size_t ap_custom_ai_ftell_proc(C_STRUCT aiFile* ai_file);

size_t ap_custom_ai_fsize_proc(C_STRUCT aiFile* ai_file);


void ap_custom_ai_fflush_proc(C_STRUCT aiFile* ai_file);

C_ENUM aiReturn ap_custom_ai_fseek_proc(
        C_STRUCT aiFile* ai_file,
        size_t offset,
        C_ENUM aiOrigin origin
);

/**
 * @brief read file to memory
 * should free the pointer after use
 * @param file file name
 * @param ptr    [out] pointer points to the pointer of the buffer
 * @param length [out] file length
 * @return int AP_Error_Types
 */
int ap_read_file_to_memory(const char *file, char **ptr, int *length);

/**
 * @brief open file and get its file descriptor
 * should close this fd after use
 * @param file file name
 * @param pfd [out] pointer points to file descriptor
 * @return int AP_Error_Types
 */
int ap_open_file_descriptor(const char *file, int *pfd);

/**
 * @brief close file descriptor opened by ap_open_file_descriptor
 *
 * @param fd
 * @return int AP_Error_Types
 */
int ap_close_file_descriptor(int fd);

#endif // AP_CUSTOM_IO_H