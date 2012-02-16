#include "openal.hpp"

#if USE_OPENAL

#include <c_lib/sound/sound.hpp>
#include <AL/alut.h>

namespace OpenALSound
{

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

    // Initialization 
    ALCdevice* Device = alcOpenDevice(NULL); // select the "preferred device" 

    ALCcontext* Context;
    if (Device)
    {
        Context=alcCreateContext(Device,NULL); 
        alcMakeContextCurrent(Context);  
    }
    
    // Check for EAX 2.0 support 
    ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
    if (g_bEAX)
        printf("EAX is preent\n");

    // Generate Buffers 
    alGetError(); // clear error code

}

void close()
{

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
    return 0;
}


}

#endif
