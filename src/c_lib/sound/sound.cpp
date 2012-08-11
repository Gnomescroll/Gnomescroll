#include "sound.hpp"

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>
#include <t_map/t_map.hpp>

namespace Sound
{

const float GS_SOUND_DISTANCE_CUTOFF = 128.0f;
struct Vec3 listener_position;

void init()
{
    OpenALSound::init();
    parse_sound_triggers("./media/sound/sounds.csv");
}

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
int play_2d_sound(char* fn)
{
    if (!Options::sound) return -1;
    return OpenALSound::play_2d_sound(fn);
}

//Public
int play_3d_sound(char* fn, float x, float y, float z, float vx, float vy, float vz)
{
    if (!Options::sound) return -1;

    x = translate_point(x);
    y = translate_point(y);
    x = quadrant_translate_f(listener_position.x, x);
    y = quadrant_translate_f(listener_position.y, y);

    float dist = vec3_distance_squared(listener_position, vec3_init(x,y,z));
    if (dist > GS_SOUND_DISTANCE_CUTOFF*GS_SOUND_DISTANCE_CUTOFF)
        return -1;

    return OpenALSound::play_3d_sound(fn, x,y,z, vx,vy,vz);
}

void stop_sound(int sound_id)
{
    OpenALSound::stop_sound(sound_id);
}

void close()
{
    return OpenALSound::close();
}

/*
    UTILITIES
                */
int test()
{
    return OpenALSound::test();
}

//CYTHON
void enumerate_sound_devices()
{
    OpenALSound::enumerate_devices();
}

}
