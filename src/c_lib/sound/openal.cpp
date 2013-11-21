/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "openal.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <sound/sound.hpp>
#include <sound/soundfiles.hpp>
#include <common/compat_al.h>
#include <physics/vec3.hpp>

namespace OpenALSound
{

const int MAX_SOURCES_PER_SAMPLE = 6;
static const int MAX_SOUNDS = Sound::MAX_WAV_BUFFERS;
static const ALsizei MAX_BUFFERS = Sound::MAX_WAV_BUFFERS;
static const ALsizei MAX_SOURCES = 16;

// lookup table
class GSSoundBuffer
{
    public:
        int id;
        char* event_name;
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
        IF_ASSERT(source_id < 0 || source_id >= MAX_SOURCES)
        {
            printf("Can't add %d because invalid id\n", source_id);
            return false;
        }

        for (int i=0; i<this->current_sources; i++)
        {
            if (this->sources[i] == source_id)
            {
                printf("Can't add %d because this source is already added\n", source_id);
                return false;
            }
        }
        IF_ASSERT(this->current_sources < 0)
            this->current_sources = 0;
        if (this->current_sources >= this->max_sources)
        {
            printf("Can't add %d because max sources reached for this buffer\n", source_id);
            return false;
        }
        return true;
    }

    void remove_source(int index)
    {
        IF_ASSERT(index < 0 || index >= this->max_sources) return;

        IF_ASSERT(this->current_sources <= 0)
        {
            this->current_sources = 0;
            return;
        }

        IF_ASSERT(index >= this->current_sources) return;

        if (index == this->current_sources-1)
            this->current_sources--;
        else
            this->sources[index] = this->sources[--this->current_sources];
    }

    GSSoundBuffer() :
        id(-1), event_name(NULL), buffer_id(-1), loaded(false),
        metadata(NULL), max_sources(MAX_SOURCES_PER_SAMPLE),
        current_sources(0)
    {
        this->sources = (int*)malloc(sizeof(int) * this->max_sources);
        for (int i=0; i<this->max_sources; this->sources[i++] = -1);
    }

    ~GSSoundBuffer()
    {
        free(this->sources);
        free(this->event_name);
    }
};

static GSSoundBuffer** sound_buffers = NULL;
static int soundfile_index = 0;

static bool enabled = true;
static bool inited = false;
static bool buffers_inited = false;
static bool sources_inited = false;
static ALCdevice* device = NULL;
static ALCcontext* context = NULL;

static ALuint* buffers = NULL;
static int buffer_index = 0;

static ALuint* sources = NULL;

static SoundID sound_id_index = NULL_SOUND_ID;

class GSSoundSource
{
    public:
        int id;
        SoundID sound_id;
        int buffer_id;
        bool active;
        bool two_dimensional;
        // add AL source properties here so we can vary per source easily
        float pitch_multiplier;
        float gain_multiplier;

    void use(int buffer_id)
    {
        GS_ASSERT(buffer_id >= 0 && buffer_id < MAX_SOUNDS);
        this->buffer_id = buffer_id;
        this->active = true;
        this->two_dimensional = true;
        this->pitch_multiplier = 1.0f;
        this->gain_multiplier = 1.0f;
    }

    void deactivate()
    {
        this->active = false;
        this->sound_id = NULL_SOUND_ID;
    }

    GSSoundSource() :
        id(-1), sound_id(NULL_SOUND_ID), buffer_id(-1),
        active(false), two_dimensional(false),
        pitch_multiplier(1.0f), gain_multiplier(1.0f)
    {}
};

static GSSoundSource* active_sources = NULL;
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

