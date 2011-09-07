#include <fmod.h>
#include <fmod_errors.h>
#include <wincompat.h>
#include <stdio.h>

#define MAX_CHANNELS 100
#define MAX_SOUNDS 200

// Init, globals, system
FMOD_SYSTEM* sound_sys;
void C_init(float vol);
void update_sound_system();
void release_globals();
void release_all();

// Sounds
FMOD_SOUND* sounds[MAX_SOUNDS];
int _add_sound(FMOD_SOUND* snd);
int load_2d_sound(char* soundfile);
int load_3d_sound(char* soundfile, float mindistance);
int play_2d_sound(int snd_id);
int play_3d_sound(int snd_id, float x, float y, float z, float vx, float vy, float vz);
void end_sound(int snd_id);
void release_sound(FMOD_SOUND* sound);

// Channels
FMOD_CHANNELGROUP* chgroup;
FMOD_CHANNEL* channels[MAX_CHANNELS];
int update_channel(int ch_id, float x, float y, float z, float vx, float vy, float vz);

// Listener
void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);

// Vectors
const FMOD_VECTOR create_vector(float x, float y, float z);
void set_vector(FMOD_VECTOR* vec, float x, float y, float z);

// Debug
int ERRCHECK(FMOD_RESULT result);
int test();
//void print_trace();
