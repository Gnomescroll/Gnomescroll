#include "sound.h"

//FMOD_SYSTEM *sound_sys;

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    }
}

void init_sound_system(int virtual_voices) {
    FMOD_RESULT result;

    result = FMOD_System_Create(&sound_sys);   // create system
    ERRCHECK(result);

    result = FMOD_System_Init(sound_sys, virtual_voices, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
}

void update_sound_system() {
    FMOD_System_Update(sound_sys);
}

void end_sound_system() {
    FMOD_System_Release(sound_sys);
}

FMOD_SOUND* load_2d_sound(char *soundfile) {
    FMOD_SOUND* sound;
    FMOD_RESULT result;

    result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_2D, 0, &sound);
    ERRCHECK(result);

    return sound;
}

FMOD_SOUND* load_3d_sound(char *soundfile, float mindistance) { // use lower mindistance for quieter things. use 4.0f as default
    FMOD_SOUND* sound;
    FMOD_RESULT result;

    result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_3D, 0, &sound);
    ERRCHECK(result);
    result = FMOD_Sound_Set3DMinMaxDistance(sound, mindistance, 10000.0f);
    ERRCHECK(result);

    return sound;
}

FMOD_CHANNEL* play_2d_sound(FMOD_SOUND* sound) {
    FMOD_CHANNEL* channel = 0;
    FMOD_RESULT result;
    result = FMOD_System_PlaySound(sound_sys, FMOD_CHANNEL_FREE, sound, FALSE, &channel);
    ERRCHECK(result);
    return channel;
}

FMOD_CHANNEL* play_3d_sound(FMOD_SOUND* sound, FMOD_VECTOR pos, FMOD_VECTOR vel) {
    FMOD_CHANNEL* channel = 0;
    FMOD_RESULT result;
    result = FMOD_System_PlaySound(sound_sys, FMOD_CHANNEL_FREE, sound, FALSE, &channel);
    ERRCHECK(result);
    result = FMOD_Channel_Set3DAttributes(channel, &pos, &vel);
    ERRCHECK(result);
    return channel;
}

void release_sound_system(FMOD_SOUND* sound) {
    FMOD_Sound_Release(sound);
}

int main(int argc, char *argv[]){return 0;}
