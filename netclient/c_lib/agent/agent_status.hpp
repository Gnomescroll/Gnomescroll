#pragma once

class Agent_state;  // forward declaration
class PlayerAgent_state;

class Base_status {
    private:
    public:
        int health;
        
};

// Use for:
// All agents server side
// All non-player agents client side.
// In client side, ignore properties such as health
class Agent_status: public Base_status {

    private:
        Agent_state* a;

    public:
        Agent_status(Agent_state* agent) : a(agent) {}
};

// Use for:
// Client side PlayerAgent
class PlayerAgent_status: public Base_status {

    private:
        PlayerAgent_state* a;

    public:
        PlayerAgent_status(PlayerAgent_state* agent) : a(agent) {}
};
