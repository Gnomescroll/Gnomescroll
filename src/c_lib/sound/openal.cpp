#include "openal.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <sound/sound.hpp>
#include <common/compat_al.h>
#include <physics/vec3.hpp>

namespace OpenALSound
{

const int MAX_SOURCES_PER_SAMPLE = 6;
static const int MAX_SOUNDS = Sound::MAX_WAV_BUFFERS;
static const ALsizei MAX_BUFFERS = Sound::MAX_WAV_BUFFERS;
static const ALsizei MAX_SOURCES = 16;

// lookup table
class GS_SoundBuffer
{
    public:
        int id;
        unsigned int hash;
        char* fn;
        int buffer_id;
        bool loaded;
        Soundfile* metadata;
        const int max_sources;
        int current_sources;
        int* sources;

    bool add_source(int source_id)
    {
        if (!this->can_add_source(source_id)) return false;
        this->sources[this->current_sources++] = source_id;
        return true;
    }
    
    bool can_add_source(int source_id)
    {
        GS_ASSERT(source_id >= 0 && source_id < MAX_SOURCES);
        if (source_id < 0 || source_id >= MAX_SOURCES) printf("can't add %d because invalid id\n", source_id);
        if (source_id < 0 || source_id >= MAX_SOURCES) return false;
        for (int i=0; i<this->current_sources; i++)
        {   // TODO -- this used to return false. not sure why but it was breaking things (after i rearranged the source_id acquisition to prevent sound leaks)
            // the naming in this sound module is horrible (buffers and sources mean 2 things -- gnomescroll buffers (more like files+metadata) and gnomescroll sources (something keeping tracks of al source ids in use)
            if (this->sources[i] == source_id) printf("can't add %d because this source is already added\n", source_id);
            if (this->sources[i] == source_id) return false;
            if (this->sources[i] == source_id) return true;
        }
        GS_ASSERT(this->current_sources >= 0);
        if (this->current_sources < 0) this->current_sources = 0;
        if (this->current_sources >= this->max_sources) printf("Can't add %d because max sources reached for this buffer\n", source_id);
        if (this->current_sources >= this->max_sources) return false;
        return true;
    }

    void remove_source(int index)
    {
        GS_ASSERT(index >= 0 && index < this->max_sources);
        if (index < 0 || index >= this->max_sources) return;

        GS_ASSERT(this->current_sources > 0);
        if (this->current_sources <= 0)
        {
            this->current_sources = 0;
            return;
        }

        GS_ASSERT(index < this->current_sources); // TODO -- re-enable this assert and FIX IT
        if (index >= this->current_sources) return;

        if (index == this->current_sources-1)
            this->current_sources--;
        else
            this->sources[index] = this->sources[--this->current_sources];
    }

    GS_SoundBuffer() :
    id(-1),
    hash(0), fn(NULL), buffer_id(-1), loaded(false), metadata(NULL),
    max_sources(MAX_SOURCES_PER_SAMPLE),
    current_sources(0)
    {
        this->sources = (int*)malloc(sizeof(int) * this->max_sources);
        for (int i=0; i<this->max_sources; this->sources[i++] = -1);
    }

    ~GS_SoundBuffer()
    {
        free(this->sources);
        free(this->fn);
    }
};

static GS_SoundBuffer** sound_buffers = NULL;
static int soundfile_index = 0;

static bool enabled = true;
static bool inited = false;
static bool buffers_inited = false;
static bool sources_inited = false;
static ALCdevice *device = NULL;
static ALCcontext *context = NULL;

static ALuint* buffers = NULL;
static int buffer_index = 0;

static ALuint* sources = NULL;

class GS_SoundSource
{
    public:
        bool active;
        bool two_dimensional;
        // add AL source properties here so we can vary per source easily