void update_listener(const Vec3& p, const Vec3& v, const Vec3& f, const Vec3& u)
{
    // flip y and z to map to openal's coordinate space
    if (!enabled) return;
    alListener3f(AL_POSITION, p.x, p.z, p.y);
    alListener3f(AL_VELOCITY, v.x, v.z, v.y);
    float o[6];
    o[0] = f.x;
    o[1] = f.z;
    o[2] = f.y;
    o[3] = u.x;
    o[4] = u.z;
    o[5] = u.y;
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
    sound_buffers = (GSSoundBuffer**)calloc(MAX_SOUNDS, sizeof(GSSoundBuffer*));

    Sound::init_wav_buffers();

    const char* default_device = enumerate_devices();
    GS_ASSERT(default_device != NULL);

    // open device
    if (Options::sound_device[0] != '\0')
    {
        printf("Using device: %s\n", Options::sound_device);
        device = alcOpenDevice(Options::sound_device);
    }
    else
    {
        if (default_device != NULL)
            printf("Using device: %s\n", default_device);
        device = alcOpenDevice(NULL); // select the "preferred device"
    }

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
    set_volume(float(Options::sfx) / 100.0f);
    update_listener(vec3_init(0), vec3_init(0), vec3_init(1,0,0), vec3_init(0,0,1));
    if (checkError())
    {
        close();
        enabled = false;
        return;
    }

    // init active sources buffer
    active_sources = new GSSoundSource[MAX_SOURCES];
    for (int i=0; i<MAX_SOURCES; i++) active_sources[i].id = i;

    enabled = true;
    inited = true;
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
    if (!enabled) return;

    // check if file has been loaded
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        GSSoundBuffer* s = sound_buffers[i];
        if (s == NULL) continue;
        if (s->metadata != NULL && strcmp(s->metadata->filename, snd->filename) == 0)
        {   // already loaded this wav file into an openal buffer.
            // create a new instance of GS_Soundbuffer and copy the OpenAL buffer id
            GSSoundBuffer* new_s = new GSSoundBuffer;
            new_s->id = soundfile_index++;
            new_s->event_name = (char*)malloc((strlen(snd->event_name)+1)*sizeof(char));
            strcpy(new_s->event_name, snd->event_name);
            new_s->buffer_id = s->buffer_id;
            new_s->loaded = true;
            new_s->metadata = snd;
            sound_buffers[new_s->id] = new_s;
            return;
        }
    }

    // we havent loaded this file into an OpenAL buffer yet. do it now, then create a new GSSoundBuffer

    if (buffer_index == MAX_BUFFERS)
    {
        printf("ERROR OpenALSound::load_sound -- no AL buffers available\n");
        return;
    }
    unsigned char* buffer = NULL;

    // Load test.wav
    int data_id = Sound::load_wav_file(snd->filename, &buffer);
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
        printf("WARNING: audio file %s is in stereo format. 3D sound will not be applied for this sample.\n", snd->filename);

    // put the PCM data into the alBuffer
    // (this will copy the buffer, so we must free our PCM buffer)
    alBufferData(buffers[buffer_index], fmt, buffer, data->size, data->sample_rate);
    free(buffer);
    if (checkError())
        return;

    // put in lookup table for playback
    GSSoundBuffer* s = new GSSoundBuffer;
    s->id = soundfile_index++;
    s->event_name = (char*)malloc((strlen(snd->event_name)+1) * sizeof(char));
    strcpy(s->event_name, snd->event_name);
    s->buffer_id = buffer_index++;
    s->loaded = true;
    s->metadata = snd;
    sound_buffers[s->id] = s;
}

static GSSoundSource* get_source_from_sound_id(SoundID sound_id)
{
    if (sound_id == NULL_SOUND_ID) return NULL;
    for (int i=0; i<MAX_SOURCES; i++)
        if (active_sources[i].sound_id == sound_id)
            return &active_sources[i];
    return NULL;
}

static int get_free_source()
{
    for (int i=0; i<MAX_SOURCES; i++)
        if (!active_sources[i].active)
            return i;
    return -1;
}

GSSoundBuffer* get_sound_buffer_from_event_name(const char* event_name)
{
    if (sound_buffers == NULL) return NULL;
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        if (sound_buffers[i] != NULL && sound_buffers[i]->loaded &&
            strcmp(sound_buffers[i]->event_name, event_name) == 0)
        {
            return sound_buffers[i];
        }
    }
    printf("No sound buffer found for event_name %s\n", event_name);
    return NULL;
}

GSSoundBuffer* get_sound_buffer_from_soundfile_id(int soundfile_id)
{
    IF_ASSERT(soundfile_id < 0 || soundfile_id >= MAX_SOUNDS) return NULL;
    if (sound_buffers[soundfile_id] == NULL) return NULL;
    if (!sound_buffers[soundfile_id]->loaded) return NULL;
    return sound_buffers[soundfile_id];
}

