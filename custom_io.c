#include <GLES3/gl3.h>
#include <assimp/cfileio.h>
#include <android/asset_manager.h>

#include "custom_io.h"
#include "main.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "GE_CUSTOM_IO"
#endif

struct aiFile* customFileOpenProc(
        C_STRUCT aiFileIO* customIO, const char* pFileName, const char* pMode)
{
    GE_errorno = GE_ERROR_SUCCESS;
    if (customIO == NULL || pFileName == NULL) {
        GE_errorno = GE_ERROR_INVALID_POINTER;
        return NULL;
    }

    AAssetManager *pManager = getLocalAAssetManager();
    AAsset *pathAsset = AAssetManager_open(pManager, pFileName,
                                           AASSET_MODE_RANDOM);
    if (pathAsset == NULL) {
        GE_errorno = GE_ERROR_ASSET_OPEN_FAILED;
        return NULL;
    }
    struct aiFile *pAiFile = malloc(sizeof(struct aiFile));
    if (pAiFile == NULL) {
        GE_errorno = GE_ERROR_MALLOC_FAILED;
        return NULL;
    }
    memset(pAiFile, 0, sizeof(struct aiFile));

    pAiFile->ReadProc = customFileReadProc;
    pAiFile->WriteProc = customFileWriteProc;
    pAiFile->TellProc = customFileTellProc;
    pAiFile->FileSizeProc = customFileSizeProc;
    pAiFile->SeekProc = customFileSeek;
    pAiFile->FlushProc = customFileFlushProc;
    pAiFile->UserData = (char *) pathAsset;

    LOGD("CustomIO: Opened file %s", pFileName);
    return pAiFile;
}

void customFileCloseProc(C_STRUCT aiFileIO* pAiFileIO, C_STRUCT aiFile* pAiFile)
{
    AAsset_close((AAsset *) pAiFile->UserData);
}

size_t customFileReadProc(C_STRUCT aiFile* pAiFile, char* pBuffer, size_t size, size_t count)
{
    return AAsset_read((AAsset *) pAiFile->UserData, pBuffer, size * count);
}

size_t customFileWriteProc(C_STRUCT aiFile* pAiFile,
                          const char* pBuffer, size_t size, size_t count)
{
    // do nothing~
    return 0;
}

size_t customFileTellProc(C_STRUCT aiFile* pAiFile)
{
    long iRemainLength = AAsset_getRemainingLength((AAsset*) pAiFile->UserData);
    long iTotalLength = AAsset_getLength((AAsset*) pAiFile->UserData);
    return iTotalLength - iRemainLength;
}

size_t customFileSizeProc(C_STRUCT aiFile* pAiFile)
{
    return AAsset_getLength((AAsset*) pAiFile->UserData);
}

void customFileFlushProc(C_STRUCT aiFile* pAiFile)
{
    // do nothing~
}

C_ENUM aiReturn customFileSeek(C_STRUCT aiFile* pAiFile, size_t offset,
                                      C_ENUM aiOrigin origin)
{
    int iRet = AAsset_seek((AAsset *) pAiFile->UserData, (long) offset, origin);
    if (iRet >= 0) {
        return 0;
    }
    return -1;
}