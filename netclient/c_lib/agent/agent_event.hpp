#pragma once

#ifdef DC_CLIENT

#include <c_lib/particles/billboard_text.hpp>

class Agent_state;

class Agent_event {
    private:
        Agent_state* a;
    public:

        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void died();
        void born();
        void crouched();
        void uncrouched();
        void life_changing(bool dead);
        void reload_weapon(int type);

        BillboardText* bb;
        void display_name();
        void hide_name();

        void joined_team(int team);
        void name_changed();
        
        void picked_up_flag();
        void dropped_flag();
        void scored_flag();

        void fired_weapon_at_object(int id, int type, int part);
        void fired_weapon_at_block(float x, float y, float z, int cube, int side);
        void fired_weapon_at_nothing();
        void melee_attack_object(int id, int type, int part, float x, float y, float z);
        void melee_attack_nothing();

        void hit_block();
        void placed_block();
        void threw_grenade();

        void coins_changed(unsigned int coins);

        Agent_event(Agent_state* owner);
        ~Agent_event();
};

#endif
