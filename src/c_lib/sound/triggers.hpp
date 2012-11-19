#pragma once

namespace Sound
{

#if DC_CLIENT

#define SOUND_TRIGGER_HEADER(NAME)\
void NAME();\
void NAME(float x, float y, float z, float vx, float vy, float vz);\
void NAME(struct Vec3 p);
void NAME(struct Vec3 p, struct Vec3 v);

#define SOUND_LOOP_TRIGGER_HEADER(NAME)\
int NAME(bool start, int source_id);\
int NAME(bool start, int source_id, float x, float y, float z, float vx, float vy, float vz);

SOUND_TRIGGER_HEADER(fire_laser)
SOUND_TRIGGER_HEADER(out_of_ammo)
SOUND_TRIGGER_HEADER(reload)
SOUND_TRIGGER_HEADER(laser_hit_block)
SOUND_TRIGGER_HEADER(laser_hit_agent)
SOUND_TRIGGER_HEADER(pick_hit_block)
SOUND_TRIGGER_HEADER(pick_hit_agent)
SOUND_TRIGGER_HEADER(pick_swung)
SOUND_TRIGGER_HEADER(block_destroyed)
SOUND_TRIGGER_HEADER(block_took_damage)
SOUND_TRIGGER_HEADER(spawner_placed)
SOUND_TRIGGER_HEADER(respawned)
SOUND_TRIGGER_HEADER(died)
SOUND_TRIGGER_HEADER(jetpack_on)
SOUND_TRIGGER_HEADER(victory)
SOUND_TRIGGER_HEADER(restore_health)
SOUND_TRIGGER_HEADER(restore_ammo)
SOUND_TRIGGER_HEADER(block_set)
SOUND_TRIGGER_HEADER(plasma_grenade_explode)
SOUND_TRIGGER_HEADER(grenade_bounce)
SOUND_TRIGGER_HEADER(agent_took_damage)
SOUND_TRIGGER_HEADER(you_joined_server)
SOUND_TRIGGER_HEADER(player_joined_server)
SOUND_TRIGGER_HEADER(player_left_server)
SOUND_TRIGGER_HEADER(turret_shoot)
SOUND_TRIGGER_HEADER(pickup_item)
SOUND_TRIGGER_HEADER(smelter_on)

// motion
SOUND_TRIGGER_HEADER(soft_step_1)
SOUND_TRIGGER_HEADER(soft_step_2)
SOUND_TRIGGER_HEADER(soft_step_3)
SOUND_TRIGGER_HEADER(soft_step_4)
SOUND_TRIGGER_HEADER(hard_step_1)
SOUND_TRIGGER_HEADER(hard_step_2)
SOUND_TRIGGER_HEADER(hard_step_3)
SOUND_TRIGGER_HEADER(hard_step_4)
SOUND_TRIGGER_HEADER(soft_landing_1)
SOUND_TRIGGER_HEADER(soft_landing_2)
SOUND_TRIGGER_HEADER(soft_landing_3)
SOUND_TRIGGER_HEADER(soft_landing_4)
SOUND_TRIGGER_HEADER(hard_landing_1)
SOUND_TRIGGER_HEADER(hard_landing_2)
SOUND_TRIGGER_HEADER(hard_landing_3)
SOUND_TRIGGER_HEADER(hard_landing_4)

// on/off loops
SOUND_LOOP_TRIGGER_HEADER(mining_laser);

#endif

class Soundfile
{
    public:
        unsigned int hash;  // hash of fn
        char* fn;
        char* file;
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

    Soundfile()
    : hash(0), fn(NULL), file(NULL)
    {}

    ~Soundfile()
    {
        free(this->fn);
        free(this->file);
    }
};

extern class Soundfile* sound_file_functions;
extern int n_sounds;

bool set_soundfile(int snd_id, const char* fn, const char* file);
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

int get_soundfile_id_for_name(const char* name);

void validate_sound_config();

void teardown_triggers();

}   // Sound
