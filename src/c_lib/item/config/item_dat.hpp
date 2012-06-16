#pragma once

#include <item/config/_interface.hpp>

#include <t_map/t_properties.hpp>

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
    s.pretty_name = (char*)"Regolith";
    s.max_stack_size = 64;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"regolith");

    item_def(4, IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);
    s.pretty_name = (char*)"Quartz Crystal";


    item_def(5, IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i0, 3,7);
    s.pretty_name = (char*)"Laser Rifle";
    s.hitscan_fire_cooldown = 30;
    s.hitscan_damage = 5;
    s.hitscan_max_ammo = 30;
    s.hitscan_bullet_effect_enum = 0;
    s.max_durability = 100;
    s.max_energy = 50;
    s.max_stack_size = 1;
    s.object_damage_min = 15;
    s.object_damage_max = 25;
    s.block_damage = 2;

    item_def(7, IG_MINING_LASER, "mining_laser");
    sprite_def(i0, 3,5);
    s.pretty_name = (char*)"Mining Laser";
    s.mining_fire_cooldown = 200;
    s.mining_damage = 1;
    s.max_durability = 200;
    s.max_energy = 50;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 5;
    s.block_damage = 6;
    s.object_damage_min = 25;
    s.object_damage_max = 50;

    item_def(8, IG_GRENADE_LAUNCHER, "grenade_launcher");
    sprite_def(i0, 1,7);
    s.pretty_name = (char*)"Plasma Grenade";
    s.max_stack_size = 100;

    item_def(9, IG_RESOURCE, "food_rock-0");
    sprite_def(i1, 1,4);
    s.pretty_name = (char*)"Small Rock";
    s.max_stack_size = 16;
    s.nanite_food = true;

    //item_def(10, IG_RESOURCE, "blue_crystal");
    //sprite_def(i0, 1,4);
    //s.pretty_name = (char*)"Blue Crystal";
    //s.max_stack_size = 8;

    item_def(11, IG_NANITE_COIN, "nanite_coin");
    sprite_def(i1, 2, 7);
    s.pretty_name = (char*)"Nanite Chip";
    s.max_stack_size = 99;

    item_def(12, IG_DEBUG, "location_pointer");
    sprite_def(i0, 4,2);
    s.pretty_name = (char*)"Location Pointer";

    item_def(13, IG_DEBUG, "block_placer");
    sprite_def(i0, 4,5);
    s.pretty_name = (char*)"Block Placer";

    item_def(14, IG_UNKNOWN, "unknown");
    sprite_def(i0, 5,1);
    s.pretty_name = (char*)"Unknown";

    //item_def(15, IG_PLACER, "crate_1");
    //iso_block_sprite_def("crate_1");
    //s.placer_block_type_id = t_map::dat_get_cube_id("crate_1");
    //s.max_stack_size = 1;

    // small storage container
    item_def(16, IG_PLACER, "crate_2");
    iso_block_sprite_def("crate_2");
    s.placer_block_type_id = t_map::dat_get_cube_id("crate_2");
    s.pretty_name = (char*)"Small Storage Block";
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"crate_2");
    s.container_type = CONTAINER_TYPE_STORAGE_BLOCK_SMALL;
    container_block_def("crate_2", CONTAINER_TYPE_STORAGE_BLOCK_SMALL);

    // level 1 utility crafting block
    item_def(17, IG_PLACER, "crate_3");
    iso_block_sprite_def("crate_3");
    s.pretty_name = (char*)"Small Crafting Bench";
    s.placer_block_type_id = t_map::dat_get_cube_id("crate_3");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"crate_3");
    s.container_type = CONTAINER_TYPE_CRAFTING_BENCH_UTILITY;
    container_block_def("crate_3", CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);

    // Copper
    item_def(32, IG_RESOURCE, "copper_ore");
    sprite_def(i1, 3,3);
    s.pretty_name = (char*)"Copper Ore";
    s.max_stack_size = 64;

    item_def(33, IG_RESOURCE, "copper_bar");
    sprite_def(i1, 3,2);
    s.pretty_name = (char*)"Copper Bar";
    s.max_stack_size = 64;

    item_def(34, IG_SHOVEL, "copper_shovel");
    sprite_def(i1, 3,1);
    s.pretty_name = (char*)"Copper Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 2;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 7;
    s.block_damage = 4;
    s.object_damage_min = 15;
    s.object_damage_max = 25;

    // Gallium
    item_def(35, IG_RESOURCE, "gallium_ore");
    sprite_def(i1, 2,3);
    s.pretty_name = (char*)"Gallium Ore";
    s.max_stack_size = 64;

    item_def(36, IG_RESOURCE, "gallium_bar");
    sprite_def(i1, 2,2);
    s.pretty_name = (char*)"Gallium Bar";
    s.max_stack_size = 64;

    item_def(37, IG_SHOVEL, "gallium_shovel");
    sprite_def(i1, 2,1);
    s.pretty_name = (char*)"Gallium Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 4;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 7;
    s.block_damage = 5;
    s.object_damage_min = 20;
    s.object_damage_max = 30;

    // Iron
    item_def(38, IG_RESOURCE, "iron_ore");
    sprite_def(i1, 1,3);
    s.pretty_name = (char*)"Iron Ore";
    s.max_stack_size = 64;

    item_def(39, IG_RESOURCE, "iron_bar");
    sprite_def(i1, 1,2);
    s.pretty_name = (char*)"Iron Bar";
    s.max_stack_size = 64;

    item_def(40, IG_SHOVEL, "iron_shovel");
    sprite_def(i1, 1,1);
    s.pretty_name = (char*)"Iron Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 6;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 7;
    s.block_damage = 6;
    s.object_damage_min = 25;
    s.object_damage_max = 35;

    // Iridium
    item_def(41, IG_RESOURCE, "iridium_ore");
    sprite_def(i1, 4,3);
    s.pretty_name = (char*)"Iridium Ore";
    s.max_stack_size = 64;

    item_def(42, IG_RESOURCE, "iridium_bar");
    sprite_def(i1, 4,2);
    s.pretty_name = (char*)"Iridium Bar";
    s.max_stack_size = 64;

    item_def(43, IG_SHOVEL, "iridium_shovel");
    sprite_def(i1, 4,1);
    s.pretty_name = (char*)"Iridium Shovel";
    s.melee_fire_cooldown = 250;
    s.melee_damage = 30;
    s.max_durability = 400;
    s.max_stack_size = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 7;
    s.block_damage = 8;
    s.object_damage_min = 30;
    s.object_damage_max = 40;

    item_def(44, IG_NONE, "fist");
    //s.pretty_name = (char*)"Fist";
    s.mining_fire_cooldown = 200;
    s.mining_damage = 1;
    s.firing_range = 4.0f;
    s.firing_rate = 8;
    s.block_damage = 4;
    s.object_damage_min = 5;
    s.object_damage_max = 10;

    item_def(45, IG_RESOURCE, "graphite");
    sprite_def(i0, 5,2);
    s.pretty_name = (char*)"Graphite";
    s.max_stack_size = 64;
    s.fuel = true;
    s.fuel_burn_rate = 30 * 30; // 30 seconds
    
    item_def(46, IG_PLACER, "iron_block");
    iso_block_sprite_def("steel_1");
    s.pretty_name = (char*)"Iron Block";
    s.placer_block_type_id = t_map::dat_get_cube_id("steel_1");
    s.max_stack_size = 16;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"steel_1");
    
    item_def(47, IG_PLACER, "steel_block");
    s.pretty_name = (char*)"Steel Block";
    iso_block_sprite_def("steel_2");
    s.placer_block_type_id = t_map::dat_get_cube_id("steel_2");
    s.max_stack_size = 16;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"steel_2");

    item_def(48, IG_CONSUMABLE, "repair_kit");
    sprite_def(i0, 2,8);
    s.pretty_name = (char*)"Repair Kit";
    s.max_stack_size = 1;
    s.repair_agent_amount = 50;

    item_def(49, IG_PLACER, "cryofreezer_1");
    iso_block_sprite_def("cryofreezer_1");
    s.pretty_name = (char*)"Small Cryofreezer";
    s.placer_block_type_id = t_map::dat_get_cube_id("cryofreezer_1");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"cryofreezer_1");
    s.container_type = CONTAINER_TYPE_CRYOFREEZER_SMALL;
    container_block_def("cryofreezer_1", CONTAINER_TYPE_CRYOFREEZER_SMALL);

    item_def(50, IG_RESOURCE, "methane_ice");
    sprite_def(i0, 1,4);
    s.pretty_name = (char*)"Methane Ice";
    s.max_stack_size = 16;
    s.gas = true;
    s.fuel = true;
    s.fuel_burn_rate = 30 * 30; // 30 seconds

    item_def(51, IG_PLACER, "smelter_1");
    iso_block_sprite_def("smelter_1");
    s.pretty_name = (char*)"Simple Smelter";
    s.placer_block_type_id = t_map::dat_get_cube_id("smelter_1");
    s.max_stack_size = 1;
    s.particle_voxel = true;
    s.particle_voxel_texture = t_map::get_cube_primary_texture_index((char*)"smelter_1");
    s.container_type = CONTAINER_TYPE_SMELTER_ONE;
    container_block_def("smelter_1", CONTAINER_TYPE_SMELTER_ONE);

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
#if DC_CLIENT
    if(_item_cube_iso_spritesheet_id == -1)
    {
        printf("Error: iso_block_sprite_def, must call start_item_dat!!!\n");
        GS_ABORT();
    }

    int id = t_map::dat_get_cube_id(block_name);

    int xpos = id % 16;
    int ypos = id / 16;

    int index = LUA_blit_item_texture(_item_cube_iso_spritesheet_id, xpos+1, ypos+1);
    sprite_array[_current_item_id] = index; //check
#endif
}




}
