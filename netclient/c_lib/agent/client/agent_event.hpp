#pragma once

#include <c_lib/particles/billboard_text_hud.hpp>

class Agent_state;

class Agent_event {
    private:
        Agent_state* a;
        unsigned char r,g,b;  // team colors
        bool first_time_receiving_coins;
    public:

        class Particles::BillboardTextHud* bb;
        void display_name();
        void hide_name();

        // side effects of taking damage. dont modify health/death here
        void took_damage(int dmg);
        void healed(int health);
        void died();
        void born();
        void crouched();
        void uncrouched();
        void life_changing(bool dead);
        void reload_weapon(int type);

        void joined_team(int team);
        void name_changed(char* old_name);
        void update_team_color(unsigned char r, unsigned char b, unsigned char c);
        void update_team_color();
        
        void picked_up_flag();
        void dropped_flag();
        void scored_flag();

        void fired_weapon_at_object(int id, int type, int part);
        void fired_weapon_at_block(float x, float y, float z, int cube, int side);
        void fired_weapon_at_nothing();
        void melee_attack_object(int id, int type, int part);
        void melee_attack_nothing();
        void fire_empty_weapon(int weapon_type);

        void hit_block();
        void placed_block();
        void threw_grenade();

        void set_spawner(int pt);
        void coins_changed(unsigned int coins);

        explicit Agent_event(Agent_state* owner);
        ~Agent_event();
};
