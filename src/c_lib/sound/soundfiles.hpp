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
        id(-1), event_name(NULL), filename(NULL)
    {}

    ~Soundfile()
    {
        if (this->event_name != NULL) free(this->event_name);
        if (this->filename != NULL) free(this->filename);
    }
};

extern class Soundfile* soundfiles;
extern int n_sounds;

bool set_soundfile(int snd_id, const char* event_name, const char* filename);
void set_soundfile_properties(
    int snd_id,
    float pitch,
    float gain,
    float max_distance,
    float reference_distance,
    float minimum_gain,
    float maximum_gain,
    float rolloff_factor,
    bool loop,
    float max_playable_distance
);

int get_soundfile_id_for_name(const char* event_name);

void validate_sound_config();

void teardown_triggers();

}   // Sound
