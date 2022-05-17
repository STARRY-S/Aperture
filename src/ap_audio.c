#include "ap_audio.h"
#include "ap_utils.h"
#include "ap_decode.h"

#if !AP_PLATFORM_ANDROID
#include <AL/alut.h>
#endif

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <libavcodec/avcodec.h>

#include <pthread.h>

struct AP_Vector audio_vector = { 0, 0, 0, 0 };

static ALCdevice *device = NULL;
static ALCcontext *context = NULL;
static const ALCchar *device_name = NULL;

static inline void ap_audio_check(const char *msg)
{
        int error = alGetError();
        switch (error)
        {
        case AL_INVALID_NAME:
                LOGE("%s: AL_INVALID_NAME", msg);
                break;
        case AL_INVALID_ENUM:
                LOGE("%s: AL_INVALID_ENUM", msg);
                break;
        case AL_INVALID_VALUE:
                LOGE("%s: AL_INVALID_VALUE", msg);
                break;
        case AL_INVALID_OPERATION:
                LOGE("%s: AL_INVALID_OPERATION", msg);
                break;
        case AL_OUT_OF_MEMORY:
                LOGE("%s: AL_OUT_OF_MEMORY", msg);
                break;
        default:
                break;
        }
}

static inline void ap_audio_check_alut(const char* msg)
{
#if !AP_PLATFORM_ANDROID
        int error = alutGetError();
        const char* reason = NULL;
        switch (error)
        {
        case ALUT_ERROR_OUT_OF_MEMORY:
                reason = "ALUT_ERROR_OUT_OF_MEMORY";
                break;
        case ALUT_ERROR_INVALID_ENUM:
                reason = "ALUT_ERROR_INVALID_ENUM";
                break;
        case ALUT_ERROR_INVALID_VALUE:
                reason = "ALUT_ERROR_INVALID_VALUE";
                break;
        case ALUT_ERROR_INVALID_OPERATION:
                reason = "ALUT_ERROR_INVALID_OPERATION";
                break;
        case ALUT_ERROR_NO_CURRENT_CONTEXT:
                reason = "ALUT_ERROR_NO_CURRENT_CONTEXT";
                break;
        case ALUT_ERROR_AL_ERROR_ON_ENTRY:
                reason = "ALUT_ERROR_AL_ERROR_ON_ENTRY";
                break;
        case ALUT_ERROR_ALC_ERROR_ON_ENTRY:
                reason = "ALUT_ERROR_ALC_ERROR_ON_ENTRY";
                break;
        case ALUT_ERROR_OPEN_DEVICE:
                reason = "ALUT_ERROR_OPEN_DEVICE";
                break;
        case ALUT_ERROR_CLOSE_DEVICE:
                reason = "ALUT_ERROR_CLOSE_DEVICE";
                break;
        case ALUT_ERROR_CREATE_CONTEXT:
                reason = "ALUT_ERROR_CREATE_CONTEXT";
                break;
        case ALUT_ERROR_MAKE_CONTEXT_CURRENT:
                reason = "ALUT_ERROR_MAKE_CONTEXT_CURRENT";
                break;
        case ALUT_ERROR_DESTROY_CONTEXT:
                reason = "ALUT_ERROR_DESTROY_CONTEXT";
                break;
        case ALUT_ERROR_GEN_BUFFERS:
                reason = "ALUT_ERROR_GEN_BUFFERS";
                break;
        case ALUT_ERROR_BUFFER_DATA:
                reason = "ALUT_ERROR_BUFFER_DATA";
                break;
        case ALUT_ERROR_IO_ERROR:
                reason = "ALUT_ERROR_IO_ERROR";
                break;
        case ALUT_ERROR_UNSUPPORTED_FILE_TYPE:
                reason = "ALUT_ERROR_UNSUPPORTED_FILE_TYPE";
                break;
        case ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE:
                reason = "ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE";
                break;
        case ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA:
                reason = "ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA";
                break;
        default:
                break;
        }
        if (reason != NULL) {
                LOGE("%s: %s", msg, reason);
        }
#endif
}

