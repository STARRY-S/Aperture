/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Custom file IO functions
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */
#ifndef AP_CUSTOM_IO_H
#define  AP_CUSTOM_IO_H

#include <assimp/cfileio.h>

/**
 * @brief Custom Open File Proc, like fopen()
 * @param customIO
 * @param pFileName File name
 * @param pMode Reserve, not used
 * @return pointer points to struct aiFile, by malloc, need free,
 */
struct aiFile* ap_custom_file_open_proc(
        C_STRUCT aiFileIO* customIO,
        const char* pFileName,
        const char* pMode
);

/**
 * @brief Custom close file proc, like fclose()
 * @param pAiFileIO
 * @param pAiFile
 */
void ap_custom_file_close_proc(
        C_STRUCT aiFileIO* pAiFileIO,
        C_STRUCT aiFile* pAiFile
);

/**
 * @brief custom read file proc, like fread()
 * @param pAiFile
 * @param pBuffer
 * @param size
 * @param count
 * @return
 */
size_t ap_custom_file_read_proc(
        C_STRUCT aiFile* pAiFile,
        char* pBuffer,
        size_t size,
        size_t count
);

/**
 * @brief Custom fwrite proc, it won't do anything here, because currently I do not know
 * how to write file by android asset manager.
 * @param pAiFile reserve
 * @param pBuffer reserve
 * @param size reserve
 * @param count reserve
 * @return always zero.
 */
size_t ap_custom_file_write_proc(
        C_STRUCT aiFile* pAiFile,
        const char* pBuffer,
        size_t size,
        size_t count
);

/**
 * Custom ftell function, returns the current file position.
 * @param pAiFile
 * @return current file position
 */
size_t ap_custom_ftell_proc(C_STRUCT aiFile* pAiFile);

/**
 * Custom file size function.
 * @param pAiFile
 * @return file size.
 */
size_t ap_custom_fsize_proc(C_STRUCT aiFile* pAiFile);

/**
 * @brief Custom fflush function (it won't do anything here)
 * @param pAiFile
 */
void ap_custom_fflush_proc(C_STRUCT aiFile* pAiFile);

/**
 * @brief custom fseek proc.
 * @param pAiFile
 * @param offset
 * @return 0 if succeed, -1 on error
 */
C_ENUM aiReturn ap_custom_fseek_proc(
        C_STRUCT aiFile* pAiFile,
        size_t offset,
        C_ENUM aiOrigin
);

#endif // AP_CUSTOM_IO_H