    GS_SoundSource() :
    active(false), two_dimensional(false)
    {}
};

static class GS_SoundSource* active_sources = NULL;
static int sources_in_use = 0;

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
    if (!enabled) return;
    alListenerf(AL_GAIN, vol);
    checkError();
}

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
{
    if (!enabled) return;
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
    GS_ASSERT(!inited);

    enabled = Options::sound;
    if (!enabled) return;

    // allocate memory
    sources = (ALuint*)malloc(sizeof(ALuint) * MAX_SOURCES);
    memset(sources, -1, sizeof(ALuint));
    buffers = (ALuint*)malloc(sizeof(ALuint) * MAX_BUFFERS);
    memset(buffers, -1, sizeof(ALuint));
    sound_buffers = (GS_SoundBuffer**)calloc(MAX_SOUNDS, sizeof(GS_SoundBuffer*));
    
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
        enabled = false;
        return;
    }

    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);  

    // DONT alGetError() before call to alcMakeContextCurrent -- will return error!
    if (checkError())
    {
        close();
        enabled = false;
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
        enabled = false;
        return;
    }

    // init AL buffers
    alGenBuffers(MAX_BUFFERS, buffers); 
    if (checkError())
    {
        close();
        enabled = false;
        return;
    }
    buffers_inited = true;

    // init AL sources
    alGenSources(MAX_SOURCES, sources); 
    if (checkError())
    {
        close();
        enabled = false;
        return;
    }
    sources_inited = true;

    // set source properties
    for (int i=0; i<MAX_SOURCES; i++)
    {
        alSourcef(sources[i], AL_PITCH, 1.0f);
        alSourcef(sources[i], AL_MIN_GAIN, 0.0f);
        alSourcef(sources[i], AL_MAX_GAIN, 1.0f);
        alSourcef(sources[i], AL_MAX_DISTANCE, 256.0f);
        alSourcef(sources[i], AL_REFERENCE_DISTANCE, 1.0f);
        alSourcef(sources[i], AL_ROLLOFF_FACTOR, 1.0f);
    }

    // set distance model
    //alDistanceModel(AL_INVERSE_DISTANCE); // defaults to AL_INVERSE_DISTANCE_CLAMPED

    // init listener state
    set_volume(((float)Options::sfx) / 100.0f);
    update_listener(0,0,0, 0,0,0, 0,-1,0, 0,0,1);
    if (checkError())
    {
        close();
        enabled = false;
        return;
    }
    
    // init active sources buffer
    active_sources = new GS_SoundSource[MAX_SOURCES];
    
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

    // deallocate memory
    if (sources != NULL)
    {
        free(sources);
        sources = NULL;
    }
    if (buffers != NULL)
    {
        free(buffers);
        buffers = NULL;
    }
    if (sound_buffers != NULL)
    {
        for (int i=0; i<MAX_SOUNDS; i++)
            if (sound_buffers[i] != NULL)
                delete sound_buffers[i];
        free(sound_buffers);
        sound_buffers = NULL;
    }
    if (active_sources != NULL)
    {
        delete[] active_sources;
        active_sources = NULL;
    }

    Sound::teardown_wav_buffers();
    enabled = false;
    if (inited) printf("OpenAL sound closed.\n");
}

void load_sound(Soundfile* snd)
{
    if (!enabled)
        return;

    GS_SoundBuffer* s;
    // check if file has been loaded
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        s = sound_buffers[i];
        if (s == NULL) continue;
        if (s->metadata != NULL && strcmp(s->metadata->file, snd->file) == 0)
        {   // already loaded this wav file into an openal buffer.
            // create a new instance of GS_Soundbuffer and copy the OpenAL buffer id
            GS_SoundBuffer* new_s = new GS_SoundBuffer;
            new_s->id = soundfile_index;
            new_s->hash = snd->hash;
            new_s->fn = (char*)malloc((strlen(snd->fn)+1)*sizeof(char));
            strcpy(new_s->fn, snd->fn);
            new_s->buffer_id = s->buffer_id;
            new_s->loaded = true;
            new_s->metadata = snd;
            sound_buffers[soundfile_index] = new_s;
            soundfile_index++;
            return;
        }
    }

    // we havent loaded this file into an OpenAL buffer yet. do it now, then create a new GS_SoundBuffer 
        
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
    s = new GS_SoundBuffer;
    s->id = soundfile_index;
    s->hash = snd->hash;
    s->fn = (char*)malloc((strlen(snd->fn)+1) * sizeof(char));
    strcpy(s->fn, snd->fn);
    s->buffer_id = buffer_index;
    s->loaded = true;
    s->metadata = snd;
    sound_buffers[soundfile_index] = s;
    buffer_index++;
    soundfile_index++;
}

int get_free_source()
{
    for (int i=0; i<MAX_SOURCES; i++)
        if (!active_sources[i].active)
            return i;
    return -1;
}

GS_SoundBuffer* get_sound_buffer_from_function_name(const char* fn)
{
    unsigned int h = strhash(fn);
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        if (sound_buffers[i] == NULL) continue;
        if (sound_buffers[i]->loaded && sound_buffers[i]->hash == h)
            return sound_buffers[i];
    }
    return NULL;
}

