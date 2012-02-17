#include "openal.hpp"

#if USE_OPENAL

#include <c_lib/sound/sound.hpp>
#include <AL/alut.h>

namespace OpenALSound
{

static bool enabled = false;
static ALCdevice *device = NULL;
static ALCcontext *context = NULL;

bool checkError()
{
    ALenum a = alGetError();

    switch (a)
    {
        case AL_NO_ERROR:
            return false;
        case AL_INVALID_NAME:
            printf("OpenAL error: a bad name (ID) was passed to an OpenAL function\n");
            return true;
        case AL_INVALID_ENUM:
            printf("OpenAL error: an invalid enum value was passed to an OpenAL function\n");
            return true;
        case AL_INVALID_VALUE:
            printf("OpenAL error: an invalid value was passed to an OpenAL function\n");
            return true;
        case AL_INVALID_OPERATION:
            printf("OpenAL error: the requested operation is not valid\n");
            return true;
        case AL_OUT_OF_MEMORY:
            printf("OpenAL error: the requested operation resulted in OpenAL running out of memory\n");
            return true;
        default:
            printf("OpenAL error: unknown error\n");
            return true;
    }
    return true;
}

void init()
{
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: attempt to init OpenAl more than once\n");
        return;
    }

    Sound::init_wav_buffers();

    // Initialization 
    device = alcOpenDevice(NULL); // select the "preferred device" 

    if (device == NULL)
    {
        enabled = false;
        return;
    }

    context = alcCreateContext(device, NULL); 
    alcMakeContextCurrent(context);  
    
    // Check for EAX 2.0 support 
    ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
    if (g_bEAX)
        printf("EAX is present\n");

    if (checkError())
    { // clear error code
        enabled = false;
        return;
    }

    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    float orientation[6] = {0, -1, 0, 0, 0, 1};
    alListenerfv(AL_ORIENTATION, orientation);
    if (checkError())
    {
        enabled = false;
        return;
    }

    enabled = true;
    printf("OpenAL inited\n");
}

void close()
{
    alcMakeContextCurrent(NULL); 
    if (context != NULL)
    {
        alcDestroyContext(context);
        context = NULL;
    }
    if (device != NULL)
    {
        alcCloseDevice(device);
        device = NULL;
    }

    Sound::teardown_wav_buffers();
    
    printf("OpenAL sound closed.\n");
}

void set_volume(float vol)
{

}

void load_sound(char* file)
{

}

int play_2d_sound(char* file)
{
    return 0;

}

int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz)
{
    return 0;

}

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
{

}

void update()
{

}

int test()
{
    //init();

    const ALsizei NUM_BUFFERS = 1;
    ALuint buffers[NUM_BUFFERS];

    const ALsizei NUM_SOURCES = 1;
    ALuint sources[NUM_SOURCES];

    alGenBuffers(NUM_BUFFERS, buffers); 
    if (checkError())
    {   printf("alGenBuffers:\n");
        return 1;
    }

    // Generate Sources 
    alGenSources(NUM_SOURCES, sources); 
    if (checkError())
    {   printf("alGenSources:\n");
        alDeleteBuffers(NUM_BUFFERS, buffers); 
        return 1;
    }

    alSourcef(sources[0], AL_PITCH, 1);
    alSourcef(sources[0], AL_GAIN, 1);
    alSource3f(sources[0], AL_POSITION, 0, 0, 0);
    alSource3f(sources[0], AL_VELOCITY, 0, 0, 0);
    alSourcei(sources[0], AL_LOOPING, AL_TRUE);

    checkError();

    unsigned char* buffer = NULL;

    char fn[] = "./media/sound/wav/plasma_grenade_explode.wav";
    // Load test.wav 
    int data_id = Sound::load_wav_file(fn, &buffer);
    if (data_id < 0)
    {
        printf("OpenALSound::test -- wav data_id %d invalid\n", data_id);
        return 1;
    }
    if (buffer == NULL)
    {
        printf("OpenALSound::test -- buffer is NULL\n");
        return 1;
    }

    Sound::WavData *data = Sound::get_loaded_wav_data(data_id);
    if (data == NULL)
    {
        printf("OpenALSound::test -- wav data is NULL\n");
        return 1;
    }
    
    ALenum fmt;
    fmt = (data->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    //put the data into our sampleset buffer
    //alBufferData(buffers[0], data->format, buffer, data->size, data->sample_rate);
    //alBufferData(buffers[0], getFormatInfoFrom(data->channels, data->bits_per_sample), buffer, data->size, data->sample_rate);
    alBufferData(buffers[0], fmt, buffer, data->size, data->sample_rate);
    if (checkError())
    {   printf("alBufferData:\n");
        alDeleteBuffers(NUM_BUFFERS, buffers); 
        free(buffer);
        return 1;
    }
 
    // Attach buffer 0 to source 
    alSourcei(sources[0], AL_BUFFER, buffers[0]); 
    if (checkError())
    {   printf("alSourcei: BUFFER\n");
        alDeleteBuffers(NUM_BUFFERS, buffers); 
        free(buffer);
        return 1;
    }

    free(buffer);

    //alSourcei(sources[0],AL_LOOPING,AL_TRUE);
    //if (checkError())
    //{   printf("alSourcei: LOOPING\n");
        //alDeleteBuffers(NUM_BUFFERS, buffers); 
        //free(buffer);
        //return 1;
    //}
    
    //play
    alSourcePlay(sources[0]);
    if (checkError())
    {   printf("alSourcePlay:\n");
        alDeleteBuffers(NUM_BUFFERS, buffers); 
        free(buffer);
        return 1;
    }

    //close();
    return 0;
}


}

#endif