void set_pitch_multiplier(SoundID sound_id, float pitch)
{
    IF_ASSERT(pitch < 0.0f) pitch = 0.0f;
    GSSoundSource* source = get_source_from_sound_id(sound_id);
    if (source == NULL) return;
    active_sources[source->id].pitch_multiplier = pitch;
}

void set_gain_multiplier(SoundID sound_id, float gain)
{
    IF_ASSERT(gain < 0.0f) gain = 0.0f;
    GSSoundSource* source = get_source_from_sound_id(sound_id);
    if (source == NULL) return;
    active_sources[source->id].gain_multiplier = gain;
}

static bool set_source_properties(int source_id, const Soundfile* snd, const GSSoundSource* active_source)
{
    if (source_id < 0 || source_id >= MAX_SOURCES)
    {
        printf("WARNING -- set_source_properties -- source_id %d out of range\n", source_id);
        return false;
    }
    if (snd == NULL)
    {
        printf("WARNING -- set_source_properties -- snd is NULL\n");
        return false;
    }

    float pitch = snd->pitch;
    if (active_source != NULL)
        pitch *= active_source->pitch_multiplier;
    float gain = snd->gain;
    if (active_source != NULL)
        gain *= active_source->gain_multiplier;

    alSourcef(sources[source_id], AL_PITCH, pitch);
    alSourcef(sources[source_id], AL_GAIN, gain);
    alSourcef(sources[source_id], AL_MAX_DISTANCE, snd->max_distance);
    alSourcef(sources[source_id], AL_REFERENCE_DISTANCE, snd->reference_distance);
    alSourcef(sources[source_id], AL_MIN_GAIN, snd->minimum_gain);
    alSourcef(sources[source_id], AL_MAX_GAIN, snd->maximum_gain);
    alSourcef(sources[source_id], AL_ROLLOFF_FACTOR, snd->rolloff_factor);
    ALboolean loop = (snd->loop) ? AL_TRUE : AL_FALSE;
    alSourcei(sources[source_id], AL_LOOPING, loop);
    return (!checkError());
}

static bool update_source_state(int id, const Vec3& position, const Vec3& velocity,
                                const Vec3& orientation)
{
    alSource3f(id, AL_POSITION, position.x, position.z, position.y);
    alSource3f(id, AL_VELOCITY, velocity.x, velocity.z, velocity.y);
    alSource3f(id, AL_DIRECTION, orientation.x, orientation.z, orientation.y);
    return (!checkError());
}

static void get_listener_state(Vec3& position, Vec3& velocity, Vec3& at, Vec3& up)
{
    alGetListener3f(AL_POSITION, &position.x, &position.z, &position.y);
    alGetListener3f(AL_VELOCITY, &velocity.x, &velocity.z, &velocity.y);
    ALfloat o[6];
    alGetListenerfv(AL_ORIENTATION, o);
    for (int i=0; i<3; i++)
        at.f[i] = o[i];
    for (int i=0; i<3; i++)
        up.f[i] = o[i+3];
}

static bool add_to_sources(int buffer_id, int source_id, bool two_dimensional)
{
    IF_ASSERT(buffer_id < 0 || buffer_id >= MAX_SOUNDS) return false;
    IF_ASSERT(source_id < 0 || source_id >= MAX_SOURCES) return false;

    // add sound to active sources
    GS_ASSERT(!active_sources[source_id].active);
    if (!active_sources[source_id].active) sources_in_use++;
    active_sources[source_id].use(buffer_id);
    active_sources[source_id].two_dimensional = two_dimensional;
    return true;
}

static bool remove_from_sources(int source_id)
{
    IF_ASSERT(sources_in_use <= 0) return false;
    IF_ASSERT(source_id < 0 || source_id >= MAX_SOURCES) return false;
    IF_ASSERT(!active_sources[source_id].active) return false;

    sources_in_use--;
    active_sources[source_id].deactivate();
    return true;
}

static bool can_add_to_sources(int soundfile_id, int source_id)
{
    IF_ASSERT(soundfile_id < 0 || soundfile_id >= MAX_SOUNDS)
    {
        printf("invalid soundfile_id %d\n", soundfile_id);
        return false;
    }

    if (sources_in_use >= MAX_SOURCES)
    {
        printf("no free sources\n");
        return false;
    }

    bool can = sound_buffers[soundfile_id]->can_add_source(source_id);
    if (!can)
    {
        printf("can't add to sound_buffer\n");
        return false;
    }

    return true;
}

