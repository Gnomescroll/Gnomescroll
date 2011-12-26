#include "agent_weapons.hpp"

Agent_weapons::Agent_weapons(Agent_state* a)
: a(a)
{}

void Agent_weapons::set_ammo(int type, int ammo) {

    switch (type) {
        case Weapons::TYPE_block_applier:
            blocks.ammo = ammo;
            break;
        //case Weapons::TYPE_block_pick:
            //break;
        case Weapons::TYPE_grenade_thrower:
            grenades.ammo = ammo;
            break;
        case Weapons::TYPE_hitscan_laser:
            laser.ammo = ammo;
            break;
        default:
            printf("Agent_weapons::set_ammo unknown or invalid weapon type %d\n", type);
            return;
    }

}

void Agent_weapons::set_clip(int type, int clip) {

    switch (type) {
        //case Weapons::TYPE_block_applier:
            //break;
        //case Weapons::TYPE_block_pick:
            //break;
        //case Weapons::TYPE_grenade_thrower:
            //break;
        case Weapons::TYPE_hitscan_laser:
            laser.clip = clip;
            break;
        default:
            printf("Agent_weapons::set_clip unknown or invalid weapon type %d\n", type);
            return;
    }

}
