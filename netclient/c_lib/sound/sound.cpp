#include "sound.hpp"

namespace Sound
{

void init()
{
    OpenALSound::init();
    parse_sound_triggers((char*)"./media/sound/sounds.csv");
}

void update() {
    OpenALSound::update();
}

/* Setters */
void set_volume(float vol) {
    OpenALSound::set_volume(vol);
}

/* Listener (player) */

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz) {
    return OpenALSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
}

// Public
void load_sound(Soundfile* snd) {
    return OpenALSound::load_sound(snd);
}

// Public
int play_2d_sound(char* fn) {
    return OpenALSound::play_2d_sound(fn);
}

//Public
int play_3d_sound(char* fn, float x, float y, float z, float vx, float vy, float vz) {
    return OpenALSound::play_3d_sound(fn, x,y,z, vx,vy,vz);
}

void close() {
    return OpenALSound::close();
}

/*
    UTILITIES
                */
int test() {
    return OpenALSound::test();
}

//CYTHON
void enumerate_sound_devices()
{
    OpenALSound::enumerate_devices();
}

}
