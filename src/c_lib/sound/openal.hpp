#pragma once

#if DC_SERVER
# error Don't include this file in the server
#endif

namespace OpenALSound
{

using Sound::Soundfile;

void init();
void close();

void set_volume(float vol);

void load_sound(Soundfile* snd);

SoundID play_2d_sound(const char* event_name);
SoundID play_2d_sound(const char* event_name, float gain_multiplier,
                      float pitch_multiplier);

SoundID play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v);
SoundID play_3d_sound(const char* event_name, const Vec3& p, const Vec3& v,
                      float gain_multiplier, float pitch_multiplier);

SoundID play_2d_sound(int soundfile_id);
SoundID play_3d_sound(int soundfile_id, const Vec3& p, const Vec3& v);

void stop_sound(SoundID sound_id);

bool is_active(SoundID sound_id);

void update_listener(const Vec3& p, const Vec3& v, const Vec3& f, const Vec3& u);
void update();

int test();

void set_pitch_multiplier(SoundID sound_id, float pitch);
void set_gain_multiplier(SoundID sound_id, float gain);

const char* enumerate_devices();

}   // OpenALSound
