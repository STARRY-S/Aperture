#include "ap_audio.h"
#include "ap_utils.h"
#include "ap_decode.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/alext.h>
#include <libavcodec/avcodec.h>

#include <pthread.h>

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
}

static inline int ap_audio_fmt_al_2_ap(int al_fmt)
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

static inline int ap_audio_fmt_ap_2_al(int ap_audio_fmt, int channel)
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

int ap_audio_init()
{
        int res = 0;
        res = alutInitWithoutContext(NULL, NULL);
        if (res == 0) {
                ap_audio_check_alut("alutInitWithoutContext");
                return AP_ERROR_INIT_FAILED;
        }

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
        LOGD("ap_audio_init finished");

        return 0;
}

unsigned int ap_audio_load_buffer_WAV(const char* name)
{
        ALsizei size = 0;
        ALfloat frequency = 0.0f;
        ALenum format = 0;
        ALvoid* data = NULL;

        data = alutLoadMemoryFromFile(name, &format, &size, &frequency);
        ap_audio_check_alut("alutLoadMemoryFromFile");
        LOGD("name %s, format %d, size %d, freq %f", name, format, size, frequency);

        ALuint buffer = 0;
        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("audio buffer");
                return 0;
        }
        alBufferData(buffer, format, data, size, frequency);
        ap_audio_check("audio data");
        free(data);

        return buffer;
}

unsigned int ap_audio_load_buffer_memory(
        const char* memory, int size, int format, float frequency)
{
        if (memory == NULL || size <= 0 || format <= 0) {
                LOGE("ap_audio_load_buffer_memory: invalid parameter");
                return 0;
        }
        ALuint buffer = 0;

        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("alGenBuffers");
                return 0;
        }

        if (frequency <= 0.0f) {
                frequency = 44100.0f;
        }

        alBufferData(buffer, format, memory, size, frequency);
        ap_audio_check("alBufferData");

        return buffer;
}

int ap_audio_play_buffer_sync(unsigned int buffer, int loop)
{
        if (device == NULL || context == NULL) {
                LOGE("failed to play audio: ap_audio is not initialized");
                return AP_ERROR_INIT_FAILED;
        }
        // clear error
        alGetError();
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

        alSourcei(source, AL_LOOPING, loop);
        ap_audio_check("alSourcei");

        alSourcei(source, AL_BUFFER, buffer);
        ap_audio_check("alSourcei");

        alSourcePlay(source);
        ap_audio_check("alSourcePlay");

        ALint source_state = 0;
        LOGD("playing %u", buffer);
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        while (source_state == AL_PLAYING) {
                alGetSourcei(source, AL_SOURCE_STATE, &source_state);
                ap_audio_check("alGetSourcei");
        }

        alDeleteSources(1, &source);

        return 0;
}

static void* ap_audio_play_buffer_func(void* data)
{
        unsigned int buffer = *((unsigned*) data);
        LOGD("ap_audio_play_buffer_func: id %u", buffer);
        ap_audio_play_buffer_sync(buffer, false);
        return NULL;
}

int ap_audio_play_buffer(unsigned int buffer)
{
        pthread_t pid = 0;
        static unsigned int tmp;
        tmp = buffer;
        pthread_create(&pid, NULL, ap_audio_play_buffer_func, &tmp);

        return 0;
}

int ap_audio_delete_buffer(unsigned int buffer)
{
        alDeleteBuffers(1, &buffer);
        return 0;
}

int ap_audio_struct_init(struct AP_Audio *audio)
{
        if (audio == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        memset(audio, 0, sizeof(struct AP_Audio));

        return 0;
}

int ap_audio_open_file_WAV(const char* filename, struct AP_Audio **out_audio_p)
{
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
        out_audio->channels = ap_audio_get_channels_by_AL(format);
        out_audio->data = data;
        out_audio->format = ap_audio_fmt_al_2_ap(format);
        out_audio->frequency = frequency;

        ALuint buffer = 0;
        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("alGenBuffers");
                return AP_AUDIO_BUFFER_GEN_FAILED;
        }
        alBufferData(buffer, format, data, size, frequency);
        ap_audio_check("alBufferData");
        out_audio->buffer_id = buffer;

        return 0;
}

int ap_audio_open_file_decode(
        const char* filename, struct AP_Audio **out_audio_p)
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
        int format = 0;
        struct AP_Vector *tmp_vec = NULL;
        ap_decode_to_memory(filename, &tmp_vec, &format, &frequency);
        if (!tmp_vec || tmp_vec->length == 0) {
                return AP_ERROR_DECODE_FAILED;
        }

        out_audio->name = AP_MALLOC((strlen(filename) + 1) * sizeof(char));
        strcpy(out_audio->name, filename);
        out_audio->channels = 2;
        out_audio->data = tmp_vec->data;
        out_audio->format = format;
        out_audio->data_size = tmp_vec->length;

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
        out_audio->buffer_id = buffer;
        AP_FREE(tmp_vec);

        return 0;
}

int ap_audio_finish()
{
        device = alcGetContextsDevice(context);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);

        alutExit();
        return 0;
}
