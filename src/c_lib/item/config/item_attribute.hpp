#pragma once

#include <item/common/constants.hpp>
#include <t_map/common/constants.hpp>
#include <common/dat/properties.hpp>
#include <common/dat/modifiers.hpp>

namespace Item
{

class ItemAttribute: public Property<ItemType>
{
    public:
        ItemGroup group;

        // All items
        int max_durability;
        int max_stack_size;
        int sprite;

        //IG_PLACER
        CubeType cube_type;   // id of block type that it creates

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

        int object_damage;      // if this is nonzero it will be used, otherwise randrange will be used
        int object_damage_min;  // for randrange(min,max);
        int object_damage_max;

        // particle rendering
        bool particle_voxel;
        int particle_voxel_texture;

        bool gas;
        int gas_lifetime;

        bool fuel;
        int fuel_burn_rate; // in ticks

        ItemContainerType container_type;

        int cube_height;    // if the item occupies space, the height it occupies in cubes

        // animation
        AnimationType animation_type;

        // charges
        int max_charges;
        int recharge_rate;

        // IG_EQUIPMENT
        EquipmentType equipment_type;

        // boon crank
        bool boonable;

        // agent stat modifiers
        ModifierList modifiers;

    ItemAttribute() :
        Property<ItemType>(NULL_ITEM_TYPE),
        modifiers(MAX_ITEM_MODIFIERS)
    {
        this->group = IG_NONE;
        this->cube_type = NULL_CUBE;
        this->mech_type = NULL_MECH_TYPE;

        this->max_durability = NULL_DURABILITY;
        this->max_stack_size = 1;
        this->sprite = ERROR_SPRITE;
        this->particle_voxel = false;
        this->particle_voxel_texture = 0;
        this->firing_range = DEFAULT_FIRING_RANGE;
        this->firing_rate = 15;
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

        this->object_damage = 0;
        this->object_damage_min = 5;
        this->object_damage_max = 10;
        this->container_type = NULL_CONTAINER_TYPE;
        this->gas = false;
        this->gas_lifetime = NULL_GAS_LIFETIME;
        this->fuel = false;
        this->fuel_burn_rate = 30;
        this->cube_height = 0;
        this->animation_type = NULL_ANIMATION_TYPE;

        this->max_charges = NULL_CHARGES;
        this->recharge_rate = 1;

        this->equipment_type = NULL_EQUIPMENT_TYPE;
        this->boonable = true;
    }
};


class ItemAttributes: public Properties<ItemAttribute, ItemType>
{
    public:

    ItemAttributes():
        Properties<ItemAttribute, ItemType>(MAX_ITEM_TYPES)
    {}
};

class SynthesizerItem
{
    public:

        ItemType item_type;
        int cost;

        int xslot;
        int yslot;

    SynthesizerItem()
    {
        this->item_type = NULL_ITEM_TYPE;
        this->cost = NULL_COST;

        this->xslot = NULL_SLOT;
        this->yslot = NULL_SLOT;
    }
};


class CraftingRecipe
{
    public:

        int id;
        ItemType output;
        int output_stack;
        int reagent_num;
        ItemType reagent[CRAFT_BENCH_INPUTS_MAX];
        int reagent_count[CRAFT_BENCH_INPUTS_MAX];

        // temporary state information
        bool available;

    CraftingRecipe()
    {
        init();
    }

    void init()
    {
        this->id = NULL_CRAFTING_RECIPE;
        this->output = NULL_ITEM_TYPE;
        this->output_stack = 1;
        this->reagent_num = 0;
        for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; i++)
        {
            this->reagent[i] = NULL_ITEM_TYPE;
            this->reagent_count[i] = 1;
        }
        this->available = true;
    }
};


class SmeltingRecipe
{
    public:

        int id;
        int output_num;
        ItemType output[SMELTER_OUTPUTS_MAX]; //item type
        int output_stack[SMELTER_OUTPUTS_MAX];
        int reagent_num;
        ItemType reagent[SMELTER_INPUTS_MAX];
        int reagent_count[SMELTER_INPUTS_MAX];
        int creation_time;  // total ticks to synthesize

        bool available;

    SmeltingRecipe()
    {
        this->init();
    }

    void init()
    {
        this->id = NULL_SMELTING_RECIPE;
        for (int i=0; i<SMELTER_OUTPUTS_MAX; this->output[i++] = NULL_ITEM_TYPE);
        for (int i=0; i<SMELTER_OUTPUTS_MAX; this->output_stack[i++] = 1);
        this->output_num = 0;
        this->reagent_num = 0;
        for (int i=0; i<SMELTER_INPUTS_MAX; i++)
        {
            this->reagent[i] = NULL_ITEM_TYPE;
            this->reagent_count[i] = 1;
        }
        this->available = true;
        this->creation_time = DEFAULT_SMELTING_RECIPE_CREATION_TIME;
    }
};

}   // Item
