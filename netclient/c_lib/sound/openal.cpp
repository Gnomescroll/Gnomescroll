#include "openal.hpp"

#include <c_lib/sound/sound.hpp>
//#include <AL/al.h>
//#include <AL/alc.h>
#include <compat_al.h>

namespace OpenALSound
{

const int MAX_SOURCES_PER_SAMPLE = 6;

// lookup table
class GS_SoundBuffer
{
    public:
        int id;
        unsigned int hash;
        int buffer_id;
        bool loaded;
        Soundfile* metadata;
        const int max_sources;
        int current_sources;
        int* sources;

    void add_source(int source_id)
    {
        if (this->current_sources >= this->max_sources)
        {
            printf("ERROR: -- GS_SoundBuffer::add_source -- attempt to add source %d to GS_SoundBuffer %d, but it is full.\n", source_id, this->id);
            return;
        }
        this->sources[this->current_sources++] = source_id;
    }

    GS_SoundBuffer():
        id(-1),
        hash(0), buffer_id(-1), loaded(false), metadata(NULL),
        max_sources(MAX_SOURCES_PER_SAMPLE),
        current_sources(0)
        {
            this->sources = (int*)malloc(sizeof(int) * this->max_sources);
        }

    ~GS_SoundBuffer()
    {
        free(this->sources);
    }
};
static const int MAX_SOUNDS = Sound::MAX_WAV_BUFFERS;
static GS_SoundBuffer sound_buffers[MAX_SOUNDS];
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

struct GS_SoundSource {
    int source_id;
    bool two_dimensional;
};
static GS_SoundSource active_sources[MAX_SOURCES] = {{-1, false}};

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

    // DONT alGetError() before call to alcMakeContextCurrent -- will return error!
    if (checkError())
    {
        close();
        inited = enabled = false;
        device = NULL;
        return;
    }

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
    set_volume(((float)Options::sfx) / 100.0f);
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

static unsigned int hash(char* s)
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

void load_sound(Soundfile* snd)
{
    if (!enabled)
        return;

    GS_SoundBuffer* s;
    // check if file has been loaded
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        s = &sound_buffers[i];
        if (s->metadata != NULL && strcmp(s->metadata->file, snd->file) == 0)
        {   // already loaded this sound file
            GS_SoundBuffer* new_s = &sound_buffers[soundfile_index];
            new_s->id = soundfile_index;
            new_s->hash = hash(snd->fn);
            new_s->buffer_id = s->buffer_id;
            new_s->loaded = true;
            new_s->metadata = snd;
            soundfile_index++;
            return;
        }
    }
        
    if (buffer_index == MAX_BUFFERS)
    {
        printf("ERROR OpenALSound::load_sound -- no AL buffers available\n");
        return;
    }
    unsigned char* buffer = NULL;

    // Load test.wav 
    int data_id = Sound::load_wav_file(snd->file, &buffer);
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
        printf("WARNING: audio file %s is in stereo format. 3D sound will not be applied for this sample.\n", snd->file);
        
    // put the PCM data into the alBuffer
    // (this will copy the buffer, so we must free our PCM buffer)
    alBufferData(buffers[buffer_index], fmt, buffer, data->size, data->sample_rate);
    free(buffer);
    if (checkError())
        return;

