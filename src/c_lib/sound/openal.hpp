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

int play_2d_sound(const char* event_name);
int play_2d_sound(const char* event_name, float gain_multiplier, float pitch_multiplier);

int play_3d_sound(const char* event_name, struct Vec3 p, struct Vec3 v);
int play_3d_sound(const char* event_name, struct Vec3 p, struct Vec3 v, float gain_multiplier, float pitch_multiplier);

int play_2d_sound(int soundfile_id);
int play_3d_sound(int soundfile_id, struct Vec3 p, struct Vec3 v);

void stop_sound(int sound_id);

void update_listener(const struct Vec3& p, const struct Vec3& v,
                     const struct Vec3& f, const struct Vec3& u);
void update();

int test();

void set_pitch_multiplier(int sound_id, float pitch);
void set_gain_multiplier(int sound_id, float gain);

const char* enumerate_devices();

}   // OpenALSound
