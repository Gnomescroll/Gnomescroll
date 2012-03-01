#pragma once

namespace Weapons {

typedef enum {
    TYPE_block_pick,
    TYPE_hitscan_laser,
    TYPE_block_applier,
    TYPE_grenade_thrower,
    TYPE_spawner_placer
} weapon_type;

const int WEAPON_HUD_STRING_MAX = 22+1;
const char hud_undefined_string[] = "--";
const char hud_display_format_string[] = "%s/%s::%s/%s";

class Weapon {
    protected:
        char* clip_str;
        char* clip_size_str;
        char* ammo_str;
        char* max_ammo_str;

    public:
        int max_ammo;
        int speed;

        int id;
        int owner;
        int type;

        int ammo;

        bool scope;

        char* hud_string;

        void restore_ammo();

        Weapon(weapon_type type);
        ~Weapon();
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
        ~HitscanLaser();
};

class BlockApplier: public Weapon {
    public:
        int block;

        void set_block(int id);
        bool fire();
        char* hud_display();

        BlockApplier();
        ~BlockApplier();
};

class BlockPick: public Weapon {

    public:
        bool fire();

        char* hud_display();
        BlockPick();
        ~BlockPick();
};

class GrenadeThrower: public Weapon {

    public:
        bool fire();

        char* hud_display();
        GrenadeThrower();
        ~GrenadeThrower();
};


class SpawnerPlacer: public Weapon
{
    public:
        bool fire();

        char* hud_display();
        SpawnerPlacer();
        ~SpawnerPlacer();
};

}
