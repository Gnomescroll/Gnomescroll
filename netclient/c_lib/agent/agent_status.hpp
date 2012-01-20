#pragma once

#define AGENT_HEALTH 100
#define RESPAWN_TICKS (2000 / 30)

#include <common/enum_types.hpp>

const int PLAYER_NAME_MAX_LENGTH = 24;

class Agent_state;  // forward declaration
class PlayerAgent_state;

class Base_status {
    private:
    public:
        int health;
        bool dead;
        int respawn_countdown;

        unsigned int kills;
        unsigned int deaths;
        unsigned int suicides;

        unsigned int health_max;

        char name[PLAYER_NAME_MAX_LENGTH + 1];

        int team;
        bool has_flag;
        unsigned int flag_captures;

        void set_name(char* n);

        Base_status();
};

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
class Agent_status: public Base_status {

    private:
        Agent_state* a;

    public:
        Agent_status(Agent_state* agent) : Base_status(), a(agent) {}

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

};

// Use for:
// Client side PlayerAgent
class PlayerAgent_status: public Base_status {

    private:
        PlayerAgent_state* a;

    public:
        PlayerAgent_status(PlayerAgent_state* agent) : Base_status(), a(agent) {}
};
