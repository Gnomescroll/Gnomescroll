#include "sound.hpp"

#include <sound/packets.hpp>
#include <sound/triggers.hpp>

#if DC_CLIENT
#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>
#include <t_map/t_map.hpp>
#endif

namespace Sound
{

void init_packets()
{
    play_2d_sound_StoC::register_client_packet();
    play_3d_sound_StoC::register_client_packet();
}

void init()
{
    #if DC_CLIENT
    OpenALSound::init();
    #endif
    
    parse_sound_triggers("./media/sound/sounds.csv");
    validate_sound_config();
}

void close()
{
    teardown_triggers();
    #if DC_CLIENT
    OpenALSound::close();
    #endif
}


#if DC_CLIENT
const float GS_SOUND_DISTANCE_CUTOFF = 128.0f;
struct Vec3 listener_position;

void update()
{
    OpenALSound::update();
}

/* Setters */
void set_volume(float vol)
{
    OpenALSound::set_volume(vol);
}

/* Listener (player) */

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz)
{
    if (!Options::sound) return;
    listener_position = vec3_init(x,y,z);
    return OpenALSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
}

// Public
void load_sound(Soundfile* snd)
{
    if (!Options::sound) return;
    return OpenALSound::load_sound(snd);
}

// Public
int play_2d_sound(const char* fn)
{
    if (!Options::sound) return -1;
    return OpenALSound::play_2d_sound(fn);
}

int play_2d_sound(const char* fn, float gain_multiplier, float pitch_multiplier)
{
    if (!Options::sound) return -1;
    return OpenALSound::play_2d_sound(fn, gain_multiplier, pitch_multiplier);
}

int play_2d_sound(int soundfile_id)
{
    if (!Options::sound) return -1;
    return OpenALSound::play_2d_sound(soundfile_id);
}

//Public

// deprecated, use Vec3
int play_3d_sound(const char* fn, float x, float y, float z, float vx, float vy, float vz)
{
    if (!Options::sound) return -1;
    return play_3d_sound(fn, vec3_init(x,y,z), vec3_init(vx,vy,vz));
}

int play_3d_sound(const char* event_name, struct Vec3 p, struct Vec3 v, float gain_multiplier, float pitch_multiplier)
{
    if (!Options::sound) return -1;

    p = translate_position(p);
    p = quadrant_translate_position(listener_position, p);

    float dist = vec3_distance_squared(listener_position, p);
    if (dist > GS_SOUND_DISTANCE_CUTOFF*GS_SOUND_DISTANCE_CUTOFF)
        return -1;

    return OpenALSound::play_3d_sound(event_name, p, v, gain_multiplier, pitch_multiplier);
}

int play_3d_sound(const char* event_name, struct Vec3 p, struct Vec3 v)
{
    return play_3d_sound(event_name, p, v, 1.0f, 1.0f);
}

int play_3d_sound(int soundfile_id, struct Vec3 p, struct Vec3 v)
{
    if (!Options::sound) return -1;

    p = translate_position(p);
    p = quadrant_translate_position(listener_position, p);

    float dist = vec3_distance_squared(listener_position, p);
    if (dist > GS_SOUND_DISTANCE_CUTOFF*GS_SOUND_DISTANCE_CUTOFF)
        return -1;

    return OpenALSound::play_3d_sound(soundfile_id, p, v);
}

void stop_sound(int sound_id)
{
    OpenALSound::stop_sound(sound_id);
}

void set_pitch_multiplier(int sound_id, float pitch)
{
    OpenALSound::set_pitch_multiplier(sound_id, pitch);
}

void set_gain_multiplier(int sound_id, float gain)
{
    OpenALSound::set_gain_multiplier(sound_id, gain);
}

/*
    UTILITIES
                */
int test()
{
    return OpenALSound::test();
}

void enumerate_sound_devices()
{
    OpenALSound::enumerate_devices();
}

#endif

#if DC_SERVER
void send_play_2d_sound(const char* name, ClientID client_id)
{
    int sound_id = get_soundfile_id_for_name(name);
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    play_2d_sound_StoC msg;
    msg.sound_id = sound_id;
    msg.sendToClient(client_id);
}

void broadcast_play_2d_sound(const char* name)
{
    int sound_id = get_soundfile_id_for_name(name);
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    play_2d_sound_StoC msg;
    msg.sound_id = sound_id;
    msg.broadcast();
}

void send_play_3d_sound(const char* name, ClientID client_id, struct Vec3 p)
{
    int sound_id = get_soundfile_id_for_name(name);
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    play_3d_sound_StoC msg;
    msg.sound_id = sound_id;
    msg.position = p;
    msg.sendToClient(client_id);
}

void broadcast_play_3d_sound(const char* name, struct Vec3 p)
{
    int sound_id = get_soundfile_id_for_name(name);
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    play_3d_sound_StoC msg;
    msg.sound_id = sound_id;
    msg.position = p;
    msg.broadcast();
}

void broadcast_exclude_play_3d_sound(const char* name, struct Vec3 p, int ignore_client_id)
{
    int sound_id = get_soundfile_id_for_name(name);
    GS_ASSERT(sound_id >= 0);
    if (sound_id < 0) return;
    play_3d_sound_StoC msg;
    msg.sound_id = sound_id;
    msg.position = p;
    msg.broadcast_exclude(ignore_client_id);
}
#endif

}   // Sound