GS_SoundBuffer* get_sound_buffer_from_soundfile_id(int soundfile_id)
{
    GS_ASSERT(soundfile_id >= 0 && soundfile_id < MAX_SOUNDS);
    if (soundfile_id < 0 || soundfile_id >= MAX_SOUNDS) return NULL;
    if (sound_buffers[soundfile_id] == NULL) return NULL;
    if (!sound_buffers[soundfile_id]->loaded) return NULL;
    return sound_buffers[soundfile_id];
}

int set_source_properties(int source_id, Soundfile* snd)
{
    if (source_id < 0 || source_id >= MAX_SOURCES)
    {
        printf("WARNING -- set_source_properties -- source_id %d out of range\n", source_id);
        return 1;
    }
    if (snd == NULL)
    {
        printf("WARNING -- set_source_properties -- snd is NULL\n");
        return 1;
    }
    alSourcef(sources[source_id], AL_PITCH, snd->pitch);
    alSourcef(sources[source_id], AL_GAIN, snd->gain);
    alSourcef(sources[source_id], AL_MAX_DISTANCE, snd->max_distance);
    alSourcef(sources[source_id], AL_REFERENCE_DISTANCE, snd->reference_distance);
    alSourcef(sources[source_id], AL_MIN_GAIN, snd->minimum_gain);
    alSourcef(sources[source_id], AL_MAX_GAIN, snd->maximum_gain);
    alSourcef(sources[source_id], AL_ROLLOFF_FACTOR, snd->rolloff_factor);
    ALboolean loop = (snd->loop) ? AL_TRUE : AL_FALSE;
    alSourcei(sources[source_id], AL_LOOPING, loop);
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
    GS_ASSERT(soundfile_id >= 0 && soundfile_id < MAX_SOUNDS);
    if (soundfile_id < 0 || soundfile_id >= MAX_SOUNDS) return false;

    GS_ASSERT(source_id >= 0 && source_id < MAX_SOURCES);
    if (source_id < 0 || source_id >= MAX_SOURCES) return false;

    // add sound to active sources
    GS_ASSERT(!active_sources[source_id].active);
    if (!active_sources[source_id].active) sources_in_use++;
    active_sources[source_id].active = true;
    active_sources[source_id].two_dimensional = two_dimensional;
    return true;
}

static bool can_add_to_sources(int soundfile_id, int source_id)
{
    GS_ASSERT(soundfile_id >= 0 && soundfile_id < MAX_SOUNDS);
    if (soundfile_id < 0 || soundfile_id >= MAX_SOUNDS) printf("invalid soundfile_id %d\n", soundfile_id);
    if (soundfile_id < 0 || soundfile_id >= MAX_SOUNDS) return false;

    if (sources_in_use >= MAX_SOURCES) printf("no free sources\n");
    if (sources_in_use >= MAX_SOURCES) return false;
    
    bool can = sound_buffers[soundfile_id]->can_add_source(source_id);
    if (!can) printf("can't add to sound_buffer\n");
    if (!can) return false;

    return true;
}

static int play_sound(int source_id, GS_SoundBuffer* sound_buffer, float x, float y, float z, float vx, float vy, float vz, float ox, float oy, float oz)
{
    if (!enabled)
        return -1;

    if (source_id >= MAX_SOURCES || source_id < 0)
        return -1;
        
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

int play_2d_sound(class GS_SoundBuffer* sound_buffer)
{
    if (!enabled)
        return -1;

    // get listener state
    ALfloat x,y,z;
    ALfloat vx,vy,vz;
    ALfloat o[6];
    get_listener_state(&x, &y, &z, &vx, &vy, &vz, o);
    // play at listener state

    // get free source
    int source_id = get_free_source();
    if (source_id >= MAX_SOURCES || source_id < 0)
        return -1;

    // add sound to active sources
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id)) printf("Can't add %s\n", sound_buffer->fn);
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id))
        return -1;

    int ret_source_id = play_sound(source_id, sound_buffer, x,z,y,vx,vz,vy, o[0], o[2], o[1]);
    GS_ASSERT(source_id == ret_source_id);
    if (source_id < 0) return source_id;
    
    // add sound to active sources
    bool added = add_to_sources(sound_buffer->buffer_id, source_id, true);
    GS_ASSERT(added);

    return source_id;
}

int play_2d_sound(const char* fn)
{
    if (!enabled) return -1;
    
    // lookup buffer from file
    class GS_SoundBuffer* sound_buffer = get_sound_buffer_from_function_name(fn);
    GS_ASSERT(sound_buffer != NULL);
    
    if (sound_buffer == NULL) return -1;
    if (sound_buffer->buffer_id < 0) return -1;
    if (sound_buffer->current_sources >= sound_buffer->max_sources) return -1;

    return play_2d_sound(sound_buffer);
}