static SoundID generate_sound_id(int source_id)
{
    IF_ASSERT(source_id < 0 || source_id >= MAX_SOURCES) return NULL_SOUND_ID;
    GSSoundSource* source = &active_sources[source_id];
    GS_ASSERT(source->active);
    sound_id_index = SoundID(int(sound_id_index) + 1);
    source->sound_id = sound_id_index;
    return source->sound_id;
}

static SoundID play_sound(int source_id, const GSSoundBuffer* sound_buffer,
                          const GSSoundSource* active_source,
                          const Vec3& position, const Vec3& velocity,
                          const Vec3& orientation)
{
    if (!enabled)
        return NULL_SOUND_ID;

    if (source_id >= MAX_SOURCES || source_id < 0)
        return NULL_SOUND_ID;

    // set source properties
    if (!set_source_properties(source_id, sound_buffer->metadata, active_source))
        return NULL_SOUND_ID;

    // set source state
    if (!update_source_state(sources[source_id], position, velocity, orientation))
        return NULL_SOUND_ID;

    // Attach buffer 0 to source
    alSourcei(sources[source_id], AL_BUFFER, buffers[sound_buffer->buffer_id]);
    if (checkError())
        return NULL_SOUND_ID;

    // play
    alSourcePlay(sources[source_id]);
    if (checkError())
        return NULL_SOUND_ID;

    return generate_sound_id(source_id);
}

static SoundID play_2d_sound(GSSoundBuffer* sound_buffer, float gain_multiplier, float pitch_multiplier)
{
    if (!enabled)
        return NULL_SOUND_ID;

    // get listener state
    Vec3 position;
    Vec3 velocity;
    Vec3 at;
    Vec3 up;
    get_listener_state(position, velocity, at, up);

    // get free source
    int source_id = get_free_source();
    if (source_id < 0 || source_id >= MAX_SOURCES)
        return NULL_SOUND_ID;

    // add sound to active sources
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id))
    {
        printf("Can't add %s\n", sound_buffer->event_name);
        return NULL_SOUND_ID;
    }

    // add sound to active sources
    bool added = add_to_sources(sound_buffer->id, source_id, true);
    IF_ASSERT(!added) return NULL_SOUND_ID;
    GSSoundSource* active_source = &active_sources[source_id];
    active_source->gain_multiplier = gain_multiplier;
    active_source->pitch_multiplier = pitch_multiplier;

    SoundID sound_id = play_sound(source_id, sound_buffer, active_source, position, velocity, at);
    if (sound_id == NULL_SOUND_ID)
    {   // play sound failed, back out
        remove_from_sources(source_id);
    }
    return sound_id;
}

SoundID play_2d_sound(const char* event_name, float gain_multiplier, float pitch_multiplier)
{
    if (!enabled)
        return NULL_SOUND_ID;

    // lookup buffer from file
    GSSoundBuffer* sound_buffer = get_sound_buffer_from_event_name(event_name);
    GS_ASSERT(sound_buffers == NULL || sound_buffer != NULL);

    if (sound_buffer == NULL)
        return NULL_SOUND_ID;
    if (sound_buffer->buffer_id < 0)
        return NULL_SOUND_ID;
    if (sound_buffer->current_sources >= sound_buffer->max_sources)
        return NULL_SOUND_ID;

    return play_2d_sound(sound_buffer, gain_multiplier, pitch_multiplier);
}

SoundID play_2d_sound(const char* event_name)
{
    return play_2d_sound(event_name, 1.0f, 1.0f);
}

SoundID play_2d_sound(int soundfile_id)
{
    if (!enabled)
        return NULL_SOUND_ID;

    GSSoundBuffer* sound_buffer = get_sound_buffer_from_soundfile_id(soundfile_id);
    if (sound_buffer == NULL ||
        sound_buffer->buffer_id < 0 ||
        sound_buffer->current_sources >= sound_buffer->max_sources)
    {
        return NULL_SOUND_ID;
    }

    return play_2d_sound(sound_buffer, 1.0f, 1.0f);
}

