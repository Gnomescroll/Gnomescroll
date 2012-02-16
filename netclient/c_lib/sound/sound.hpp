#include <fmod.h>
#include <fmod_errors.h>
#include <c_lib/sound/wincompat.h>
#include <stdio.h>

#define USE_FMOD 0
#define USE_OPENAL 1

namespace Sound {

void init();
void close();

void set_volume(float vol);

void load_sound(char* file);

int play_2d_sound(char* file);
int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz);

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);
void update();

int test();

}
