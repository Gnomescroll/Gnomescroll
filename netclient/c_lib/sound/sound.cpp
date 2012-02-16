#include "sound.hpp"

namespace Sound
{

void init() {
    #if USE_FMOD
    FMODSound::init();
    #endif
    #if USE_OPENAL
    OpenALSound::init();
    #endif
}

void update() {
    #if USE_FMOD
    FMODSound::update();
    #endif
    #if USE_OPENAL
    OpenALSound::update();
    #endif
}

/* Setters */
void set_volume(float vol) {
    #if USE_FMOD
    FMODSound::set_volume(vol);
    #endif
    #if USE_OPENAL
    OpenALSound::set_volume(vol);
    #endif
}

/* Listener (player) */

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz) {
    #if USE_FMOD
    return FMODSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
    #endif
    #if USE_OPENAL
    return OpenALSound::update_listener(x,y,z, vx,vy,vz, fx,fy,fz, ux,uy,uz);
    #endif
}

// Public
void load_sound(char *file) {
    #if USE_FMOD
    return FMODSound::load_sound(file);
    #endif
    #if USE_OPENAL
    return OpenALSound::load_sound(file);
    #endif
}

// Public
int play_2d_sound(char* file) {
    #if USE_FMOD
    return FMODSound::play_2d_sound(file);
    #endif
    #if USE_OPENAL
    return OpenALSound::play_2d_sound(file);
    #endif
}

//Public
int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz) {
    #if USE_FMOD
    return FMODSound::play_3d_sound(file, x,y,z, vx,vy,vz);
    #endif
    #if USE_OPENAL
    return OpenALSound::play_3d_sound(file, x,y,z, vx,vy,vz);
    #endif
}

void close() {
    #if USE_FMOD
    return FMODSound::close();
    #endif
    #if USE_OPENAL
    return OpenALSound::close();
    #endif
}

/*
    UTILITIES
                */
int test() {
    #if USE_FMOD
    return FMODSound::test();
    #endif
    #if USE_OPENAL
    return OpenALSound::test();
    #endif
}

}
