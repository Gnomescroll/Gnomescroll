#pragma once

#define AGENT_HEALTH 100

class Agent_state;  // forward declaration
class PlayerAgent_state;

class Base_status {
    private:
    public:
        int health;
        bool dead;
        int apply_damage(int dmg);

        Base_status() : health(AGENT_HEALTH), dead(false) {}
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
};

// Use for:
// Client side PlayerAgent
class PlayerAgent_status: public Base_status {

    private:
        PlayerAgent_state* a;

    public:
        PlayerAgent_status(PlayerAgent_state* agent) : Base_status(), a(agent) {}
};
