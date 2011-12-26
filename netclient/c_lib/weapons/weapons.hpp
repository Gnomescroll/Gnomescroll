#pragma once

namespace Weapons {

typedef enum {
    TYPE_block_pick,
    TYPE_hitscan_laser,
    TYPE_block_applier,
    TYPE_grenade_thrower
} weapon_type;

const char hud_undefined_string[] = "--";
const char hud_display_format_string[] = "%s/%s::%s/%s";

class Weapon {

    public:
        int max_ammo;
        int speed;

        int id;
        int owner;
        int type;

        int ammo;

        char hud_string[20+1];

        Weapon(weapon_type type) :
        max_ammo(0), speed(1),
        id(0),
        owner(0),
        type(type),
        ammo(0)
        {}
};

class HitscanLaser: public Weapon {

    public:
        int clip_size;
        int clip;

        bool fire();
        void reload();

        char* hud_display() {
            char clip_str[4+1];
            sprintf(clip_str, "%d", clip);
            char clip_size_str[4+1];
            sprintf(clip_size_str, "%d", clip_size);
            char ammo_str[4+1];
            sprintf(ammo_str, "%d", ammo);
            char max_ammo_str[4+1];
            sprintf(max_ammo_str, "%d", max_ammo);
            
            sprintf(hud_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
            return hud_string;
        }


    HitscanLaser() :
    Weapon(TYPE_hitscan_laser)
    {
        // load these from dat later
        clip_size = 100;
        clip = 100;
        max_ammo = 100;
        ammo = 100;
    }
};

class BlockPick: public Weapon {

    public:
        bool fire();

        char* hud_display() {
            const char* clip_str = hud_undefined_string;
            const char* clip_size_str = hud_undefined_string;
            const char* ammo_str = hud_undefined_string;
            const char* max_ammo_str = hud_undefined_string;
            sprintf(hud_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
            return hud_string;
        }


    BlockPick() :
    Weapon(TYPE_block_pick)
    {}
};

class BlockApplier: public Weapon {

    public:
        int block;
        bool fire();

        char* hud_display() {
            const char* clip_str = hud_undefined_string;
            const char* clip_size_str = hud_undefined_string;
            char ammo_str[4+1];
            sprintf(ammo_str, "%d", ammo);
            char max_ammo_str[4+1];
            sprintf(max_ammo_str, "%d", max_ammo);
            
            sprintf(hud_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
            return hud_string;
        }


    BlockApplier() :
    Weapon(TYPE_block_applier)
    {
        max_ammo = 9999;
        ammo = 9999;
        block = 1;
    }
};

class GrenadeThrower: public Weapon {

    public:
        bool fire();

        char* hud_display() {
            const char* clip_str = hud_undefined_string;
            const char* clip_size_str = hud_undefined_string;
            char ammo_str[4+1];
            sprintf(ammo_str, "%d", ammo);
            char max_ammo_str[4+1];
            sprintf(max_ammo_str, "%d", max_ammo);
            
            sprintf(hud_string, clip_str, clip_size_str, ammo_str, max_ammo_str);
            return hud_string;
        }


    GrenadeThrower() :
    Weapon(TYPE_grenade_thrower)
    {
        max_ammo = 9999;
        ammo = 9999;
    }
};


}
