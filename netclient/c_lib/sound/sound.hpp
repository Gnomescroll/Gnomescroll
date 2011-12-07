#include <fmod.h>
#include <fmod_errors.h>
#include <c_lib/sound/wincompat.h>
#include <stdio.h>

namespace Sound {

class Soundfile {
    public:
        unsigned int hash;
        int sound2d;
        int sound3d;
};

void init(float vol);
void close();

void set_volume(float vol);
void set_enabled(int y);
void set_sound_path(char* path);

void update();

// Sounds
void load_sound(char* file);

int play_2d_sound(int snd_id);
int play_3d_sound(int snd_id, float x, float y, float z, float vx, float vy, float vz);
void end_sound(int snd_id);

// Channels
int update_channel(int ch_id, float x, float y, float z, float vx, float vy, float vz);

// Listener
void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);

// Vectors
const FMOD_VECTOR create_vector(float x, float y, float z);
void set_vector(FMOD_VECTOR* vec, float x, float y, float z);

// Debug
int test();

}
