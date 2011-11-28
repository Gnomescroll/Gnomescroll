#pragma once

/*
 * All client agent input triggers should be implemented here
 */

class PlayerAgent_state;// forward declare

class PlayerAgent_action {
    private:
        PlayerAgent_state* p;
        
    public:
        PlayerAgent_action(PlayerAgent_state* player_agent): p(player_agent) {}
        
        //void left_mouse_click() {}
        //void right_mouse_click() {}

        void fire();
        void hit_block();
        
};
