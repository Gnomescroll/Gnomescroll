/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/defines.h>
#include <net_lib/common/types.hpp>
#include <physics/vec3.hpp>

typedef enum
{
    NULL_SOUND_ID = 0
} SoundID;

namespace Sound
{

void init();
void close();

void init_packets();

const char* get_random_name(const char* fmt, int lower, int upper);

#if DC_CLIENT

extern Vec3 listener_position;

void set_volume(float vol);

void load_sound(class Soundfile* snd);

SoundID play_2d_sound(const char* event_name);
inline SoundID play_2d_sound(const char* event_name, float gain_multiplier, float pitch_multiplier);

inline SoundID play_3d_sound(const char* event_name, const Vec3& p);
inline SoundID play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v);
inline SoundID play_3d_sound(const char* event_name, const Vec3& p, float gain_multiplier, float pitch_multiplier);
SoundID play_3d_sound(const char* event_name, Vec3 p, Vec3 v, float gain_multiplier, float pitch_multiplier);

SoundID play_2d_sound(int soundfile_id);
SoundID play_3d_sound(int soundfile_id, const Vec3& p);
SoundID play_3d_sound(int soundfile_id, Vec3 p, Vec3 v);

void stop_sound(SoundID sound_id);
bool is_active(SoundID sound_id);

void update_listener(const Vec3& p, const Vec3& v, const Vec3& f, const Vec3& u);
void update();

void set_pitch_multiplier(SoundID sound_id, float pitch);
void set_gain_multiplier(SoundID sound_id, float gain);

int test();

void enumerate_sound_devices();
#endif

#if DC_SERVER
void send_play_2d_sound(const char* name, ClientID client_id);
void broadcast_play_2d_sound(const char* name);
void send_play_3d_sound(const char* name, ClientID client_id, const Vec3& p);
void broadcast_play_3d_sound(const char* name, const Vec3& p);
void broadcast_exclude_play_3d_sound(const char* name, const Vec3& p, ClientID ignore_client_id);
#endif

}   // Sound
