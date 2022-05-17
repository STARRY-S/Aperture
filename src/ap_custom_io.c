#include "ap_utils.h"
#include "ap_custom_io.h"

#include <assimp/cfileio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "AP_CUSTOM_IO"
#endif

struct aiFile* ap_custom_file_open_proc(
        C_STRUCT aiFileIO* custom_io,
        const char* file_name,
        const char* mode)
{
        if (custom_io == NULL || file_name == NULL || mode == NULL) {
                // AP_ERROR_INVALID_POINTER;
                return NULL;
        }

#if AP_PLATFORM_ANDROID
        AAssetManager *manager = ap_get_asset_manager();
        AAsset *asset_path = AAssetManager_open(
                manager, file_name,
                AASSET_MODE_RANDOM
        );
        if (asset_path == NULL) {
                // AP_ERROR_ASSET_OPEN_FAILED;
                return NULL;
        }
#else
        FILE *fp = NULL;
        if (!(fp = fopen(file_name, mode))) {
                LOGE("Failed to open: %s", file_name);
                return NULL;
        }
#endif

        struct aiFile *ai_file = AP_MALLOC(sizeof(struct aiFile));
        if (ai_file == NULL) {
                // AP_ERROR_MALLOC_FAILED;
                return NULL;
        }
        memset(ai_file, 0, sizeof(struct aiFile));

        ai_file->ReadProc     = ap_custom_file_read_proc;
        ai_file->WriteProc    = ap_custom_file_write_proc;
        ai_file->TellProc     = ap_custom_ftell_proc;
        ai_file->FileSizeProc = ap_custom_fsize_proc;
        ai_file->SeekProc     = ap_custom_fseek_proc;
        ai_file->FlushProc    = ap_custom_fflush_proc;
#if AP_PLATFORM_ANDROID
        ai_file->UserData = (char *) asset_path;
#else
        ai_file->UserData = (char *) fp;
#endif

        return ai_file;
}

void ap_custom_file_close_proc(
        C_STRUCT aiFileIO* ai_file_io,
        C_STRUCT aiFile* ai_file)
{
#if AP_PLATFORM_ANDROID
        AAsset_close((AAsset *) ai_file->UserData);
#else
        fclose((FILE*) ai_file->UserData);
#endif
}

size_t ap_custom_file_read_proc(
        C_STRUCT aiFile* ai_file,
        char* buffer,
        size_t size,
        size_t count)
{
#if AP_PLATFORM_ANDROID
        return AAsset_read(
                (AAsset *) ai_file->UserData, buffer, size * count
        );
#else
        return fread(buffer, size, count, (FILE*) ai_file->UserData);
#endif
}

size_t ap_custom_file_write_proc(
        C_STRUCT aiFile* ai_file,
        const char* buffer,
        size_t size,
        size_t count)
{
        // do nothing~
        return 0;
}

size_t ap_custom_ftell_proc(C_STRUCT aiFile* ai_file)
{
#if AP_PLATFORM_ANDROID

        long iRemainLength =
                AAsset_getRemainingLength((AAsset*) ai_file->UserData);
        long iTotalLength = AAsset_getLength((AAsset*) ai_file->UserData);
        return iTotalLength - iRemainLength;
#else
        return ftell((FILE*) ai_file->UserData);
#endif
}

size_t ap_custom_fsize_proc(C_STRUCT aiFile* ai_file)
{
#if AP_PLATFORM_ANDROID
        return AAsset_getLength((AAsset*) ai_file->UserData);
#else
        int current = ftell((FILE*) ai_file->UserData);
        if (current < 0) {
                LOGE("ftell error");
                return 0;
        }
        fseek((FILE*) ai_file->UserData, 0l, SEEK_END);
        int length = ftell((FILE*) ai_file->UserData);
        if (length < 0) {
                LOGE("ftell error");
                return 0;
        }
        fseek((FILE*) ai_file->UserData, current, SEEK_SET);
        return length;
#endif
}

void ap_custom_fflush_proc(C_STRUCT aiFile* ai_file)
{
        #if !AP_PLATFORM_ANDROID
        fflush((FILE*) ai_file->UserData);
        #endif
}

C_ENUM aiReturn ap_custom_fseek_proc(
        C_STRUCT aiFile* ai_file,
        size_t offset,
        C_ENUM aiOrigin origin)
{
        int ret = -1;

#if AP_PLATFORM_ANDROID

        ret = AAsset_seek(
                (AAsset *) ai_file->UserData,
                (long) offset,
                origin
        );
        if (ret >= 0) {
                return 0;
        }

        return -1;

#else

        switch (origin)
        {
        case aiOrigin_CUR:
                ret = fseek((FILE*) ai_file->UserData, offset, SEEK_CUR);
                break;
        case aiOrigin_END:
                ret = fseek((FILE*) ai_file->UserData, offset, SEEK_END);
                break;
        case aiOrigin_SET:
                ret = fseek((FILE*) ai_file->UserData, offset, SEEK_SET);
                break;
        default:
                break;
        }
        if (ret != 0) {
                return -1;
        }

        return 0;

#endif
}