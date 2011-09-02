#include "./api/inc/fmod.h"
#include "./api/inc/fmod_errors.h"
#include "wincompat.h"
#include <stdio.h>

FMOD_SYSTEM* sound_sys;

void ERRCHECK(FMOD_RESULT result);

void init_sound_system(int virtual_voices);
void update_sound_system();
void release_sound_system();

FMOD_SOUND* load_2d_sound(char* soundfile);
FMOD_SOUND* load_3d_sound(char* soundfile, float mindistance);

FMOD_CHANNEL* play_2d_sound(FMOD_SOUND* sound);
FMOD_CHANNEL* play_3d_sound(FMOD_SOUND* sound, FMOD_VECTOR pos, FMOD_VECTOR vel);

void release_sound(FMOD_SOUND* sound);
