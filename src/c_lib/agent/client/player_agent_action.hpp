/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

/*
 * All client agent input triggers should be implemented here
 */

namespace Agents
{

class PlayerAgent;// forward declare

class PlayerAgentAction
{
    private:
        PlayerAgent* p;
        SoundID mining_laser_sound_id;

    public:

        // use for adjusted animation vector
        // firing vector is from camera position, but want to play animation
        // from a different location animation does not block viewport
        // but need this new vector to still point to target location
        // compute this vector whenever a htiscan target is acquired
        Vec3 target_direction;

    // mining laser
    void begin_mining_laser();
    void end_mining_laser();
    void update_mining_laser();

    void fire_weapon(ItemType weapon_type);

    bool set_block(ItemID placer_id);

    void throw_grenade();
    void place_spawner();
    void place_turret();

    Vec3 get_aiming_point();

    explicit PlayerAgentAction(PlayerAgent* player_agent);
};

}   // Agents
