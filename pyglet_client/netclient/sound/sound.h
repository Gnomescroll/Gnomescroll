#include <fmod.h>
#include <fmod_errors.h>
#include <wincompat.h>
#include <stdio.h>

#define MAX_SOUNDS 100

FMOD_SYSTEM* sound_sys;
FMOD_CHANNELGROUP* chgroup;

int ERRCHECK(FMOD_RESULT result);

void C_init(float vol);
void update_sound_system();
void release_globals();

FMOD_SOUND* load_2d_sound(char* soundfile);
FMOD_SOUND* load_3d_sound(char* soundfile, float mindistance);
FMOD_CHANNEL* play_2d_sound(FMOD_SOUND* sound);
FMOD_CHANNEL* play_3d_sound(FMOD_SOUND* sound, FMOD_VECTOR pos, FMOD_VECTOR vel);

void release_sound(FMOD_SOUND* sound);

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);
int update_channel(FMOD_CHANNEL* ch, const FMOD_VECTOR pos, const FMOD_VECTOR vel);

const FMOD_VECTOR create_vector(float x, float y, float z);

int test();
void print_trace();
