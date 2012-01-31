#pragma once

#ifdef DC_CLIENT

#include <c_lib/particles/billboard_text.hpp>

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

        BillboardText* bb;
        void display_name();
        void hide_name();

        void joined_team(int team);
        void name_changed();
        
        void picked_up_flag();
        void dropped_flag();
        void scored_flag();

        void fired_weapon_at_object(int id, int type, int part, float x, float y, float z);
        void fired_weapon_at_block(float x, float y, float z, int cube, int side);
        void fired_weapon_at_nothing();

        void hit_block();
        void placed_block();
        void threw_grenade();

        void coins_changed(unsigned int coins);

        Agent_event(Agent_state* owner);
        ~Agent_event();
};

#endif
