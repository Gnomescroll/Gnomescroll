#pragma once

#include <common/defines.h>

namespace Sound
{

void init();
void close();

void init_packets();

#if DC_CLIENT

extern struct Vec3 listener_position;

void set_volume(float vol);

void load_sound(class Soundfile* snd);

int play_2d_sound(const char* file);
int play_3d_sound(const char* file, float x, float y, float z, float vx, float vy, float vz);
int play_3d_sound(const char* file, struct Vec3 p, struct Vec3 v);
int play_3d_sound(const char* file, struct Vec3 p) { return play_3d_sound(file, p, vec3_init(0,0,0)); }

int play_2d_sound(int soundfile_id);
int play_3d_sound(int soundfile_id, struct Vec3 p, struct Vec3 v);

void stop_sound(int sound_id);

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);
void update();

int test();

void enumerate_sound_devices();
#endif

#if DC_SERVER
void send_play_2d_sound(const char* name, int client_id);
void broadcast_play_2d_sound(const char* name);
void send_play_3d_sound(const char* name, int client_id, struct Vec3 p);
void broadcast_play_3d_sound(const char* name, struct Vec3 p);
void broadcast_exclude_play_3d_sound(const char* name, struct Vec3 p, int ignore_client_id);
#endif

}   // Sound
