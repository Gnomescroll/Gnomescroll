#pragma once

/*
 * All client agent input triggers should be implemented here
 */

class PlayerAgent_state;// forward declare

class PlayerAgent_action {
    private:
        PlayerAgent_state* p;
        
    public:

        // use for adjusted animation vector
        // firing vector is from camera position, but want to play animation
        // from a different location animation does not block viewport
        // but need this new vector to still point to target location
        // compute this vector whenever a htiscan target is acquired
        Vec3 target_direction;
    
        //void fire();
        void hitscan_laser();

        void tick_mining_laser();
        void fire_mining_laser();

        //void set_block();
        //bool switch_weapon(int i);
        //void reload();
        //int select_block();
        //void pickup_item(); // raycast to nearby item, add to inventory if found
        
        void throw_grenade();
        void place_spawner();
        void place_turret();

        Vec3 get_aiming_point();

        explicit PlayerAgent_action(PlayerAgent_state* player_agent);
};