    // put in lookup table for playback
    s = &sound_buffers[soundfile_index];
    s->id = soundfile_index;
    s->hash = hash(snd->fn);
    s->buffer_id = buffer_index;
    s->loaded = true;
    s->metadata = snd;
    buffer_index++;
    soundfile_index++;
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

GS_SoundBuffer* get_sound_buffer_from_function_name(char *fn)
{
    unsigned int h = hash(fn);
    for (int i=0; i<MAX_SOUNDS; i++)
        if (sound_buffers[i].loaded && sound_buffers[i].hash == h)
            return &sound_buffers[i];
    return NULL;
}

int set_source_properties(int source_id, Soundfile* snd)
{
    if (source_id < 0 || source_id >= MAX_SOURCES)
        printf("WARNING -- set_source_properties -- source_id %d out of range\n", source_id);
    alSourcef(sources[source_id], AL_PITCH, snd->pitch);
    alSourcef(sources[source_id], AL_GAIN, snd->gain);
    alSourcef(sources[source_id], AL_MAX_DISTANCE, snd->max_distance);
    alSourcef(sources[source_id], AL_REFERENCE_DISTANCE, snd->reference_distance);
    alSourcef(sources[source_id], AL_MIN_GAIN, snd->minimum_gain);
    alSourcef(sources[source_id], AL_MAX_GAIN, snd->maximum_gain);
    return (checkError());
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

static bool add_to_sources(int soundfile_id, int source_id, bool two_dimensional)
{
    // add sound to active sources
    for (int i=0; i<MAX_SOURCES; i++)
        if (active_sources[i].source_id < 0)
        {
            active_sources[i].source_id = source_id;
            active_sources[i].two_dimensional = two_dimensional;
            sound_buffers[soundfile_id].add_source(source_id);
            return true;
        }
    return false;
}

static int play_sound(GS_SoundBuffer* sound_buffer, float x, float y, float z, float vx, float vy, float vz, float ox, float oy, float oz)
{
    if (!enabled)
        return -1;

    // get free source
    int source_id = get_free_source();
    //printf("source_id = %d\n", source_id);
    if (source_id < 0)
        return -1;

    if (source_id >= MAX_SOURCES || source_id < 0)
    printf("/*********/\nWARNING: source_id %d out of bounds\n/***********/\n", source_id);
        
    // set source properties
    if (set_source_properties(source_id, sound_buffer->metadata))
        return -1;

    // set source state
    if (update_source_state(sources[source_id], x,y,z, vx,vy,vz, ox,oy,oz))
        return -1;

    // Attach buffer 0 to source
    alSourcei(sources[source_id], AL_BUFFER, buffers[sound_buffer->buffer_id]); 
    if (checkError())
        return -1;

    // play
    alSourcePlay(sources[source_id]);
    if (checkError())
        return -1;
    
    return source_id;
}

int play_2d_sound(char* fn)
{
    if (!enabled)
        return -1;

    // lookup buffer from file
    GS_SoundBuffer* sound_buffer = get_sound_buffer_from_function_name(fn);
    //printf("function name %s\n", fn);

    if (sound_buffer == NULL)
        return -1;
    if (sound_buffer->buffer_id < 0)
        return -1;

    if (sound_buffer->current_sources >= sound_buffer->max_sources)
        return -1;
        
    // get listener state
    ALfloat x,y,z;
    ALfloat vx,vy,vz;
    ALfloat o[6];
    get_listener_state(&x, &y, &z, &vx, &vy, &vz, o);
    // play at listener state
    int source_id = play_sound(sound_buffer, x,z,y,vx,vz,vy, o[0], o[2], o[1]);
    if (source_id < 0) return source_id;
    
    // add sound to active sources
    if (!add_to_sources(sound_buffer->buffer_id, source_id, true))
        return -1;

    return source_id;
}

int play_3d_sound(char* fn, float x, float y, float z, float vx, float vy, float vz, float ox, float oy, float oz)
{
    if (!enabled)
        return -1;

    // lookup buffer from file
    GS_SoundBuffer* sound_buffer = get_sound_buffer_from_function_name(fn);
    //printf("function name %s\n", fn);

    if (sound_buffer == NULL)
        return -1;
    if (sound_buffer->buffer_id < 0)
        return -1;

    if (sound_buffer->current_sources >= sound_buffer->max_sources)
        return -1;
        
    int source_id = play_sound(sound_buffer, x,y,z, vx,vy,vz, ox,oy,oz);
    if (source_id < 0) return source_id;
    // add sound to active sources
    if (!add_to_sources(sound_buffer->buffer_id, source_id, false))
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

    ALint source_state;

    // expire any used sources
    for (int i=0; i<MAX_SOURCES; i++)
    {
        GS_SoundSource* ss = &active_sources[i];
        if (ss->source_id < 0) continue;
        
        alGetSourcei(sources[ss->source_id], AL_SOURCE_STATE, &source_state);
        if (source_state != AL_PLAYING)
            ss->source_id = -1;
        else    // update 2d sound
        {
            if (ss->two_dimensional)    // update 2d listeners
                update_source_state(sources[ss->source_id], x,z,y,vx,vz,vy, o[0], o[2], o[1]);
        }
    }

    // update sound_buffer metadata
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        GS_SoundBuffer* b = &sound_buffers[i];
        const int current_sources = b->current_sources;
        for (int j=0; j<current_sources; j++)
        {
            int gs_source_id = b->sources[j];
            if (active_sources[gs_source_id].source_id < 0)
            {   // expired
                b->current_sources--;
                b->sources[b->current_sources] = -1;
            }
        }
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
