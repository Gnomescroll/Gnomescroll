#pragma once

#define AGENT_HEALTH 100
#define RESPAWN_TICKS (2000 / 30)

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
        int score() {
            return kills - suicides;
        }

        char name[PLAYER_NAME_MAX_LENGTH + 1];

        int team;

        void set_name(char* n) {
            int i;
            for (i=0; i<PLAYER_NAME_MAX_LENGTH && n[i] != '\0'; i++) {
                name[i] = n[i];
            }
            name[i] = '\0';
        }
        
        Base_status() :
            health(AGENT_HEALTH),
            dead(false),
            respawn_countdown(RESPAWN_TICKS),
            kills(0),
            deaths(0),
            team(0)
        {}
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

        int apply_damage(int dmg, int inflictor_id);
        int die();
        int die(int inflictor_id);
        void kill(int victim_id);
        void respawn();

};

// Use for:
// Client side PlayerAgent
class PlayerAgent_status: public Base_status {

    private:
        PlayerAgent_state* a;

    public:
        PlayerAgent_status(PlayerAgent_state* agent) : Base_status(), a(agent) {}
};
