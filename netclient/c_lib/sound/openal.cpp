#include "openal.hpp"

#ifdef USE_OPENAL

#include <c_lib/sound/sound.hpp>
//#include <AL/al.h>
//#include <AL/alc.h>
#include <compat_al.h>

namespace OpenALSound
{

// lookup table
class Soundfile {
    public:
        unsigned int hash;
        int buffer;
        bool loaded;

    Soundfile():
        hash(0), buffer(-1), loaded(false)
        {}
};
static const int MAX_SOUNDS = Sound::MAX_WAV_BUFFERS;
static Soundfile soundfiles[MAX_SOUNDS];
static int soundfile_index = 0;

static bool enabled = true;
static bool inited = false;
static bool buffers_inited = false;
static bool sources_inited = false;
static ALCdevice *device = NULL;
static ALCcontext *context = NULL;

static const ALsizei MAX_BUFFERS = Sound::MAX_WAV_BUFFERS;
static ALuint buffers[MAX_BUFFERS];
static int buffer_index = 0;

static const ALsizei MAX_SOURCES = 16;
static ALuint sources[MAX_SOURCES];

static int active_2d_sounds[MAX_SOURCES] = {-1};

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

void set_volume(float vol)
{
    if (!enabled)
        return;
    alListenerf(AL_GAIN, vol);
    checkError();
}

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
{
    if (!enabled)
        return;
    alListener3f(AL_POSITION, x,z,y);
    alListener3f(AL_VELOCITY, vx,vz,vy);
    float o[6];
    o[0] = fx;
    o[1] = fz;
    o[2] = fy;
    o[3] = ux;
    o[4] = uz;
    o[5] = uy;
    for (int i=0; i<3; i++)
        o[i] *= -1; // reverse forward vector
    alListenerfv(AL_ORIENTATION, o);
}

void init()
{
    if (inited)
    {
        printf("WARNING: attempt to init OpenAl more than once\n");
        return;
    }

    inited = enabled = Options::sound;
    if (!enabled) return;
    
    Sound::init_wav_buffers();

    // open device (enumerate before this) 
    if (Options::sound_device[0] != '\0')
        device = alcOpenDevice(Options::sound_device);
    else
        device = alcOpenDevice(NULL); // select the "preferred device" 

    if (device == NULL)
    {
        printf("OpenAL error: sound device %s not found\n", Options::sound_device);
        close();
        inited = enabled = false;
        return;
    }

    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);  
    
    // Check for EAX 2.0 support 
    ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
    if (g_bEAX)
        printf("EAX is present\n");

    if (checkError())
    {
        close();
        inited = enabled = false;
        return;
    }

    // init AL buffers
    alGenBuffers(MAX_BUFFERS, buffers); 
    if (checkError())
    {
        close();
        inited = enabled = false;
        return;
    }
    buffers_inited = true;

    // init AL sources
    alGenSources(MAX_SOURCES, sources); 
    if (checkError())
    {
        close();
        inited = enabled = false;
        return;
    }
    sources_inited = true;
    
    // set source properties
    for (int i=0; i<MAX_SOURCES; i++)
    {
        alSourcef(sources[i], AL_PITCH, 1.0f);
        alSourcef(sources[i], AL_MIN_GAIN, 0.0f);
        alSourcef(sources[i], AL_MAX_GAIN, 1.0f);
        alSourcef(sources[i], AL_MAX_DISTANCE, 1000.0f);
        alSourcef(sources[i], AL_REFERENCE_DISTANCE, 5.0f);
    }

    // set distance model
    //alDistanceModel(AL_INVERSE_DISTANCE); // defaults to AL_INVERSE_DISTANCE_CLAMPED

    // init listener state
    set_volume(Options::sfx);
    update_listener(0,0,0, 0,0,0, 0,-1,0, 0,0,1);
    if (checkError())
    {
        close();
        inited = enabled = false;
        return;
    }

    enabled = true;
    inited = true;
    printf("OpenAL inited\n");
}

void close()
{
    if (sources_inited)
    {
        alDeleteSources(MAX_SOURCES, sources);
        checkError();
    }

    if (buffers_inited)
    {
        alDeleteBuffers(MAX_BUFFERS, buffers);
        buffer_index = 0;
        checkError();
    }

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
    enabled = false;
    if (inited)
        printf("OpenAL sound closed.\n");
}

