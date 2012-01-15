#pragma once

/*
 * All client agent input triggers should be implemented here
 */

class PlayerAgent_state;// forward declare

class PlayerAgent_action {
    private:
        PlayerAgent_state* p;
        
    public:
        PlayerAgent_action(PlayerAgent_state* player_agent)
        :
        p(player_agent)
        {}

        void fire();
        void hitscan();
        void hit_block();
        void set_block();
        void throw_grenade();

        void switch_weapon(int i);
        void reload();
        int select_block();
};
