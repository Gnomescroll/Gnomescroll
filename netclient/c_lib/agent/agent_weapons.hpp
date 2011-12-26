#pragma once

#include <c_lib/weapons/weapons.hpp>

class Agent_state;  // forward declaration

class Agent_weapons {

    public:
        Agent_state* a;

        Weapons::HitscanLaser laser;
        Weapons::BlockApplier blocks;
        Weapons::BlockPick pick;
        Weapons::GrenadeThrower grenades;

        void set_clip(int type, int clip);
        void set_ammo(int type, int ammo);

        Agent_weapons(Agent_state* a);
};
