#include <GLES3/gl3.h>
#include <assimp/cfileio.h>

#include "custom_io.h"
#include "ge_utils.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "GE_CUSTOM_IO"
#endif

struct aiFile* customFileOpenProc(
        C_STRUCT aiFileIO* customIO, const char* pFileName, const char* pMode)
{
    if (customIO == NULL || pFileName == NULL || pMode == NULL) {
        // AP_ERROR_INVALID_POINTER;
        return NULL;
    }

    #ifdef __ANDROID__
    AAssetManager *pManager = ap_get_local_asset_manager();
    AAsset *pathAsset = AAssetManager_open(pManager, pFileName,
                                           AASSET_MODE_RANDOM);
    if (pathAsset == NULL) {
        // AP_ERROR_ASSET_OPEN_FAILED;
        return NULL;
    }
    #else
    FILE *fp = NULL;
    if (!(fp = fopen(pFileName, pMode))) {
        LOGE("Failed to open: %s", pFileName);
        return NULL;
    }
    #endif

    struct aiFile *pAiFile = malloc(sizeof(struct aiFile));
    if (pAiFile == NULL) {
        // AP_ERROR_MALLOC_FAILED;
        return NULL;
    }
    memset(pAiFile, 0, sizeof(struct aiFile));

    pAiFile->ReadProc = customFileReadProc;
    pAiFile->WriteProc = customFileWriteProc;
    pAiFile->TellProc = customFileTellProc;
    pAiFile->FileSizeProc = customFileSizeProc;
    pAiFile->SeekProc = customFileSeek;
    pAiFile->FlushProc = customFileFlushProc;
    #ifdef __ANDROID__
    pAiFile->UserData = (char *) pathAsset;
    #else
    pAiFile->UserData = (char *) fp;
    #endif

    LOGD("CustomIO: Opened file %s\n", pFileName);
    return pAiFile;
}

void customFileCloseProc(C_STRUCT aiFileIO* pAiFileIO, C_STRUCT aiFile* pAiFile)
{
    #ifdef __ANDROID__
    AAsset_close((AAsset *) pAiFile->UserData);
    #else
    fclose((FILE*) pAiFile->UserData);
    #endif
    LOGD("Closed file.");
}

size_t customFileReadProc(C_STRUCT aiFile* pAiFile, char* pBuffer, size_t size, size_t count)
{
    #ifdef __ANDROID__
    return AAsset_read((AAsset *) pAiFile->UserData, pBuffer, size * count);
    #else
    return fread(pBuffer, size, count, (FILE*) pAiFile->UserData);
    #endif
}

size_t customFileWriteProc(C_STRUCT aiFile* pAiFile,
                          const char* pBuffer, size_t size, size_t count)
{
    // do nothing~
    return 0;
}

size_t customFileTellProc(C_STRUCT aiFile* pAiFile)
{
    #ifdef __ANDROID__

    long iRemainLength = AAsset_getRemainingLength((AAsset*) pAiFile->UserData);
    long iTotalLength = AAsset_getLength((AAsset*) pAiFile->UserData);
    return iTotalLength - iRemainLength;

    #else

    return ftell((FILE*) pAiFile->UserData);

    #endif
}

size_t customFileSizeProc(C_STRUCT aiFile* pAiFile)
{
    #ifdef __ANDROID__

    return AAsset_getLength((AAsset*) pAiFile->UserData);

    #else

    int iCurrent = ftell((FILE*) pAiFile->UserData);
    if (iCurrent < 0) {
        LOGE("ftell error\n");
        return 0;
    }
    fseek((FILE*) pAiFile->UserData, 0l, SEEK_END);
    int iLength = ftell((FILE*) pAiFile->UserData);
    if (iLength < 0) {
        LOGE("ftell error\n");
        return 0;
    }
    fseek((FILE*) pAiFile->UserData, iCurrent, SEEK_SET);
    return iLength;

    #endif
}

void customFileFlushProc(C_STRUCT aiFile* pAiFile)
{
    #ifndef __ANDROID__
    fflush((FILE*) pAiFile->UserData);
    #endif
}

C_ENUM aiReturn customFileSeek(C_STRUCT aiFile* pAiFile,
                                size_t offset,
                                C_ENUM aiOrigin origin)
{
    int iRet = -1;

    #ifdef __ANDROID__

    iRet = AAsset_seek((AAsset *) pAiFile->UserData, (long) offset, origin);
    if (iRet >= 0) {
        return 0;
    }

    return -1;

    #else

    switch (origin)
    {
    case aiOrigin_CUR:
        iRet = fseek((FILE*) pAiFile->UserData, offset, SEEK_CUR);
        break;
    case aiOrigin_END:
        iRet = fseek((FILE*) pAiFile->UserData, offset, SEEK_END);
        break;
    case aiOrigin_SET:
        iRet = fseek((FILE*) pAiFile->UserData, offset, SEEK_SET);
        break;
    default:
        break;
    }
    if (iRet != 0) {
        return -1;
    }

    return 0;

    #endif
}