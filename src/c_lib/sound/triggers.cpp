#include "triggers.hpp"

#include <sound/sound.hpp>
#include <sound/csv_parser.hpp>

namespace Sound
{

#define SOUND_TRIGGER(NAME)\
void NAME()\
{\
    play_2d_sound((char*)#NAME);\
}\
void NAME(float x, float y, float z, float vx, float vy, float vz)\
{\
    play_3d_sound((char*)#NAME, x,y,z,vx,vy,vz);\
}

#define SOUND_LOOP_TRIGGER(NAME)\
int NAME(bool start, int source_id)\
{\
    if (start) return play_2d_sound((char*)#NAME);\
    else\
    {\
        stop_sound(source_id);\
        return -1;\
    }\
}\
int NAME(bool start, int source_id, float x, float y, float z, float vx, float vy, float vz)\
{\
    if (start) return play_3d_sound((char*)#NAME, x,y,z,vx,vy,vz);\
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
SOUND_TRIGGER(flag_picked_up)
SOUND_TRIGGER(flag_scored)
SOUND_TRIGGER(spawner_placed)
SOUND_TRIGGER(respawned)
SOUND_TRIGGER(died)
SOUND_TRIGGER(jetpack_on)
SOUND_TRIGGER(victory)
SOUND_TRIGGER(restore_health)
SOUND_TRIGGER(restore_ammo)
SOUND_TRIGGER(block_set)
SOUND_TRIGGER(grenade_explode)
SOUND_TRIGGER(grenade_bounce)
SOUND_TRIGGER(agent_took_damage)
SOUND_TRIGGER(you_joined_server)
SOUND_TRIGGER(player_joined_server)
SOUND_TRIGGER(player_left_server)
SOUND_TRIGGER(turret_shoot)
SOUND_TRIGGER(pickup_item)

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

/* function -> filename mapping */

struct Soundfile* sound_file_functions = NULL;  // allocated by csv_parser
int n_sounds = 0;

static bool function_registered(char *fn)
{
    if (sound_file_functions == NULL)
        return NULL;
    for (int i=0; i<n_sounds; i++)
        if (!strcmp(fn, sound_file_functions[i].fn))
            return true;
    return false;
}

bool set_soundfile(int snd_id, char* fn, char* file)
{
    // check if function mapped already
    if (function_registered(fn))
        return false;
    
    int fn_len = strlen(fn);
    int file_len = strlen(file);
    struct Soundfile* snd = &sound_file_functions[snd_id];
    snd->fn = (char*)malloc(sizeof(char) * (fn_len + 1));
    strcpy(snd->fn, fn);
    snd->file = (char*)malloc(sizeof(char) * (file_len + 1));
    strcpy(snd->file, file);

    load_sound(snd);

    //printf("Set sound trigger: %s -> %s\n", fn, file);

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
    bool loop
)
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
    //printf("set properties: ");
    //printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", pitch, gain, max_distance, reference_distance, minimum_gain, maximum_gain);
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

}
