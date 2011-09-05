#include "sound.h"

/* Init and update */
void init_sound_system() {
    FMOD_RESULT result;

    result = FMOD_System_Create(&sound_sys);   // create system
    ERRCHECK(result);

    result = FMOD_System_Init(sound_sys, MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
}

void init_channel_group(float vol) {
    FMOD_RESULT r;
    r = FMOD_System_CreateChannelGroup(sound_sys, "main", &chgroup);
    ERRCHECK(r);
    
    // assign channel group
    r = FMOD_ChannelGroup_SetVolume(chgroup, vol);
    ERRCHECK(r);

    update_listener(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void C_init(float vol) {
    init_sound_system();
    init_channel_group(vol);
}

void update_sound_system() {
    FMOD_System_Update(sound_sys);
    // do not ERRCHECK
}


/* Listener (player) */

FMOD_VECTOR lis_pos;
FMOD_VECTOR lis_vel;
FMOD_VECTOR lis_for;
FMOD_VECTOR lis_up;

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz) {
    set_vector(&lis_pos, x, y, z);
    set_vector(&lis_vel, vx, vy, vz);
    set_vector(&lis_for, fx, fy, fz);
    set_vector(&lis_up, ux, uy, uz);
    
    FMOD_RESULT r = FMOD_System_Set3DListenerAttributes(sound_sys, 0, &lis_pos, &lis_vel, &lis_for, &lis_up);
    ERRCHECK(r);
}


/* Channels */

int _add_channel(FMOD_CHANNEL* ch) {    // to channels array
    int i;
    for (i=0; i<MAX_CHANNELS; i++) {
        if (channels[i] == NULL) {
            channels[i] = ch;
            ////printf("_add_channel :: channel found :: %d\n", i);
            break;
        }
    }
    if (i == MAX_CHANNELS) {
        ////printf("_add_channel :: No NULL channels found\n");
        i = -1;
    }
    return i;
}

int _update_channel(FMOD_CHANNEL* ch, const FMOD_VECTOR pos, const FMOD_VECTOR vel) {
    FMOD_RESULT r = FMOD_Channel_Set3DAttributes(ch, &pos, &vel);
    int i = ERRCHECK(r);
    return i;
}

int update_channel(int ch_id, float x, float y, float z, float vx, float vy, float vz) {
    int i = -1;
    if (ch_id < 0 || ch_id >= MAX_CHANNELS) {
        return i;
    }
    FMOD_CHANNEL* ch = channels[ch_id];
    if (ch == NULL) {
        const FMOD_VECTOR pos = create_vector(x,y,z);
        const FMOD_VECTOR vel = create_vector(vx,vy,vz);
        i = _update_channel(ch, pos, vel);
    }
    return i;
}


/* Sounds */

int _add_sound(FMOD_SOUND* snd) {   // to sounds array
    int i;
    for (i=0; i<MAX_SOUNDS; i++) {
        if (sounds[i] == NULL) {
            sounds[i] = snd;
            ////printf("_add_sound :: sound found :: %d\n", i);
            break;
        }
    }
    if (i == MAX_SOUNDS) {
        ////printf("_add_sound :: No NULL sounds found, reached max :: %d\n", i);
        i = -1;
    }
    return i;
}

FMOD_SOUND* _load_2d_sound(char *soundfile) {
    FMOD_SOUND* sound;
    FMOD_RESULT result;

    result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_2D, 0, &sound);
    ERRCHECK(result);

    return sound;
}

FMOD_SOUND* _load_3d_sound(char *soundfile, float mindistance) { // use lower mindistance for quieter things. use 4.0f as default
    FMOD_SOUND* sound;
    FMOD_RESULT result;

    result = FMOD_System_CreateSound(sound_sys, soundfile, FMOD_3D, 0, &sound);
    ERRCHECK(result);
    result = FMOD_Sound_Set3DMinMaxDistance(sound, mindistance, 10000.0f);
    ERRCHECK(result);

    return sound;
}

FMOD_CHANNEL* _play_2d_sound(FMOD_SOUND* sound) {
    FMOD_CHANNEL* channel = 0;
    FMOD_RESULT result;

    result = FMOD_System_PlaySound(sound_sys, FMOD_CHANNEL_FREE, sound, TRUE, &channel);
    ERRCHECK(result);
    result = FMOD_Channel_SetChannelGroup(channel, chgroup);
    ERRCHECK(result);
    result = FMOD_Channel_SetPaused(channel, FALSE);
    ERRCHECK(result);

    return channel;
}


FMOD_CHANNEL* _play_3d_sound(FMOD_SOUND* sound, const FMOD_VECTOR pos, const FMOD_VECTOR vel) {
    FMOD_CHANNEL* channel = 0;
    FMOD_RESULT result;

    result = FMOD_System_PlaySound(sound_sys, FMOD_CHANNEL_FREE, sound, TRUE, &channel);
    ERRCHECK(result);
    result = FMOD_Channel_SetChannelGroup(channel, chgroup);
    ERRCHECK(result);
    result = FMOD_Channel_Set3DAttributes(channel, &pos, &vel);
    ERRCHECK(result);
    result = FMOD_Channel_SetPaused(channel, FALSE);
    ERRCHECK(result);

    return channel;
}


//Public
int load_2d_sound(char *soundfile) {
    FMOD_SOUND* snd = _load_2d_sound(soundfile);
    int i = _add_sound(snd);
    return i;
}

//Public
int load_3d_sound(char *soundfile, float mindistance) {
    //printf("load_3d_sound :: request :: %s %f\n", soundfile, mindistance);
    FMOD_SOUND* snd = _load_3d_sound(soundfile, mindistance);
    int i = _add_sound(snd);
    //printf("load_3d_sound :: loaded :: %d\n", i);
    return i;
}

//Public
int play_2d_sound(int snd_id) {
    int i = -1;
    if (snd_id < 0 || snd_id >= MAX_SOUNDS) {
        return i;
    }
    FMOD_CHANNEL* ch = NULL;
    FMOD_SOUND* snd = sounds[snd_id];
    if (snd != NULL) {
        ch =_play_2d_sound(snd);
        i = _add_channel(ch);
    }
    return i;
}

//Public
int play_3d_sound(int snd_id, float x, float y, float z, float vx, float vy, float vz) {
    int i = -1;
    if (snd_id < 0 || snd_id >= MAX_SOUNDS) {
        //printf("play_3d_sound :: snd_id out of range :: %d\n", snd_id);
        return i;
    }
    FMOD_SOUND* snd = sounds[snd_id];
    FMOD_CHANNEL* ch = NULL;
    if (snd != NULL) {
        const FMOD_VECTOR pos = create_vector(x,y,z);
        const FMOD_VECTOR vel = create_vector(vx,vy,vz);
        ch = _play_3d_sound(snd, pos, vel);
        i = _add_channel(ch);
        //printf("play_3d_sound :: channel added :: %d\n", i);
    }
    return i;
}


/* Completion */
void release_sound(FMOD_SOUND* sound) {
    FMOD_RESULT r = FMOD_Sound_Release(sound);
    ERRCHECK(r);
}

void end_sound(int snd_id) {
    if (snd_id < 0 || snd_id >= MAX_SOUNDS) {
        return;
    }
    FMOD_SOUND* snd = sounds[snd_id];
    if (snd != NULL) {
        release_sound(snd);
    }
    sounds[snd_id] = NULL;
}

void release_sound_system() {
    FMOD_RESULT result = FMOD_System_Release(sound_sys);
    ERRCHECK(result);
}

void release_channel_group() {
    FMOD_RESULT result = FMOD_ChannelGroup_Release(chgroup);
    ERRCHECK(result);
}

void release_globals() {
    release_channel_group();
    release_sound_system();
}

void release_sounds() {
    int i;
    for (i=0; i<MAX_SOUNDS; i++) {
        end_sound(i);
    }
}

void release_all() {
    release_sounds();
    release_globals();
}

/*
    UTILITIES
                */

/* Vectors */
const float _tick = 30.0f; // ticks per second
void set_vector(FMOD_VECTOR* vec, float x, float y, float z) {
    // convert velocity units to m/s (comes in at m/tick, tick=33ms)
    x *= _tick;
    y *= _tick;
    z *= _tick;
    vec->x = x;
    vec->y = z; // flip y and z for our coordinate system
    vec->z = y;
}

const FMOD_VECTOR create_vector(float x, float y, float z) {
    x *= _tick;
    y *= _tick;
    z *= _tick;
    const FMOD_VECTOR vec = {x, z, y};  // y and z must be flipped to match our coordinate system
    return vec;
}


/* Debug */

int ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        //print_trace();
        //exit(1);
        return 1;
    }
    return 0;
}

int test() {
    init_sound_system(100);
    FMOD_SOUND* gun = _load_2d_sound("../media/sound/wav/semishoot.wav");

    _play_2d_sound(gun);
    Sleep(1500);

    release_sound(gun);
    release_sound_system();
    return 0;
}

//void
 //print_trace (void)
 //{
   //void *array[10];
   //size_t size;
   //char **strings;
   //size_t i;
 
   //size = backtrace (array, 10);
   //strings = backtrace_symbols (array, size);
 
   ////printf ("Obtained %zd stack frames.\n", size);
 
   //for (i = 0; i < size; i++)
      ////printf ("%s\n", strings[i]);
 
   //free (strings);
 //}
     
