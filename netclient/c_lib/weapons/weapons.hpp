#pragma once

namespace Weapons {

const int WEAPON_HUD_STRING_MAX = 31;

//typedef enum {
    //TYPE_hitscan_laser,
    //TYPE_block_pick,
    //TYPE_block_applier,
    //TYPE_grenade_thrower,
    //TYPE_spawner_placer,
    //TYPE_turret_placer
//} weapon_type;

//class Weapon {
    //protected:
        //char* clip_str;
        //char* clip_size_str;
        //char* ammo_str;
        //char* max_ammo_str;

    //public:
        //int max_ammo;
        //int speed;

        //int id;
        //int owner;
        //int type;

        //int ammo;

        //bool scope;

        //char* hud_string;

        //void add_ammo(int n);
        //void restore_ammo();

        //explicit Weapon(weapon_type type);
        //~Weapon();
//};

//class HitscanLaser: public Weapon {        
    //public:
        //int clip_size;
        //int clip;

        //bool fire();
        //void reload();
        //void restore_ammo();
        //char* hud_display();

        //HitscanLaser();
        //~HitscanLaser();
//};

//class BlockApplier: public Weapon {
    //public:
        //int block;

        //void set_block(int id);
        //bool can_fire();
        //bool fire();
        //char* hud_display();

        //BlockApplier();
        //~BlockApplier();
//};

//class BlockPick: public Weapon {

    //public:
        //bool fire();

        //char* hud_display();
        //BlockPick();
        //~BlockPick();
//};

//class GrenadeThrower: public Weapon {

    //public:
        //bool fire();

        //char* hud_display();
        //GrenadeThrower();
        //~GrenadeThrower();
//};

//// TODO -- one weapon to synthesize items with
//// TEMP
//class SpawnerPlacer: public Weapon
//{
    //public:
        //bool fire();

        //char* hud_display();
        //SpawnerPlacer();
        //~SpawnerPlacer();
//};

//class TurretPlacer: public Weapon
//{
    //public:
        //bool fire();

        //char* hud_display();
        //TurretPlacer();
        //~TurretPlacer();
//};

}
