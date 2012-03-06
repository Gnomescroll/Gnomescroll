#pragma once

#include <c_lib/weapons/weapons.hpp>

class Agent_state;  // forward declaration

class Agent_weapons {

    private:
        void weapon_change_message();

    public:
        Agent_state* a;

        int active;

        Weapons::HitscanLaser laser;
        Weapons::BlockPick pick;
        Weapons::BlockApplier blocks;
        Weapons::GrenadeThrower grenades;
        Weapons::SpawnerPlacer spawner;

        void set_clip(int type, int clip);
        void set_ammo(int type, int ammo);
        void set_active(int slot);
        void switch_up();
        void switch_down();
        bool is_active(int type);
        int active_type();
        void set_active_block(int block);
        //void adjust_active();

        char* hud_display();

        bool fire();
        bool fire(int type);
        void reload();
        void reload(int type);

        void restore_ammo();

        bool can_zoom();

        explicit Agent_weapons(Agent_state* a);
};
