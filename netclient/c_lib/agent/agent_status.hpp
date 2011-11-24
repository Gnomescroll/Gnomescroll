#pragma once

class Agent_state;  // forward declaration

class Agent_status {
    private:
        Agent_state* a;
    public:
        int health;
        
        Agent_status(Agent_state* agent) : a(agent) {}
};
