#pragma once

namespace Weapons {

typedef enum {
    TYPE_block_pick,
    TYPE_hitscan_laser,
    TYPE_block_applier,
    TYPE_grenade_thrower,
    TYPE_spawner_placer
} weapon_type;

class Weapon {

    public:
        int max_ammo;
        int speed;

        int id;
        int owner;
        int type;

        int ammo;

        bool scope;

        char hud_string[20+1];

        void restore_ammo();

        Weapon(weapon_type type);
};

class HitscanLaser: public Weapon {

    public:
        int clip_size;
        int clip;

        bool fire();
        void reload();
        void restore_ammo();
        char* hud_display();

        HitscanLaser();
};

class BlockApplier: public Weapon {

    public:
        int block;
        
        bool fire();
        char* hud_display();

        BlockApplier();
};

class BlockPick: public Weapon {

    public:
        bool fire();

        char* hud_display();
        BlockPick();
};

class GrenadeThrower: public Weapon {

    public:
        bool fire();

        char* hud_display();
        GrenadeThrower();
};


class SpawnerPlacer: public Weapon
{
    public:
        bool fire();

        char* hud_display();
        SpawnerPlacer();
};

}
