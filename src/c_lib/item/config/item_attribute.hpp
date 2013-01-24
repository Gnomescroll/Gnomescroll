#pragma once

#include <item/common/constants.hpp>
#include <t_map/common/constants.hpp>

namespace Item
{
    
class ItemAttribute
{
    public:
        bool loaded;

        int item_type;
        ItemGroup group;
        char name[DAT_NAME_MAX_LENGTH+1];
        char pretty_name[ITEM_PRETTY_NAME_MAX_LENGTH+1];

        // All items
        int max_durability;
        int max_stack_size;
        int sprite;

        //IG_PLACER
        CubeType cube_type;   //id of block type that it creates

        //IG_MECH_PLACER_PLAYER
        MechType mech_type;

        //IG_HITSCAN_WEAPON
        int hitscan_fire_cooldown;      // ms per bullet
        int hitscan_damage;             // damage
        int hitscan_bullet_effect_enum; // bullet effect

        //IG_MELEE_WEAPON
        int melee_fire_cooldown;
        int melee_damage;

        // weapons
        float firing_range;
        int firing_rate;
        int block_damage[MAX_CUBES];

        int object_damage;       // if this is nonzero it will be used, otherwise randrange will be used
        int object_damage_min;  // for randrange(min,max);
        int object_damage_max;

        // particle rendering
        bool particle_voxel;
        int particle_voxel_texture;

        // health
        unsigned int repair_agent_amount;

        bool gas;
        int gas_lifetime;
        
        bool fuel;
        int fuel_burn_rate; // in ticks

        ItemContainerType container_type;

        int cube_height;    // if the item occupies space, the height it occupies in cubes

        // animation
        int animation_id;

        // charges
        int max_charges;
        int recharge_rate;

    ItemAttribute() :
        loaded(false), item_type(NULL_ITEM_TYPE), group(IG_NONE)
    {
        this->load_defaults();
    }

    void load_defaults()
    {
        loaded = false;

        // PUT ALL DEFAULTS HERE
        memset(this->name, 0, sizeof(this->name));
        memset(this->pretty_name, 0, sizeof(this->pretty_name));
        cube_type = NULL_CUBE;
        mech_type = NULL_MECH_TYPE;

        max_durability = NULL_DURABILITY;
        max_stack_size = 1;
        sprite = ERROR_SPRITE;
        particle_voxel = false;
        particle_voxel_texture = 0;
        firing_range = DEFAULT_FIRING_RANGE;
        firing_rate = 6;
        // match defaults with fist
        for (int i=0; i<MAX_CUBES; i++)
        {
            if (!t_map::isInUse((CubeType)i)) continue;
            int dmg = 1;
            switch (t_map::get_cube_material((CubeType)i))
            {
                case CUBE_MATERIAL_DIRT:
                    dmg = 2;
                    break;
                case CUBE_MATERIAL_STONE:
                    dmg = 1;
                    break;
                case CUBE_MATERIAL_DECORATION:
                    dmg = 4;
                    break;
                case CUBE_MATERIAL_NONE:
                    dmg = 1;
                    break;
            }
            this->block_damage[i] = dmg;
        }
        
        object_damage = 0;
        object_damage_min = 5;
        object_damage_max = 10;
        repair_agent_amount = 0;
        container_type = NULL_CONTAINER_TYPE;
        gas = false;
        gas_lifetime = NULL_GAS_LIFETIME;
        fuel = false;
        fuel_burn_rate = 30;
        cube_height = 0;
        animation_id = 0;

        this->max_charges = NULL_CHARGES;
        this->recharge_rate = 1;
    }

    void set_type(int item_type, ItemGroup group)
    {
        this->group = group;
        this->item_type = item_type;
    }
};

}   // Item
