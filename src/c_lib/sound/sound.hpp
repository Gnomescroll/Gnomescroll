#pragma once

#include <common/defines.h>
#include <net_lib/common/types.hpp>
#include <physics/vec3.hpp>

namespace Sound
{

void init();
void close();

void init_packets();

#if DC_CLIENT

extern Vec3 listener_position;

void set_volume(float vol);

void load_sound(class Soundfile* snd);

int play_2d_sound(const char* event_name);
inline int play_2d_sound(const char* event_name, float gain_multiplier, float pitch_multiplier);

inline int play_3d_sound(const char* event_name, const Vec3& p);
inline int play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v);
inline int play_3d_sound(const char* event_name, const Vec3& p, float gain_multiplier, float pitch_multiplier);
int play_3d_sound(const char* event_name, Vec3 p, Vec3 v, float gain_multiplier, float pitch_multiplier);

int play_2d_sound(int soundfile_id);
int play_3d_sound(int soundfile_id, Vec3 p, Vec3 v);

void stop_sound(int sound_id);

void update_listener(const Vec3& p, const Vec3& v, const Vec3& f, const Vec3& u);
void update();

void set_pitch_multiplier(int sound_id, float pitch);
void set_gain_multiplier(int sound_id, float gain);

int test();

void enumerate_sound_devices();
#endif

#if DC_SERVER
void send_play_2d_sound(const char* name, ClientID client_id);
void broadcast_play_2d_sound(const char* name);
void send_play_3d_sound(const char* name, ClientID client_id, const Vec3& p);
void broadcast_play_3d_sound(const char* name, const Vec3& p);
void broadcast_exclude_play_3d_sound(const char* name, const Vec3& p, int ignore_client_id);
#endif

}   // Sound
