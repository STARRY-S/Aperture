/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Custom file IO functions
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
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

// TODO: Implement f* I/O functions for Android and *nix system

#endif // AP_CUSTOM_IO_H