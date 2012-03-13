#include "sound.hpp"

namespace Sound
{

void init()
{
    #ifdef USE_FMOD
    FMODSound::init();
    #endif
    #ifdef USE_OPENAL
    OpenALSound::init();
    #endif
    parse_sound_triggers((char*)"./media/sound/sounds.csv");
}

void update() {
    #ifdef USE_FMOD
    FMODSound::update();
    #endif
    #ifdef USE_OPENAL
    OpenALSound::update();
    #endif
}

/* Setters */
void set_volume(float vol) {
    #ifdef USE_FMOD
    FMODSound::set_volume(vol);
    #endif
    #ifdef USE_OPENAL
    OpenALSound::set_volume(vol);
    #endif
}

/* Listener (player) */

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz) {
    #ifdef USE_FMOD
    return FMODSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
    #endif
}

// Public
void load_sound(Soundfile* snd) {
    #ifdef USE_FMOD
    return FMODSound::load_sound(snd);
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::load_sound(snd);
    #endif
}

// Public
int play_2d_sound(char* fn) {
    #ifdef USE_FMOD
    return FMODSound::play_2d_sound(fn);    //outdated
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::play_2d_sound(fn);
    #endif
}

//Public
int play_3d_sound(char* fn, float x, float y, float z, float vx, float vy, float vz) {
    #ifdef USE_FMOD
    return FMODSound::play_3d_sound(fn, x,y,z, vx,vy,vz);
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::play_3d_sound(fn, x,y,z, vx,vy,vz);
    #endif
}

void close() {
    #ifdef USE_FMOD
    return FMODSound::close();
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::close();
    #endif
}

/*
    UTILITIES
                */
int test() {
    #ifdef USE_FMOD
    return FMODSound::test();
    #endif
    #ifdef USE_OPENAL
    return OpenALSound::test();
    #endif
}

}
