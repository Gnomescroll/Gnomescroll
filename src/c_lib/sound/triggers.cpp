#include "triggers.hpp"

#include <sound/sound.hpp>
#include <sound/csv_parser.hpp>

namespace Sound
{

#if DC_CLIENT
#define SOUND_TRIGGER(NAME)\
int NAME()\
{\
    return play_2d_sound(#NAME);\
}\
int NAME(float x, float y, float z, float vx, float vy, float vz)\
{\
    return play_3d_sound(#NAME, x,y,z,vx,vy,vz);\
}\
int NAME(struct Vec3 p)\
{\
    return play_3d_sound(#NAME, p);\
}\
int NAME(struct Vec3 p, struct Vec3 v)\
{\
    return play_3d_sound(#NAME, p, v);\
}

#define SOUND_LOOP_TRIGGER(NAME)\
int NAME(bool start, int source_id)\
{\
    if (start) return play_2d_sound(#NAME);\
    else\
    {\
        stop_sound(source_id);\
        return -1;\
    }\
}\
int NAME(bool start, int source_id, float x, float y, float z, float vx, float vy, float vz)\
{\
    if (start) return play_3d_sound(#NAME, x,y,z,vx,vy,vz);\
    else\
    {\
        stop_sound(source_id);\
        return -1;\
    }\
}

/* triggers */

SOUND_TRIGGER(fire_laser)
SOUND_TRIGGER(out_of_ammo)
SOUND_TRIGGER(reload)
SOUND_TRIGGER(laser_hit_block)
SOUND_TRIGGER(laser_hit_agent)
SOUND_TRIGGER(pick_hit_block)
SOUND_TRIGGER(pick_hit_agent)
SOUND_TRIGGER(pick_swung)
SOUND_TRIGGER(block_destroyed)
SOUND_TRIGGER(block_took_damage)
SOUND_TRIGGER(spawner_placed)
SOUND_TRIGGER(respawned)
SOUND_TRIGGER(died)
SOUND_TRIGGER(victory)
SOUND_TRIGGER(restore_health)
SOUND_TRIGGER(restore_ammo)
SOUND_TRIGGER(block_set)
SOUND_TRIGGER(plasma_grenade_explode)
SOUND_TRIGGER(grenade_bounce)
SOUND_TRIGGER(agent_took_damage)
SOUND_TRIGGER(you_joined_server)
SOUND_TRIGGER(player_joined_server)
SOUND_TRIGGER(player_left_server)
SOUND_TRIGGER(turret_shoot)
SOUND_TRIGGER(pickup_item)
SOUND_TRIGGER(smelter_on)

// motion
SOUND_TRIGGER(soft_step_1)
SOUND_TRIGGER(soft_step_2)
SOUND_TRIGGER(soft_step_3)
SOUND_TRIGGER(soft_step_4)
SOUND_TRIGGER(hard_step_1)
SOUND_TRIGGER(hard_step_2)
SOUND_TRIGGER(hard_step_3)
SOUND_TRIGGER(hard_step_4)
SOUND_TRIGGER(soft_landing_1)
SOUND_TRIGGER(soft_landing_2)
SOUND_TRIGGER(soft_landing_3)
SOUND_TRIGGER(soft_landing_4)
SOUND_TRIGGER(hard_landing_1)
SOUND_TRIGGER(hard_landing_2)
SOUND_TRIGGER(hard_landing_3)
SOUND_TRIGGER(hard_landing_4)

// on/off loops
SOUND_LOOP_TRIGGER(mining_laser);
SOUND_LOOP_TRIGGER(jetpack_loop);

#endif

/* function -> filename mapping */

class Soundfile* sound_file_functions = NULL;  // allocated by csv_parser
int n_sounds = 0;

static bool function_registered(const char *fn)
{
    if (sound_file_functions == NULL)
        return NULL;
    for (int i=0; i<n_sounds; i++)
        if (!strcmp(fn, sound_file_functions[i].fn))
            return true;
    return false;
}

bool set_soundfile(int snd_id, const char* fn, const char* file)
{
    // check if function mapped already
    if (function_registered(fn))
        return false;

    int fn_len = (int)strlen(fn);
    int file_len = (int)strlen(file);
    class Soundfile* snd = &sound_file_functions[snd_id];
    snd->fn = (char*)malloc(sizeof(char) * (fn_len + 1));
    strcpy(snd->fn, fn);
    snd->hash = strhash(fn);
    snd->file = (char*)malloc(sizeof(char) * (file_len + 1));
    strcpy(snd->file, file);

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
    Soundfile* s = &sound_file_functions[snd_id];
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
    GS_ASSERT(sound_file_functions != NULL);
    if (sound_file_functions == NULL) return;
    for (int i=0; i<n_sounds; i++)
    {
        unsigned int hash = sound_file_functions[i].hash;
        char* fn = sound_file_functions[i].fn;
        for (int j=0; j<n_sounds; j++)
        {
            if (i==j) continue;
            GS_ASSERT(hash != sound_file_functions[j].hash || strcmp(fn, sound_file_functions[j].fn) == 0);
        }
    }
}

int get_soundfile_id_for_name(const char* name)
{
    GS_ASSERT(sound_file_functions != NULL);
    if (sound_file_functions == NULL) return -1;

    unsigned int hash = strhash(name);
    for (int i=0; i<n_sounds; i++)
        if (hash == sound_file_functions[i].hash)
            return i;
    return -1;
}

void teardown_triggers()
{
    if (sound_file_functions == NULL)
        return;

    for (int i=0; i<n_sounds; i++)
    {
        free(sound_file_functions[i].fn);
        free(sound_file_functions[i].file);
    }
    free(sound_file_functions);
}

}   // Sound
