/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Sound
{

class Soundfile
{
    public:
        int id;
        char* event_name;
        char* filename;
        // properties
        float pitch;
        float gain;
        float max_distance;
        float reference_distance;
        float minimum_gain;
        float maximum_gain;
        float rolloff_factor;
        bool loop;
        float max_playable_distance;

    Soundfile() :
        id(-1), event_name(NULL), filename(NULL),
        pitch(1.0f), gain(1.0f), max_distance(128.0f),
        reference_distance(5.0f), minimum_gain(0.0f), maximum_gain(1.0f),
        rolloff_factor(10.0f), loop(false), max_playable_distance(128.0f)
    {}

    ~Soundfile()
    {
        free(this->event_name);
        free(this->filename);
    }
};

extern class Soundfile* soundfiles;
extern int n_sounds;

bool set_soundfile(int snd_id, const char* event_name, const char* filename);
void set_soundfile_properties(int snd_id,
                              float pitch,
                              float gain,
                              float max_distance,
                              float reference_distance,
                              float minimum_gain,
                              float maximum_gain,
                              float rolloff_factor,
                              bool loop,
                              float max_playable_distance);

int get_soundfile_id_for_name(const char* event_name);
void validate_sound_config();
void teardown_triggers();

}   // Sound
