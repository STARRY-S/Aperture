#include "ap_utils.h"
#include "ap_custom_io.h"

#include <assimp/cfileio.h>

#if AP_PLATFORM_ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#elif AP_PLATFORM_WINDOWS
#include <io.h>
#else
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "AP_CUSTOM_IO"
#endif

struct aiFile* ap_custom_ai_file_open_proc(
        C_STRUCT aiFileIO* custom_io,
        const char* file_name,
        const char* mode)
{
        if (custom_io == NULL || file_name == NULL || mode == NULL) {
                LOGE("ap_custom_ai_file_open_proc: invalid parameter");
                return NULL;
        }

#if AP_PLATFORM_ANDROID
        AAssetManager *manager = ap_get_asset_manager();
        AAsset *asset_path = AAssetManager_open(
                manager, file_name,
                AASSET_MODE_RANDOM
        );
        if (asset_path == NULL) {
                LOGE("ap_custom_ai_file_open_proc: AAssetManager_open failed");
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
                LOGE("ap_custom_ai_file_open_proc: malloc failed");
                return NULL;
        }
        memset(ai_file, 0, sizeof(struct aiFile));

        ai_file->ReadProc     = ap_custom_ai_file_read_proc;
        ai_file->WriteProc    = ap_custom_ai_file_write_proc;
        ai_file->TellProc     = ap_custom_ai_ftell_proc;
        ai_file->FileSizeProc = ap_custom_ai_fsize_proc;
        ai_file->SeekProc     = ap_custom_ai_fseek_proc;
        ai_file->FlushProc    = ap_custom_ai_fflush_proc;
#if AP_PLATFORM_ANDROID
        ai_file->UserData = (char *) asset_path;
#else
        ai_file->UserData = (char *) fp;
#endif

        return ai_file;
}

void ap_custom_ai_file_close_proc(
        C_STRUCT aiFileIO* ai_file_io,
        C_STRUCT aiFile* ai_file)
{
#if AP_PLATFORM_ANDROID
        AAsset_close((AAsset *) ai_file->UserData);
#else
        fclose((FILE*) ai_file->UserData);
#endif
        AP_FREE(ai_file);
}

size_t ap_custom_ai_file_read_proc(
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

size_t ap_custom_ai_file_write_proc(
        C_STRUCT aiFile* ai_file,
        const char* buffer,
        size_t size,
        size_t count)
{
        // do nothing~
        return 0;
}

size_t ap_custom_ai_ftell_proc(C_STRUCT aiFile* ai_file)
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

size_t ap_custom_ai_fsize_proc(C_STRUCT aiFile* ai_file)
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

void ap_custom_ai_fflush_proc(C_STRUCT aiFile* ai_file)
{
        #if !AP_PLATFORM_ANDROID
        fflush((FILE*) ai_file->UserData);
        #endif
}

C_ENUM aiReturn ap_custom_ai_fseek_proc(
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

int ap_read_file_to_memory(const char *file, char **ptr, int *length)
{
        if (!file || !ptr || !length) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        *length = 0;
        *ptr = NULL;
        char *buffer = NULL;
#if AP_PLATFORM_ANDROID
        AAssetManager *mgr = ap_get_asset_manager();
        if (!mgr) {
                LOGE("ap_read_file_to_memory failed: asset manager is NULL");
                return AP_ERROR_INIT_FAILED;
        }
        AAsset *m_asset = AAssetManager_open(mgr, file, AASSET_MODE_UNKNOWN);
        if (m_asset == NULL) {
                LOGE("ap_read_file_to_memory failed: %s", file);
                return AP_ERROR_ASSET_OPEN_FAILED;
        }
        int file_length = AAsset_getLength(m_asset);
        char *file_data = (char *) AAsset_getBuffer(m_asset);
        if (!file_length || !file_data) {
                LOGE("AAsset_getLength: %d", file_length);
                LOGE("AAsset_getBuffer: %p", file_data);
                AAsset_close(m_asset);
                return AP_ERROR_ASSET_OPEN_FAILED;
        }
        buffer = AP_MALLOC(file_length + 1);
        if (!buffer) {
                AAsset_close(m_asset);
                return AP_ERROR_MALLOC_FAILED;
        }
        memset(buffer, 0, file_length + 1);
        memcpy(buffer, file_data, file_length);
        *ptr = buffer;
        *length = file_length;
        AAsset_close(m_asset);
// TODO: Windows
// #elif AP_PLATFORM_WINDOWS
#else
        int fd = open(file, O_RDONLY);
        if (fd < 0) {
                LOGE("ap_read_file_to_memory open failed: %d", errno);
                return AP_ERROR_OPEN_FILE_FAILED;
        }
        int file_length = lseek(fd, 0, SEEK_END);
        if (file_length < 0) {
                LOGE("ap_read_file_to_memory lseek failed: %d", errno);
                close(fd);
                return AP_ERROR_OPEN_FILE_FAILED;
        }
        lseek(fd, 0, SEEK_SET);
        buffer = AP_MALLOC(file_length);
        memset(buffer, 0, file_length);
        int ret = 0;
        int c = '\0';
        for (int i = 0; ((ret = read(fd, &c, 1)) > 0); ++i) {
                buffer[i] = c;
        }
        if (ret < 0) {
                LOGE("ap_read_file_to_memory read failed: %d", errno);
                close(fd);
                return AP_ERROR_OPEN_FILE_FAILED;
        }
        close(fd);
        *ptr = buffer;
        *length = file_length;
#endif

        return 0;
}

int ap_open_file_descriptor(const char *file, int *pfd)
{
        if (!file || !pfd) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        *pfd = 0;
#if AP_PLATFORM_ANDROID
        AAssetManager *mgr = (AAssetManager *) ap_get_asset_manager();
        if (!mgr) {
                LOGE("ap_open_file_descriptor failed: asset manager is NULL");
                return 0;
        }
        AAsset *m_asset = AAssetManager_open(mgr, file, AASSET_MODE_UNKNOWN);
        if (m_asset == NULL) {
                LOGE("ap_open_file_descriptor failed: %s", file);
                return AP_ERROR_OPEN_FILE_FAILED;
        }
        off_t offset, length;
        int fd = AAsset_openFileDescriptor(m_asset, &offset, &length);
        AAsset_close(m_asset);
        if (fd <= 0) {
                LOGW("ap_open_file_descriptor: file descriptor opened is 0");
        }
        *pfd = fd;
// #elif AP_PLATFORM_WINDOWS
// TODO: windows
#else
        int fd = open(file, O_RDONLY);
        if (fd < 0) {
                LOGE("ap_open_file_descriptor open failed: %d", errno);
                return AP_ERROR_OPEN_FILE_FAILED;
        }
        *pfd = fd;
#endif
        return 0;
}

int ap_close_file_descriptor(int fd)
{
        close(fd);
        return 0;
}