unsigned int hash(char* s)
{
    unsigned int highorder;
    unsigned int h = 0;
    int i;
    for (i=0; s[i] != '\0'; i++) {
         highorder = h & 0xf8000000;    // extract high-order 5 bits from h
                                        // 0xf8000000 is the hexadecimal representation
                                        //   for the 32-bit number with the first five 
                                        //   bits = 1 and the other bits = 0   
         h = h << 5;                    // shift h left by 5 bits
         h = h ^ (highorder >> 27);     // move the highorder 5 bits to the low-order
                                        //   end and XOR into h
         h = h ^ (unsigned int)s[i];                  // XOR h and ki
    }
    return h;
}

void load_sound(char* fn)
{
    if (!enabled)
        return;
        
    if (buffer_index == MAX_BUFFERS)
    {
        printf("ERROR OpenALSound::load_sound -- no AL buffers available\n");
        return;
    }
    unsigned char* buffer = NULL;

    // Load test.wav 
    int data_id = Sound::load_wav_file(fn, &buffer);
    if (data_id < 0)
    {
        printf("OpenALSound::load_sound -- wav data_id %d invalid\n", data_id);
        return;
    }
    if (buffer == NULL)
    {
        printf("OpenALSound::load_sound -- buffer data is NULL\n");
        return;
    }

    Sound::WavData *data = Sound::get_loaded_wav_data(data_id);
    if (data == NULL)
    {
        printf("OpenALSound::load_sound -- wav metadata is NULL\n");
        return;
    }
    //Sound::print_wav_data(data);

    // retrieve OpenAL specific format, determined from wav metadata
    ALenum fmt = Sound::get_openal_wav_format(data);
    if (fmt == AL_FORMAT_STEREO8 || fmt == AL_FORMAT_STEREO16)  // stereo samples wont be attenuated in OpenAL
        printf("WARNING: audio file %s is in stereo format. 3D sound will not be applied for this sample.\n", fn);
        
    // put the PCM data into the alBuffer
    // (this will copy the buffer, so we must free our PCM buffer)
    alBufferData(buffers[buffer_index], fmt, buffer, data->size, data->sample_rate);
    free(buffer);
    if (checkError())
        return;

    // put in lookup table for playback
    Soundfile* s;
    s = &soundfiles[soundfile_index++];
    s->hash = hash(fn);
    s->buffer = buffer_index;
    s->loaded = true;

    buffer_index++;
}

int get_free_source()
{
    ALint source_state;
    for (int i=0; i<MAX_SOURCES; i++)
    {
        alGetSourcei(sources[i], AL_SOURCE_STATE, &source_state);
        if (source_state != AL_PLAYING)
            return i;
    }
    return -1;
}

int get_buffer_from_filename(char *fn)
{
    unsigned int h = hash(fn);
    for (int i=0; i<MAX_SOUNDS; i++)
        if (soundfiles[i].loaded && soundfiles[i].hash == h)
            return soundfiles[i].buffer;

    return -1;
}


int update_source_state(int id, float x, float y, float z, float vx, float vy, float vz, float ox, float oy, float oz)
{
    alSource3f(id, AL_POSITION, x, z, y);
    alSource3f(id, AL_VELOCITY, vx, vz, vy);
    alSource3f(id, AL_DIRECTION, ox, oz, oy);
    if (checkError())
        return -1;
    return 0;
}

void get_listener_state(float *x, float *y, float *z, float *vx, float *vy, float *vz, float orientation[6])
{
    alGetListener3f(AL_POSITION, x, y, z);
    alGetListener3f(AL_VELOCITY, vx, vy, vz);
    alGetListenerfv(AL_ORIENTATION, orientation);
}

int play_2d_sound(char* file)
{
    if (!enabled)
        return -1;
        
    // get listener state
    ALfloat x,y,z;
    ALfloat vx,vy,vz;
    ALfloat o[6];
    get_listener_state(&x, &y, &z, &vx, &vy, &vz, o);
    // play at listener state
    int source_id = play_3d_sound(file, x,z,y,vx,vz,vy, o[0], o[2], o[1]);

    // add sound to active sources
    for (int i=0; i<MAX_SOURCES; i++)
        if (active_2d_sounds[i] < 0)
            active_2d_sounds[i] = source_id;
    return source_id;
}