static SoundID play_3d_sound(GSSoundBuffer* sound_buffer, const Vec3& p, const Vec3& v, float gain_multiplier, float pitch_multiplier)
{
    if (!enabled)
        return NULL_SOUND_ID;

    // TODO -- move this to the sound module
    // Need better organization of Soundfile metadata vs GSSoundBuffer
    if (vec3_distance(Sound::listener_position, p) >
            sound_buffer->metadata->max_playable_distance)
        return NULL_SOUND_ID;

    static const Vec3 o = vec3_init(0, 0, 1);

    // get free source
    int source_id = get_free_source();
    if (source_id < 0 || source_id >= MAX_SOURCES)
        return NULL_SOUND_ID;

    // add sound to active sources
    if (!can_add_to_sources(sound_buffer->buffer_id, source_id))
    {
        printf("Can't add %s\n", sound_buffer->event_name);
        return NULL_SOUND_ID;
    }

    if (!add_to_sources(sound_buffer->id, source_id, false))
        return NULL_SOUND_ID;
    GSSoundSource* active_source = &active_sources[source_id];
    active_source->gain_multiplier = gain_multiplier;
    active_source->pitch_multiplier = pitch_multiplier;

    SoundID sound_id = play_sound(source_id, sound_buffer, active_source, p, v, o);
    if (sound_id == NULL_SOUND_ID)
    {   // error, back out
        remove_from_sources(source_id);
    }
    active_source->sound_id = sound_id;
    return sound_id;
}

SoundID play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v, float gain_multiplier, float pitch_multiplier)
{
    if (!enabled)
        return NULL_SOUND_ID;

    // lookup buffer from file
    GSSoundBuffer* sound_buffer = get_sound_buffer_from_event_name(event_name);
    GS_ASSERT(sound_buffers == NULL || sound_buffer != NULL);

    if (sound_buffer == NULL)
        return NULL_SOUND_ID;
    if (sound_buffer->buffer_id < 0)
        return NULL_SOUND_ID;
    if (sound_buffer->current_sources >= sound_buffer->max_sources)
        return NULL_SOUND_ID;

    return play_3d_sound(sound_buffer, p, v, gain_multiplier, pitch_multiplier);
}

SoundID play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v)
{
    return play_3d_sound(event_name, p, v, 1.0f, 1.0f);
}

SoundID play_3d_sound(int soundfile_id, const Vec3& p, const Vec3& v)
{
    if (!enabled)
        return NULL_SOUND_ID;

    GSSoundBuffer* sound_buffer = get_sound_buffer_from_soundfile_id(soundfile_id);
    if (sound_buffer == NULL ||
        sound_buffer->buffer_id < 0 ||
        sound_buffer->current_sources >= sound_buffer->max_sources)
    {
        return NULL_SOUND_ID;
    }

    return play_3d_sound(sound_buffer, p, v, 1.0f, 1.0f);
}

void update()
{
    if (!enabled) return;

    IF_ASSERT(!inited) return;
    IF_ASSERT(active_sources == NULL) return;
    IF_ASSERT(sound_buffers == NULL) return;
    IF_ASSERT(sources == NULL) return;

    // get listener state
    Vec3 position;
    Vec3 velocity;
    Vec3 at;
    Vec3 up;
    get_listener_state(position, velocity, at, up);

    // expire any used sources
    for (int i=0; i<MAX_SOURCES; i++)
    {
        GSSoundSource* active_source = &active_sources[i];
        ALint source_state;
        alGetSourcei(sources[i], AL_SOURCE_STATE, &source_state);
        if (source_state == AL_PLAYING)
        {
            GS_ASSERT(active_source->active);      // should already be active
            active_source->active = true;          // in case it isnt
            if (active_source->two_dimensional)    // update 2d listeners
                update_source_state(sources[i], position, velocity, at);

            GS_ASSERT(active_source->buffer_id >= 0 && active_source->buffer_id < MAX_SOUNDS);
            if (active_source->buffer_id >= 0 && active_source->buffer_id < MAX_SOUNDS)
            {
                GSSoundBuffer* sound_buffer = sound_buffers[active_source->buffer_id];
                GS_ASSERT(sound_buffer != NULL);
                if (sound_buffer != NULL)
                    set_source_properties(i, sound_buffer->metadata, active_source);
            }
        }
        else
        {
            if (active_source->active) sources_in_use--;
            active_source->deactivate();
        }

    }

    int rm_sources[MAX_SOURCES] = {-1};
    int rm_sources_index = 0;

    // update sound_buffer metadata
    for (int i=0; i<MAX_SOUNDS; i++)
    {
        GSSoundBuffer* b = sound_buffers[i];
        if (b == NULL) continue;
        int cs = b->current_sources;
        for (int j=0; j<cs; j++)
        {
            int gs_source_id = b->sources[j];
            IF_ASSERT(gs_source_id < 0 || gs_source_id >= MAX_SOURCES)
                continue;
            if (!active_sources[gs_source_id].active)
                rm_sources[rm_sources_index++] = j;
        }

        GS_ASSERT(rm_sources_index <= MAX_SOURCES);

        for (int i=0; i<rm_sources_index; i++)
            b->remove_source(rm_sources[i]);

        rm_sources_index = 0;
    }
}

