#pragma once

#ifdef DC_CLIENT

class Agent_state;

class Agent_event {
    private:
        Agent_state* a;
    public:
        void fired_weapon(int type);
        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void died();
        void born();
        void life_changing(bool dead);
        void reload_weapon(int type);

        Agent_event(Agent_state* owner) : a(owner) {}
};

#endif
