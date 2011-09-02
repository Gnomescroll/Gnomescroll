#include "./api/inc/fmod.h"
#include "./api/inc/fmod_errors.h"
#include "wincompat.h"
#include <stdio.h>

FMOD_SYSTEM *sound_sys;

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

int init_sound_system() {
    FMOD_RESULT result;

    result = FMOD_System_Create(&sound_sys);   // create system
    ERRCHECK(result);

    result = FMOD_System_Init(sound_sys, 100, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
    return 0;
}

void update_sound_system() {
    FMOD_System_Update(sound_sys);
}

void end_sound_system() {
    FMOD_System_Release(sound_sys);
}

FMOD_SOUND* load_sound(const char *soundfile, int three_d) {
    FMOD_SOUND* sound;
    FMOD_RESULT result;

    if (three_d) {
        result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_SOFTWARE | FMOD_3D, 0, &sound);
    } else {
        result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_SOFTWARE | FMOD_2D, 0, &sound);
    }
    ERRCHECK(result);

    return sound;
}

FMOD_SOUND* player_gunshot;
int load_player_gunshot() { // 2d gunshot sound, for when player fires
    player_gunshot = load_sound("./wav/semishoot.wav", FALSE);
    return 0;
}

int play_player_gunshot() {
    FMOD_RESULT result = FMOD_System_PlaySound(sound_sys, FMOD_CHANNEL_FREE, player_gunshot, FALSE, NULL);
    ERRCHECK(result);
}


/* Testing */
int main(int argc, char *argv[])
{
    int i = 0;
    init_sound_system();
    load_player_gunshot();
    do {
        play_player_gunshot();
        Sleep(500);
        i = i + 1;
    } while (i < 10);
    
    end_sound_system();
    return 0;
}
