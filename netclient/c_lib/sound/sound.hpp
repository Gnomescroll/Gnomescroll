#include <fmod.h>
#include <fmod_errors.h>
#include <c_lib/sound/wincompat.h>
#include <stdio.h>

namespace Sound {

void init(float vol);
void close();

void set_volume(float vol);
void set_3D_settings(float doppler_scale, float distance_factor, float rolloff_scale);

void update();

// Sounds
void load_sound(char* file);

int play_2d_sound(char* file);
int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz);
//void end_sound(int snd_id);

// Channels
int update_channel(int ch_id, float x, float y, float z, float vx, float vy, float vz);

// Listener
void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);

// Debug
int test();

//CYTHON
void update_sound();

}