int play_2d_sound(int soundfile_id)
{
    if (!enabled) return -1;

    GS_SoundBuffer* sound_buffer = get_sound_buffer_from_soundfile_id(soundfile_id);
    if (sound_buffer == NULL
     || sound_buffer->buffer_id < 0
     || sound_buffer->current_sources >= sound_buffer->max_sources)
        return -1;

    return play_2d_sound(sound_buffer);
}

int play_3d_sound(class GS_SoundBuffer* sound_buffer, struct Vec3 p, struct Vec3 v)
{
    if (!enabled) return -1;

    // TODO -- move this to the sound module
    // Need better organization of Soundfile metadata vs GS_SoundBuffer
    if (vec3_distance(Sound::listener_position, p) > sound_buffer->metadata->max_playable_distance) return -1;

    static const struct Vec3 o = vec3_init(0,0,-1);

    // get free source
    int source_id = get_free_source();
    if (source_id >= MAX_SOURCES || source_id < 0)
        return -1;

    // add sound to active sources
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id)) printf("Can't add %s\n", sound_buffer->fn);
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id))
        return -1;
    
    source_id = play_sound(source_id, sound_buffer, p.x, p.y, p.z, v.x, v.y, v.z, o.x, o.y, o.z);
    if (source_id < 0) return source_id;
    // add sound to active sources
    if (!add_to_sources(sound_buffer->buffer_id, source_id, false)) return -1;

    return source_id;
}

int play_3d_sound(const char* fn, struct Vec3 p, struct Vec3 v)
{
    if (!enabled) return -1;

    // lookup buffer from file
    GS_SoundBuffer* sound_buffer = get_sound_buffer_from_function_name(fn);

    if (sound_buffer == NULL) return -1;
    if (sound_buffer->buffer_id < 0) return -1;
    if (sound_buffer->current_sources >= sound_buffer->max_sources) return -1;
        
    return play_3d_sound(sound_buffer, p, v);
}

int play_3d_sound(int soundfile_id, struct Vec3 p, struct Vec3 v)
{
    if (!enabled) return -1;

    GS_SoundBuffer* sound_buffer = get_sound_buffer_from_soundfile_id(soundfile_id);
    if (sound_buffer == NULL
     || sound_buffer->buffer_id < 0
     || sound_buffer->current_sources >= sound_buffer->max_sources)
        return -1;

    return play_3d_sound(sound_buffer, p, v);
}

void update()
{
    if (!enabled) return;
    
    GS_ASSERT(inited);
    GS_ASSERT(active_sources != NULL);
    GS_ASSERT(sound_buffers != NULL);
    GS_ASSERT(sources != NULL);
    if (!inited) return;
    if (active_sources == NULL) return;
    if (sound_buffers == NULL) return;
    if (sources == NULL) return;
    
    // get listener state
    ALfloat x,y,z;
    ALfloat vx,vy,vz;
    ALfloat o[6];
    get_listener_state(&x, &y, &z, &vx, &vy, &vz, o);

    ALint source_state;

    // expire any used sources
    for (int i=0; i<MAX_SOURCES; i++)
    {
        alGetSourcei(sources[i], AL_SOURCE_STATE, &source_state);
        if (source_state == AL_PLAYING)
        {
            GS_ASSERT(active_sources[i].active);    // should already be active
            active_sources[i].active = true;
            if (active_sources[i].two_dimensional)    // update 2d listeners
                update_source_state(sources[i], x,z,y,vx,vz,vy, o[0], o[2], o[1]);
        }
        else
        {
            if (active_sources[i].active) sources_in_use--;
            active_sources[i].active = false;
        }
        
    }

    int rm_sources[MAX_SOURCES] = {-1};
    int rm_sources_index = 0;

    // update sound_buffer metadata
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        GS_SoundBuffer* b = sound_buffers[i];
        if (b == NULL) continue;
        int cs = b->current_sources;
        for (int j=0; j<cs; j++)
        {
            int gs_source_id = b->sources[j];
            GS_ASSERT(gs_source_id >= 0 && gs_source_id < MAX_SOURCES);
            if (gs_source_id < 0 || gs_source_id >= MAX_SOURCES) continue;
            if (!active_sources[gs_source_id].active)
                rm_sources[rm_sources_index++] = j;
        }
        
        for (int i=0; i<rm_sources_index; i++)
            b->remove_source(rm_sources[i]);

        rm_sources_index = 0;
    }
}

void stop_sound(int sound_id)
{
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    GS_ASSERT(sources != NULL);
    if (sources == NULL) return;
    
    alSourceStop(sources[sound_id]);
    checkError();
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

}   // OpenALSound
