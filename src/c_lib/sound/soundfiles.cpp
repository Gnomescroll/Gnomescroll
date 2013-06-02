#include "soundfiles.hpp"

#include <sound/sound.hpp>
#include <sound/csv_parser.hpp>

namespace Sound
{

class Soundfile* soundfiles = NULL;  // allocated by csv_parser
int n_sounds = 0;

static bool event_registered(const char* event_name)
{
    if (soundfiles == NULL)
        return NULL;
    for (int i=0; i<n_sounds; i++)
        if (!strcmp(event_name, soundfiles[i].event_name))
            return true;
    return false;
}

bool set_soundfile(int snd_id, const char* event_name, const char* filename)
{
    if (event_registered(event_name))
        return false;

    size_t event_name_len = strlen(event_name);
    size_t file_len = strlen(filename);
    class Soundfile* snd = &soundfiles[snd_id];
    snd->event_name = (char*)malloc(sizeof(char) * (event_name_len + 1));
    strcpy(snd->event_name, event_name);
    snd->filename = (char*)malloc(sizeof(char) * (file_len + 1));
    strcpy(snd->filename, filename);

    #if DC_CLIENT
    load_sound(snd);
    #endif

    return true;
}

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
    float max_playable_distance)
{
    if (snd_id < 0 || snd_id >= n_sounds)
    {
        printf("WARNING: set_soundfile_properties -- snd_id %d invalid (n_sounds=%d)\n", snd_id, n_sounds);
        return;
    }
    Soundfile* s = &soundfiles[snd_id];
    s->pitch = pitch;
    s->gain = gain;
    s->max_distance = max_distance;
    s->reference_distance = reference_distance;
    s->minimum_gain = minimum_gain;
    s->maximum_gain = maximum_gain;
    s->rolloff_factor = rolloff_factor;
    s->loop = loop;
    s->max_playable_distance = max_playable_distance;
}

void validate_sound_config()
{
    IF_ASSERT(soundfiles == NULL) return;
}

int get_soundfile_id_for_name(const char* event_name)
{
    IF_ASSERT(soundfiles == NULL) return -1;
    for (int i=0; i<n_sounds; i++)
        if (strcmp(soundfiles[i].event_name, event_name) == 0)
            return i;
    printf("WARNING: No soundfile found for event_name %s\n", event_name);
    return -1;
}

void teardown_triggers()
{
    delete[] soundfiles;
}

}   // Sound
