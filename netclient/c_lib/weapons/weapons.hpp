#pragma once

namespace Weapons {

typedef enum {
    TYPE_block_pick,
    TYPE_hitscan_laser,
    TYPE_block_applier,
    TYPE_grenade_thrower
} weapon_type;

class Weapon {

    public:
        int max_ammo;
        int speed;

        int id;
        int owner;
        int type;

        int ammo;

        Weapon(weapon_type type) :
        max_ammo(0), speed(1),
        id(0), owner(0),
        type(type),
        ammo(0)
        {}
};

class HitscanLaser: public Weapon {

    public:
        void fire();

    HitscanLaser() :
    Weapon(TYPE_hitscan_laser)
    {}
};

class BlockPick: public Weapon {

    public:
        void fire();

    BlockPick() :
    Weapon(TYPE_block_pick)
    {}
};

class BlockApplier: public Weapon {

    public:
        void fire();

    BlockApplier() :
    Weapon(TYPE_block_applier)
    {}
};

class GrenadeThrower: public Weapon {

    public:
        void fire();

    GrenadeThrower() :
    Weapon(TYPE_grenade_thrower)
    {}
};


}