int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz, float ox, float oy, float oz)
{
    if (!enabled)
        return -1;

    // get free source
    int source_id = get_free_source();
    if (source_id < 0)
        return -1;
        
    // lookup buffer from file
    int buffer_id = get_buffer_from_filename(file);
    if (buffer_id < 0)
        return -1;

    // set source state
    if (update_source_state(sources[source_id], x,y,z, vx,vy,vz, ox,oy,oz))
        return -1;

    // Attach buffer 0 to source
    alSourcei(sources[source_id], AL_BUFFER, buffers[buffer_id]); 
    if (checkError())
        return -1;

    // play
    alSourcePlay(sources[source_id]);
    if (checkError())
        return -1;

    return source_id;
}

void update()
{
    // get listener state
    ALfloat x,y,z;
    ALfloat vx,vy,vz;
    ALfloat o[6];
    get_listener_state(&x, &y, &z, &vx, &vy, &vz, o);

    // update playing sources to listener state
    ALint source_state;
    for (int i=0; i<MAX_SOURCES; i++)
    {
        int source_id = active_2d_sounds[i];
        if (source_id < 0) continue;
        alGetSourcei(sources[source_id], AL_SOURCE_STATE, &source_state);
        if (source_state != AL_PLAYING)
            active_2d_sounds[i] = -1;
        else    // update 2d sound
            update_source_state(sources[source_id], x,z,y,vx,vz,vy, o[0], o[2], o[1]);
    }
}

int test()
{
    const ALsizei MAX_BUFFERS = 1;
    ALuint buffers[MAX_BUFFERS];

    const ALsizei MAX_SOURCES = 1;
    ALuint sources[MAX_SOURCES];

    alGenBuffers(MAX_BUFFERS, buffers); 
    if (checkError())
    {   printf("alGenBuffers:\n");
        return 1;
    }

    // Generate Sources 
    alGenSources(MAX_SOURCES, sources); 
    if (checkError())
    {   printf("alGenSources:\n");
        alDeleteBuffers(MAX_BUFFERS, buffers); 
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
        printf("OpenALSound::test -- buffer data is NULL\n");
        return 1;
    }

    Sound::WavData *data = Sound::get_loaded_wav_data(data_id);
    if (data == NULL)
    {
        printf("OpenALSound::test -- wav metadata is NULL\n");
        return 1;
    }
    //Sound::print_wav_data(data);

    // retrieve OpenAL specific format, determined from wav metadata
    ALenum fmt = Sound::get_openal_wav_format(data);

    // put the PCM data into the alBuffer
    // (this will copy the buffer, so we must free our char buffer)
    alBufferData(buffers[0], fmt, buffer, data->size, data->sample_rate);
    if (checkError())
    {   printf("alBufferData:\n");
        alDeleteBuffers(MAX_BUFFERS, buffers); 
        free(buffer);
        return 1;
    }

    free(buffer);
 
    // Attach buffer 0 to source 
    alSourcei(sources[0], AL_BUFFER, buffers[0]); 
    if (checkError())
    {   printf("alSourcei: BUFFER\n");
        alDeleteBuffers(MAX_BUFFERS, buffers); 
        return 1;
    }

    //play
    alSourcePlay(sources[0]);
    if (checkError())
    {   printf("alSourcePlay:\n");
        alDeleteBuffers(MAX_BUFFERS, buffers); 
        free(buffer);
        return 1;
    }

    return 0;
}

void enumerate_devices()
{
    if (alcIsExtensionPresent(NULL, (ALchar*)"ALC_ENUMERATION_EXT") != AL_TRUE)
    {
        printf("OpenAL device enumeration extension is not available.\n");
        return;
    }
    printf("\n");
    printf("OpenAL Device enumeration:\n\n");

    const ALchar* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    const ALchar* default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    printf("\n");
    printf("Devices available:\n");
    ALchar c = '\0';
    int i = 0, j = 0;
    ALchar *device_name = (ALchar*)calloc(200, sizeof(ALchar));
    while (1)
    {
        if (c == '\0')
        {
            j = 0;
            printf("%s", device_name);
            memset(device_name, '\0', 200 * sizeof(ALchar));
            printf("\n");
            if (devices[i] == '\0')
                break;
        }

        c = devices[i++];
        device_name[j++] = c;
    }
    free(device_name);
    
    printf("\n");
    printf("Default device:\n");
    printf("%s", default_device);
    printf("\n");

}

}

#endif
