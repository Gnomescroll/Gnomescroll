#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace OpenALSound
{

using Sound::Soundfile;

void init();
void close();

void set_volume(float vol);

void load_sound(Soundfile* snd);

int play_2d_sound(const char* file);
int play_3d_sound(const char* file, struct Vec3 p, struct Vec3 v);

int play_2d_sound(int soundfile_id);
int play_3d_sound(int soundfile_id, struct Vec3 p, struct Vec3 v);

void stop_sound(int sound_id);

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);
void update();

int test();

void enumerate_devices();

}   // OpenALSound