void stop_sound(SoundID sound_id)
{
    if (sound_id == NULL_SOUND_ID) return;
    IF_ASSERT(sources == NULL) return;
    GSSoundSource* source = get_source_from_sound_id(sound_id);
    if (source == NULL) return;
    IF_ASSERT(source->id < 0 || source->id >= MAX_SOURCES) return;
    alSourceStop(sources[source->id]);
    checkError();
}

bool is_active(SoundID sound_id)
{
    return (get_source_from_sound_id(sound_id) != NULL);
}

int test()
{
    const ALsizei MAX_BUFFERS = 1;
    ALuint buffers[MAX_BUFFERS];

    const ALsizei MAX_SOURCES = 1;
    ALuint sources[MAX_SOURCES];

    alGenBuffers(MAX_BUFFERS, buffers);
    if (checkError())
    {
        printf("alGenBuffers:\n");
        return 1;
    }

    // Generate Sources
    alGenSources(MAX_SOURCES, sources);
    if (checkError())
    {
        printf("alGenSources:\n");
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

    char event_name[] = MEDIA_PATH "sound/wav/plasma_grenade_explode.wav";
    // Load test.wav
    int data_id = Sound::load_wav_file(event_name, &buffer);
    IF_ASSERT(data_id < 0)
    {
        printf("OpenALSound::test -- wav data_id %d invalid\n", data_id);
        return 1;
    }
    IF_ASSERT(buffer == NULL)
    {
        printf("OpenALSound::test -- buffer data is NULL\n");
        return 1;
    }

    Sound::WavData *data = Sound::get_loaded_wav_data(data_id);
    IF_ASSERT(data == NULL)
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
    IF_ASSERT(checkError())
    {
        printf("alBufferData:\n");
        alDeleteBuffers(MAX_BUFFERS, buffers);
        free(buffer);
        return 1;
    }

    free(buffer);
    buffer = NULL;

    // Attach buffer 0 to source
    alSourcei(sources[0], AL_BUFFER, buffers[0]);
    IF_ASSERT(checkError())
    {
        printf("alSourcei: BUFFER\n");
        alDeleteBuffers(MAX_BUFFERS, buffers);
        return 1;
    }

    //play
    alSourcePlay(sources[0]);
    IF_ASSERT(checkError())
    {
        printf("alSourcePlay:\n");
        alDeleteBuffers(MAX_BUFFERS, buffers);
        return 1;
    }

    return 0;
}

const char* enumerate_devices()
{
    if (alcIsExtensionPresent(NULL, (const ALchar*)"ALC_ENUMERATION_EXT") != AL_TRUE)
    {
        printf("OpenAL device enumeration extension is not available.\n");
        return NULL;
    }

    const char* devices = (const char*)alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    const char* default_device = (const char*)alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    if (devices != NULL)
    {
        printf("================\n");
        printf("If you want to use a specific sound device, copy one of these strings and run the program with --sound_device=\"device name\"\n");
        printf("\n");
        printf("Available OpenAL devices:\n");
        const char* device_name = devices;
        size_t len = strlen(device_name);
        while (len)
        {
            printf("%s\n", device_name);
            device_name = &device_name[len+1];
            len = strlen(device_name);
        }
        printf("\n");
        printf("Default device is:\n");
        printf("%s\n", default_device);
        printf("================\n");
    }

    return default_device;
}

}   // OpenALSound
