#pragma once

#ifdef DC_CLIENT

class Agent_state;

class Agent_event {
    private:
        Agent_state* a;
    public:
        void fired_weapon(int type);
        void fired_laser();
        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void died();
        void born();
        void life_changing(bool dead);
        void reload_weapon(int type);

        void picked_up_flag();
        void dropped_flag();
        void scored_flag();

        Agent_event(Agent_state* owner) : a(owner) {}
};

#endif
