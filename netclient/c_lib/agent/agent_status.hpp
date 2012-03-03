#pragma once

#define AGENT_HEALTH 100
#define RESPAWN_TICKS (2000 / 30)

#include <common/enum_types.hpp>

const unsigned int PLAYER_NAME_MAX_LENGTH = 24;

typedef enum AgentDeathMethod
{
    DEATH_NORMAL = 0,
    DEATH_HEADSHOT,
    DEATH_GRENADE,
    DEATH_FALL,
    DEATH_BELOW_MAP
} AgentDeathMethod;

class Agent_state;  // forward declaration

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
//class Agent_status: public Base_status {
class Agent_status {

    private:
        Agent_state* a;
        int voxel_model_restore_throttle;

    public:

        int health;
        bool dead;
        int respawn_countdown;
        int spawner;

        unsigned int kills;
        unsigned int deaths;
        unsigned int suicides;

        unsigned int health_max;

        char name[PLAYER_NAME_MAX_LENGTH + 1];
        bool identified;

        int team;
        bool has_flag;
        unsigned int flag_captures;

        unsigned int coins;

        bool vox_crouched;

        bool set_name(char* n); // return true if the new name is different
        void check_missing_name();
        
        void set_spawner(int pt);
        void set_spawner();
    
        int apply_damage(int dmg);
        int apply_damage(int dmg, int inflictor_id, Object_types inflictor_type, int part_id=-1);
        int apply_hitscan_laser_damage_to_part(int part_id, int inflictor_id, Object_types inflictor_type);
        int die();
        int die(int inflictor_id, Object_types inflictor_type, AgentDeathMethod death_method);
        void kill(int victim_id);
        void respawn();
        void restore_health();
        
        bool pickup_flag();
        bool drop_flag();
        void score_flag();

        int score();
        void send_scores(int client_id);
        void send_scores();

        void set_team(int team);

        float get_spawn_angle();
        void check_if_at_base();

        // coin stuff
        bool can_afford(Object_types obj);
        bool can_afford(unsigned int coins);
        void add_coins(unsigned int coins);
        void spend_coins(unsigned int coins);
        void send_coin_packet();
        bool purchase(Object_types obj);

        Agent_status(Agent_state* a);
};