static inline int ap_audio_get_channels_by_AL(int al_fmt)
{
        int channel = 0;
        switch (al_fmt)
        {
        case AL_FORMAT_STEREO8:
        case AL_FORMAT_STEREO16:
#ifdef AL_EXT_float32
        case AL_FORMAT_STEREO_FLOAT32:
#endif // AL_EXT_float32
#ifdef AL_EXT_double
        case AL_FORMAT_STEREO_DOUBLE_EXT:
#endif  // AL_EXT_double
                channel = 2;
                break;
        case AL_FORMAT_MONO8:
        case AL_FORMAT_MONO16:
#ifdef AL_EXT_float32
        case AL_FORMAT_MONO_FLOAT32:
#endif // AL_EXT_float32
#ifdef AL_EXT_double
        case AL_FORMAT_MONO_DOUBLE_EXT:
#endif  // AL_EXT_double
                channel = 1;
                break;
        default:
                break;
        }
        return channel;
}

int ap_audio_fmt_ap_2_al(int ap_audio_fmt, int channel)
{
        int al_fmt = 0;
        switch (ap_audio_fmt)
        {
        case AP_AUDIO_FMT_U8:
                al_fmt = (channel == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
                break;
        case AP_AUDIO_FMT_S16:
                al_fmt = (channel == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
                break;
        case AP_AUDIO_FMT_S32:
                // unsupported yet
                break;
        case AP_AUDIO_FMT_S64:
                // unsupported yet
                break;
        case AP_AUDIO_FMT_FLT:
                al_fmt = (channel == 1)
                        ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
                break;
        case AP_AUDIO_FMT_DBL:
                al_fmt = (channel == 1) ? AL_FORMAT_MONO_DOUBLE_EXT
                        : AL_FORMAT_STEREO_DOUBLE_EXT;
                break;
        default:
                break;
        }

        return al_fmt;
}

int ap_audio_fmt_av_2_ap(int av_fmt, const char *s)
{
        int fmt = 0;
        switch (av_fmt)
        {
        case AV_SAMPLE_FMT_U8:
        case AV_SAMPLE_FMT_U8P:
                fmt = AP_AUDIO_FMT_U8;
                break;
        case AV_SAMPLE_FMT_S16:
        case AV_SAMPLE_FMT_S16P:
                fmt = AP_AUDIO_FMT_S16;
                break;
        case AV_SAMPLE_FMT_S32:
        case AV_SAMPLE_FMT_S32P:
                fmt = AP_AUDIO_FMT_S32;
                break;
        case AV_SAMPLE_FMT_FLT:
        case AV_SAMPLE_FMT_FLTP:
                fmt = AP_AUDIO_FMT_FLT;
                break;
        case AV_SAMPLE_FMT_DBL:
        case AV_SAMPLE_FMT_DBLP:
                fmt = AP_AUDIO_FMT_DBL;
                break;
        case AV_SAMPLE_FMT_S64:
        case AV_SAMPLE_FMT_S64P:
                fmt = AP_AUDIO_FMT_S64;
        default:
                break;
        }
        if (fmt == 0) {
                LOGW("ap_audio_fmt_av_2_ap: unrecognized format %d %s",
                        av_fmt, s);
        }
        return fmt;
}

int ap_audio_fmt_al_2_ap(int al_fmt)
{
        int fmt = AP_AUDIO_FMT_UNKNOWN;
        switch (al_fmt)
        {
        case AL_FORMAT_STEREO8:
        case AL_FORMAT_MONO8:
                fmt = AP_AUDIO_FMT_U8;
                break;
        case AL_FORMAT_MONO16:
        case AL_FORMAT_STEREO16:
                fmt = AP_AUDIO_FMT_S16;
                break;
#ifdef AL_EXT_float32
        case AL_FORMAT_MONO_FLOAT32:
        case AL_FORMAT_STEREO_FLOAT32:
                fmt = AP_AUDIO_FMT_FLT;
                break;
#endif // AL_EXT_float32
#ifdef AL_EXT_double
        case AL_FORMAT_MONO_DOUBLE_EXT:
        case AL_FORMAT_STEREO_DOUBLE_EXT:
                fmt = AP_AUDIO_FMT_DBL;
                break;
#endif  // AL_EXT_double
        default:
                break;
        }
        if (fmt == 0) {
                LOGW("ap_audio_fmt_al_2_ap: unknow format %d", al_fmt);
        }
        return fmt;
}

int ap_audio_init()
{
#if !AP_PLATFORM_ANDROID
        int ret = 0;
        ret = alutInitWithoutContext(NULL, NULL);
        if (ret == 0) {
                ap_audio_check_alut("alutInitWithoutContext");
                return AP_ERROR_INIT_FAILED;
        }
#endif

        if (device_name == NULL) {
                device_name = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	        LOGD("ap_audio device name: %s", device_name);
        }

        if (device == NULL) {
                device = alcOpenDevice(device_name);
                if (!device) {
                        LOGE("failed to open device name %s", device_name);
                        return AP_ERROR_INIT_FAILED;
                }
        }

        // Clear the error buffer
        alGetError();

        if (context == NULL) {
                context = alcCreateContext(device, NULL);
                if (!alcMakeContextCurrent(context)) {
	                ap_audio_check("alcMakeContextCurrent");
                        return AP_ERROR_INIT_FAILED;
                }
        }

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	ap_audio_check("alListener3f");
    	alListener3f(AL_VELOCITY, 0, 0, 0);
	ap_audio_check("alListener3f");
	ALfloat orientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListenerfv(AL_ORIENTATION, orientation);
	ap_audio_check("alListenerfv");

        // initialize vector
        ap_vector_init(&audio_vector, AP_VECTOR_AUDIO);

        return 0;
}

static void* ap_audio_play_thread_func(void* data)
{
        struct AP_Audio *audio = (struct AP_Audio*) data;
        // LOGD("ap_audio_play_buffer_func: id %u", audio->buffer_id);
        ALint source_state = 0;
        alSourcePlay(audio->source_id);
        alGetSourcei(audio->source_id, AL_SOURCE_STATE, &source_state);
        while (source_state == AL_PLAYING || source_state == AL_PAUSED) {
                alGetSourcei(audio->source_id, AL_SOURCE_STATE, &source_state);
        }
        AP_FREE(audio);
        return NULL;
}

static inline int ap_audio_play_ptr(
        const struct AP_Audio *audio,
        ap_callback_func_t cb)
{
        if (audio == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (audio->source_id == 0) {
                LOGW("failed to play audio: unknown source id");
                return AP_ERROR_INVALID_PARAMETER;
        }

        pthread_t pid = 0;
        struct AP_Audio *tmp_audio = AP_MALLOC(sizeof(struct AP_Audio));
        memcpy(tmp_audio, audio, sizeof(struct AP_Audio));
        tmp_audio->cb = (cb) ? cb : NULL;
        pthread_create(&pid, NULL, ap_audio_play_thread_func, tmp_audio);

        return 0;
}

static inline int ap_audio_pause_ptr(const struct AP_Audio *audio)
{
        if (audio == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (audio->source_id == 0) {
                LOGW("failed to play audio: unknown source id");
                return AP_ERROR_INVALID_PARAMETER;
        }

        alSourcePause(audio->source_id);
        return 0;
}

static inline int ap_audio_stop_ptr(const struct AP_Audio *audio)
{
        if (audio == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (audio->source_id == 0) {
                LOGW("failed to play audio: unknown source id");
                return AP_ERROR_INVALID_PARAMETER;
        }

        alSourceStop(audio->source_id);
        return 0;
}

static inline int ap_audio_struct_init(struct AP_Audio *audio)
{
        if (audio == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        memset(audio, 0, sizeof(struct AP_Audio));

        return 0;
}

static inline int ap_audio_open_file_WAV_ptr(
        const char* filename,
        struct AP_Audio **out_audio_p)
{
#if AP_PLATFORM_ANDROID
        LOGE("failed to open WAV file: ALUT is not supported yet");
#else
        struct AP_Audio *out_audio = *out_audio_p;
        if (out_audio == NULL) {
                out_audio = *out_audio_p = AP_MALLOC(sizeof(struct AP_Audio));
                if (out_audio == NULL) {
                        return AP_ERROR_MALLOC_FAILED;
                }
                ap_audio_struct_init(out_audio);
        }

        ALsizei size = 0;
        ALfloat frequency = 0.0f;
        ALenum format = 0;
        ALvoid* data = alutLoadMemoryFromFile(
                filename, &format, &size, &frequency);
        ap_audio_check_alut("alutLoadMemoryFromFile");
        out_audio->name = AP_MALLOC((strlen(filename) + 1) * sizeof(char));
        strcpy(out_audio->name, filename);
        out_audio->data = data;
        out_audio->data_size = size;
        out_audio->channels = ap_audio_get_channels_by_AL(format);
        out_audio->format = ap_audio_fmt_al_2_ap(format);
        out_audio->frequency = frequency;

        // clear error
        alGetError();

        ALuint buffer = 0;
        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("alGenBuffers");
                return AP_AUDIO_BUFFER_GEN_FAILED;
        }
        alBufferData(buffer, format, data, size, frequency);
        ap_audio_check("alBufferData");

        // generate a source
        ALuint source;
        alGenSources((ALuint)1, &source);
        ap_audio_check("alGenSources");

        alSourcef(source, AL_PITCH, 1);
        ap_audio_check("alSourcef");

        alSourcef(source, AL_GAIN, 1);
        ap_audio_check("alSourcef");

        alSource3f(source, AL_POSITION, 0, 0, 0);
        ap_audio_check("alSource3f");

        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        ap_audio_check("alSource3f");

        alSourcei(source, AL_LOOPING, false);
        ap_audio_check("alSourcei");

        alSourcei(source, AL_BUFFER, buffer);
        ap_audio_check("alSourcei");
        out_audio->source_id = source;
        alDeleteBuffers(1, &buffer);
#endif

        return 0;
}

static inline int ap_audio_open_file_MP3_ptr(
        const char* filename,
        struct AP_Audio **out_audio_p)
{
        struct AP_Audio *out_audio = *out_audio_p;
        if (out_audio == NULL) {
                out_audio = *out_audio_p = AP_MALLOC(sizeof(struct AP_Audio));
                if (out_audio == NULL) {
                        return AP_ERROR_MALLOC_FAILED;
                }
                ap_audio_struct_init(out_audio);
        }

        float frequency = 0.0f;
        int format = 0, channels = 0;
        struct AP_Vector *tmp_vec = NULL;
        ap_decode_to_memory(filename, &tmp_vec, &format, &frequency, &channels);
        if (!tmp_vec || tmp_vec->length == 0 || channels == 0) {
                return AP_ERROR_DECODE_FAILED;
        }

        out_audio->name = AP_MALLOC((strlen(filename) + 1) * sizeof(char));
        strcpy(out_audio->name, filename);
        out_audio->data = tmp_vec->data;
        out_audio->data_size = tmp_vec->length;
        out_audio->channels = channels;
        out_audio->format = format;
        out_audio->frequency = frequency;

        // clear error
        alGetError();

        ALuint buffer = 0;
        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("alGenBuffers");
                return AP_AUDIO_BUFFER_GEN_FAILED;
        }
        int al_fmt = ap_audio_fmt_ap_2_al(format, out_audio->channels);
        alBufferData(
                buffer, al_fmt, tmp_vec->data,
                out_audio->data_size, frequency
        );
        ap_audio_check("alBufferData");

        // generate a source
        ALuint source;
        alGenSources((ALuint)1, &source);
        ap_audio_check("alGenSources");

        alSourcef(source, AL_PITCH, 1);
        ap_audio_check("alSourcef");

        alSourcef(source, AL_GAIN, 1);
        ap_audio_check("alSourcef");

        alSource3f(source, AL_POSITION, 0, 0, 0);
        ap_audio_check("alSource3f");

        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        ap_audio_check("alSource3f");

        alSourcei(source, AL_LOOPING, false);
        ap_audio_check("alSourcei");

        alSourcei(source, AL_BUFFER, buffer);
        ap_audio_check("alSourcei");
        out_audio->source_id = source;
        AP_FREE(tmp_vec);
        alDeleteBuffers(1, &buffer);

        return 0;
}

static inline int ap_audio_release_ptr(struct AP_Audio *audio)
{
        if (audio == NULL) {
                return 0;
        }

        if (audio->name != NULL) {
                AP_FREE(audio->name);
        }

        if (audio->data != NULL) {
                AP_FREE(audio->data);
        }

        if (audio->source_id) {
                alDeleteSources(1, &audio->source_id);
        }

        memset(audio, 0, sizeof(struct AP_Audio));
        return 0;
}

static inline struct AP_Audio* ap_audio_get_ptr(unsigned int id)
{
        if (id == 0) {
                return 0;
        }

        struct AP_Audio *data = (struct AP_Audio*) audio_vector.data;
        struct AP_Audio *ptr = NULL;
        for (int i = 0; i < audio_vector.length; ++i) {
                ptr = data + i;
                if (ptr->id == id) {
                        return ptr;
                }
        }

        LOGW("audio id %u not found", id);
        return NULL;
}

int ap_audio_load_MP3(const char *name, unsigned int *id)
{
        if (id == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Audio *audio = NULL;
        *id = 0;
        int ret = ap_audio_open_file_MP3_ptr(name, &audio);
        if (audio == NULL || ret != 0) {
                return ret;
        }
        audio->id = audio_vector.length + 1;
        ret = ap_vector_push_back(&audio_vector, (const char*) audio);
        if (ret != 0) {
                ap_audio_release_ptr(audio);
                AP_CHECK(ret);
                AP_FREE(audio);
        } else {
                AP_FREE(audio);
                *id = audio_vector.length;
        }

        return ret;
}

int ap_audio_load_WAV(const char *name, unsigned int *id)
{
        if (id == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_Audio *audio = NULL;
        *id = 0;
        int ret = ap_audio_open_file_WAV_ptr(name, &audio);
        if (audio == NULL || ret != 0) {
                return ret;
        }
        audio->id = audio_vector.length + 1;
        ret = ap_vector_push_back(&audio_vector, (const char*) audio);
        if (ret != 0) {
                ap_audio_release_ptr(audio);
                AP_CHECK(ret);
                AP_FREE(audio);
        } else {
                AP_FREE(audio);
                *id = audio_vector.length;
        }

        return ret;
}

int ap_audio_play(unsigned int id, ap_callback_func_t cb)
{
        struct AP_Audio *ptr = ap_audio_get_ptr(id);
        return ap_audio_play_ptr(ptr, cb);
}

int ap_audio_pause(unsigned int id)
{
        struct AP_Audio *ptr = ap_audio_get_ptr(id);
        return ap_audio_pause_ptr(ptr);
}

int ap_audio_stop(unsigned int id)
{
        struct AP_Audio *ptr = ap_audio_get_ptr(id);
        return ap_audio_stop_ptr(ptr);
}

int ap_audio_free()
{
        device = alcGetContextsDevice(context);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);

#if !AP_PLATFORM_ANDROID
        alutExit();
#endif
        struct AP_Audio *ptr, *data = NULL;
        data = (struct AP_Audio*) audio_vector.data;
        for (int i = 0; i < audio_vector.length; ++i) {
                ptr = data + i;
                ap_audio_release_ptr(ptr);
        }

        ap_vector_free(&audio_vector);

        return 0;
}
