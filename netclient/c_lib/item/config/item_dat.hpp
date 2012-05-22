#pragma once

#include <item/config/_interface.hpp>

#include <t_map/t_properties.hpp>

/*
typedef enum
{
    IG_ERROR,
    IG_RESOURCE,    //does nothing, resources, stackable
    IG_PLACER,  //consumed to create block
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,
    IG_GRENADE_LAUNCHER

} ItemGroups;
*/

/*
    int item_group_type;
    
    //IG_PLACER
    char* placer_block_type;    //type of block that it creates

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
    int mining_block_damage;
*/


namespace Item
{

void load_item_dat()
{
    start_item_dat();
    int i0 = texture_alias("media/sprites/i00.png");
    int i1 = texture_alias("media/sprites/i01.png");

    item_def(0, IG_ERROR, "error_item");
    sprite_def(i0, 4,1);

    item_def(1, IG_PLACER, "regolith");
    iso_block_sprite_def("regolith");
    s.placer_block_type_id = t_map::dat_get_cube_id("regolith");
    s.max_stack_size = 50;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"regolith");

    item_def(4, IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);

    item_def(5, IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i0, 3,7);
    s.hitscan_fire_cooldown = 30;
    s.hitscan_damage = 5;
    s.hitscan_max_ammo = 30;
    s.hitscan_bullet_effect_enum = 0;
    s.max_durability = 100;
    s.max_energy = 50;
    s.max_stack_size = 1;

    item_def(7, IG_MINING_LASER, "mining_laser");
    sprite_def(i0, 3,5);
    s.mining_fire_cooldown = 200;
    s.mining_damage = 1;
    s.mining_block_damage = 3;
    s.max_durability = 200;
    s.max_energy = 50;
    s.max_stack_size = 1;

    item_def(8, IG_GRENADE_LAUNCHER, "grenade_launcher");
    sprite_def(i0, 1,7);
    s.max_durability = 200;
    s.max_energy = 100;
    s.max_stack_size = 1;

    item_def(9, IG_RESOURCE, "food_rock-0");
    sprite_def(i1, 1,4);
    s.max_stack_size = 8;
    s.nanite_food = true;

    item_def(10, IG_RESOURCE, "blue_cystal");
    sprite_def(i0, 1,4);
    s.max_stack_size = 8;

    item_def(11, IG_NANITE_COIN, "nanite_coin");
    sprite_def(i1, 2, 7);
    s.max_stack_size = 99;

    item_def(12, IG_DEBUG, "location_pointer");
    sprite_def(i0, 4,2);

    item_def(13, IG_DEBUG, "block_placer");
    sprite_def(i0, 4,5);

    item_def(14, IG_UNKNOWN, "unknown");
    sprite_def(i0, 5,1);

    //item_def(15, IG_PLACER, "crate_1");
    //iso_block_sprite_def("crate_1");
    //s.placer_block_type_id = t_map::dat_get_cube_id("crate_1");
    //s.max_stack_size = 1;

    item_def(16, IG_PLACER, "crate_2");
    iso_block_sprite_def("crate_2");
    s.placer_block_type_id = t_map::dat_get_cube_id("crate_2");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"crate_2");

    item_def(17, IG_PLACER, "crate_3");
    iso_block_sprite_def("crate_3");
    s.placer_block_type_id = t_map::dat_get_cube_id("crate_3");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"crate_3");


/*
    Copper
*/
    item_def(32, IG_RESOURCE, "copper_ore");
    sprite_def(i1, 3,3);
    s.max_stack_size = 64;

    item_def(33, IG_RESOURCE, "copper_bar");
    sprite_def(i1, 3,2);
    s.max_stack_size = 64;

    item_def(34, IG_MELEE_WEAPON, "copper_shovel");
    sprite_def(i1, 3,1);
    s.melee_fire_cooldown = 250;
    s.melee_damage = 2;
    s.max_durability = 400;
    s.max_stack_size = 1;


/*
    Gallium
*/

    item_def(35, IG_RESOURCE, "gallium_ore");
    sprite_def(i1, 2,3);
    s.max_stack_size = 64;

    item_def(36, IG_RESOURCE, "gallium_bar");
    sprite_def(i1, 2,2);
    s.max_stack_size = 64;

    item_def(37, IG_MELEE_WEAPON, "gallium_shovel");
    sprite_def(i1, 2,1);
    s.melee_fire_cooldown = 250;
    s.melee_damage = 4;
    s.max_durability = 400;
    s.max_stack_size = 1;

/*
    Iron
*/
    item_def(38, IG_RESOURCE, "iron_ore");
    sprite_def(i1, 1,3);
    s.max_stack_size = 64;

    item_def(39, IG_RESOURCE, "iron_bar");
    sprite_def(i1, 1,2);
    s.max_stack_size = 64;

    item_def(40, IG_MELEE_WEAPON, "iron_shovel");
    sprite_def(i1, 1,1);
    s.melee_fire_cooldown = 250;
    s.melee_damage = 6;
    s.max_durability = 400;
    s.max_stack_size = 1;


/*
iridium
*/
    item_def(41, IG_RESOURCE, "iridium_ore");
    sprite_def(i1, 4,3);
    s.max_stack_size = 64;

    item_def(42, IG_RESOURCE, "iridium_bar");
    sprite_def(i1, 4,2);
    s.max_stack_size = 64;

    item_def(43, IG_MELEE_WEAPON, "iridium_shovel");
    sprite_def(i1, 4,1);
    s.melee_fire_cooldown = 250;
    s.melee_damage = 30;
    s.max_durability = 400;
    s.max_stack_size = 1;

    end_item_dat();
}


int _item_cube_iso_spritesheet_id = -1;

void start_item_dat()
{
#if DC_CLIENT
    _item_cube_iso_spritesheet_id = LUA_load_item_texture(t_map::block_item_16_surface);
#endif
}

void end_item_dat()
{
    _set_attribute();
    
    #if DC_CLIENT
    LUA_save_item_texture();
    #endif
}

void iso_block_sprite_def(const char* block_name)
{
#ifdef DC_CLIENT
    if(_item_cube_iso_spritesheet_id == -1)
    {
        printf("Error: iso_block_sprite_def, must call start_item_dat!!!\n");
        GS_ABORT();
    }

    int id = t_map::dat_get_cube_id(block_name);

    int xpos = id % 16;
    int ypos = id / 16;

    int index = LUA_blit_item_texture(_item_cube_iso_spritesheet_id, xpos, ypos);
    sprite_array[_current_item_id] = index; //check
#endif
}




}
