#pragma once

namespace Item
{


class ItemAttribute
{
    public:

    int item_group_type;
    
    //IG_PLACER
    //char* placer_block_type;  //type of block that it creates
    int placer_block_type_id;   //id of block type that it creates

    //IG_HITSCAN_WEAPON
    int hitscan_fire_cooldown;      // ms per bullet
    int hitscan_damage;             // damage
    int hitscan_max_ammo;           // max ammo
    int hitscan_bullet_effect_enum; // bullet effect

    //IG_MELEE_WEAPON
    int melee_fire_cooldown;
    int melee_damage;

    //IG_MINING_LASER
    int mining_fire_cooldown;
    int mining_damage;

    // weapons
    float firing_range;
    int firing_rate;
    int block_damage;

    int object_damage;       // if this is nonzero it will be used, otherwise randrange will be used
    int object_damage_min;  // for randrange(min,max);
    int object_damage_max;
    
    
    // All items
    int max_energy;
    int max_durability;
    int max_stack_size;

    // particle rendering
    bool particle_voxel;
    int particle_voxel_texture;
    
    // nanite
    bool nanite_food;

    ItemAttribute()
    // DONT PUT ANYTHING HERE OR BREAKS
    {
        load_defaults(IG_ERROR);
    }

    void load_defaults(int group_type)
    {
        // PUT ALL DEFAULTS HERE
        item_group_type = group_type;
        //placer_block_type = NULL;
        max_energy = NULL_ENERGY;
        max_durability = NULL_DURABILITY;
        max_stack_size = 1;
        particle_voxel = false;
        particle_voxel_texture = 0;
        nanite_food = false;
        firing_range = NULL_FIRING_RANGE;
        firing_rate = 1;
        block_damage = 0;
        object_damage = 0;
        object_damage_min = 0;
        object_damage_max = 1;
    }
};

}
