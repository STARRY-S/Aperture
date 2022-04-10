#include "ap_audio.h"
#include "ap_utils.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include <pthread.h>

ALCdevice *device = NULL;
ALCcontext *context = NULL;
const ALCchar *device_name = NULL;

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

static inline ALenum ap_audio_to_al_format(short channels, short samples)
{
	bool stereo = (channels > 1);

	if (samples == 16) {
		if (stereo)
			return AL_FORMAT_STEREO16;
		else
			return AL_FORMAT_MONO16;
        } else if (samples == 8) {
		if (stereo)
			return AL_FORMAT_STEREO8;
		else
			return AL_FORMAT_MONO8;
	}

        return -1;
}

static inline int ap_audo_list_device(const ALCchar *devices)
{
	const ALCchar *device = devices, *next = devices + 1;
	size_t len = 0;

	fprintf(stdout, "Devices list:\n");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		fprintf(stdout, "%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	fprintf(stdout, "----------\n");

        return 0;
}

int ap_audio_init()
{
        int res = 0;
        res = alutInitWithoutContext(NULL, NULL);
        if (res == 0) {
                ap_audio_check_alut("init alut");
                return AP_ERROR_INIT_FAILED;
        }

        if (device_name == NULL) {
                device_name = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        }


        if (device == NULL) {
                device = alcOpenDevice(device_name);
                if (!device) {
                        LOGE("failed to open device name %s", device_name);
                        return AP_ERROR_INIT_FAILED;
                }
        }
	LOGI("audio device: %s", alcGetString(device, ALC_DEVICE_SPECIFIER));
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

        return 0;
}

unsigned int ap_audio_load_buffer(const char* name)
{
        ALsizei size = 0;
        ALfloat frequency = 0.0f;
        ALenum format;
        ALboolean loop = AL_FALSE;
        const ALvoid* data = NULL;

        data = alutLoadMemoryFromFile(name, &format, &size, &frequency);
        ap_audio_check_alut("alutLoadMemoryFromFile");
        LOGI("name %s, format %d, size %d, freq %f", name, format, size, frequency);

        ALuint buffer = 0;
        alGenBuffers((ALuint)1, &buffer);
        if (buffer == 0) {
                ap_audio_check("audio buffer");
                return 0;
        }
        alBufferData(buffer, format, data, size, frequency);
        ap_audio_check("audio data");

        return buffer;
}

int ap_audio_play_buffer_sync(unsigned int buffer)
{
        // generate a source
        ALuint source;
        alGenSources((ALuint)1, &source);

        alSourcef(source, AL_PITCH, 1);
        // check for errors
        alSourcef(source, AL_GAIN, 1);
        // check for errors
        alSource3f(source, AL_POSITION, 0, 0, 0);
        // check for errors
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        // check for errors
        alSourcei(source, AL_LOOPING, AL_TRUE);
        // check for errros

        // bind a buffer to source
        alSourcei(source, AL_BUFFER, buffer);

        alSourcePlay(source);
        // check for errors

        ALint source_state = 0;
        LOGI("playing %u", buffer);
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        // check for errors
        while (source_state == AL_PLAYING) {
                alGetSourcei(source, AL_SOURCE_STATE, &source_state);
                // check for errors
        }

        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        device = alcGetContextsDevice(context);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);

        return 0;
}

void* ap_audio_play_buffer_func(void* data)
{
        LOGI("ap_audio_play_buffer_func thread func");
        unsigned int buffer = *((unsigned*) data);
        LOGI("%u", buffer);
        ap_audio_play_buffer_sync(buffer);
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

int ap_audio_finish()
{
        alutExit();
        return 0;
}
