#pragma once

#define AGENT_HEALTH 100
#define RESPAWN_TICKS (2000 / 30)

#include <common/enum_types.hpp>

const unsigned int PLAYER_NAME_MAX_LENGTH = 24;

class Agent_state;  // forward declaration

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
//class Agent_status: public Base_status {
class Agent_status {

    private:
        Agent_state* a;

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

        void set_name(char* n);

        void set_spawner(int pt);
        void set_spawner();
    
        int apply_damage(int dmg);
        int apply_damage(int dmg, int inflictor_id, Object_types inflictor_type);
        int die();
        int die(int inflictor_id, Object_types inflictor_type);
        void kill(int victim_id);
        void respawn();
        void restore_health();
        
        void pickup_flag();
        void drop_flag();
        void score_flag();

        int score();
        void send_scores(int client_id);
        void send_scores();

        void set_team(int team);

        float get_spawn_angle();

        // coin stuff
        bool can_afford(Object_types obj);
        bool can_afford(unsigned int coins);
        void add_coins(unsigned int coins);
        void spend_coins(unsigned int coins);
        void send_coin_packet();
        bool purchase(Object_types obj);

        Agent_status(Agent_state* a);
};
