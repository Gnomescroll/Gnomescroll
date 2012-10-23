#pragma once

#include <item/config/_interface.hpp>

#include <t_map/t_properties.hpp>
#include <t_map/common/types.hpp>

namespace Item
{

void verify_item_dat();

void load_item_dat()
{
    start_item_dat();
    int i0 = texture_alias("./media/sprites/item/i00.png");
    int i1 = texture_alias("./media/sprites/item/i01.png");
    int p0 = texture_alias("./media/sprites/item/p00.png");
    //int i2 = texture_alias("./media/sprites/item/i02.png");

    item_def(IG_ERROR, "error_item");
    set_pretty_name("Error");
    sprite_def(i0, 4,1);

    item_def(IG_PLACER, "regolith");
    iso_block_sprite_def("regolith");
    s->block_type_id = t_map::dat_get_cube_id("regolith");
    set_pretty_name("Regolith");
    s->max_stack_size = 64;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("regolith");
    s->cube_height = 1;

    item_def(IG_PLACER, "rock");
    iso_block_sprite_def("rock");
    s->block_type_id = t_map::dat_get_cube_id("rock");
    set_pretty_name("Rock");
    s->max_stack_size = 64;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("rock");
    s->cube_height = 1;
    
    item_def(IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);
    set_pretty_name("Quartz Crystal");

    item_def(IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i1, 5,8);
    set_pretty_name("Laser Rifle");
    s->hitscan_fire_cooldown = 30;
    s->hitscan_damage = 5;
    s->hitscan_bullet_effect_enum = 0;
    s->max_durability = 100;
    s->max_stack_size = 1;
    s->object_damage_min = 15;
    s->object_damage_max = 25;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 0);

    item_def(IG_MINING_LASER, "blue_mining_laser");
    sprite_def(i0, 3,5);
    set_pretty_name("Mining Laser");
    s->max_durability = 1200;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 6);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 15;
    s->object_damage_max = 20;
    s->animation_id = 0;

    item_def(IG_MINING_LASER, "red_mining_laser");
    sprite_def(i0, 3,7);
    set_pretty_name("Mining Laser");
    s->max_durability = 1200;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 8);
    block_damage_def(CUBE_MATERIAL_DECORATION, 12);
    s->object_damage_min = 20;
    s->object_damage_max = 25;
    s->animation_id = 1;

    item_def(IG_MINING_LASER, "green_mining_laser");
    sprite_def(i0, 4,7);
    set_pretty_name("Mining Laser");
    s->max_durability = 1200;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 7);
    block_damage_def(CUBE_MATERIAL_STONE, 10);
    block_damage_def(CUBE_MATERIAL_DECORATION, 16);
    s->object_damage_min = 25;
    s->object_damage_max = 30;
    s->animation_id = 2;

    item_def(IG_SYNTHESIZER_COIN, "synthesizer_coin");
    sprite_def(i1, 2, 7);
    set_pretty_name("Synthesizer Chip");
    s->max_stack_size = 999;

    item_def(IG_GRENADE_LAUNCHER, "plasma_grenade");
    sprite_def(i0, 1,7);
    set_pretty_name("Plasma Grenade");
    s->max_stack_size = 100;

    item_def(IG_DEBUG, "location_pointer");
    sprite_def(i0, 4,2);
    set_pretty_name("Location Pointer");

    item_def(IG_DEBUG, "block_placer");
    sprite_def(i0, 4,5);
    set_pretty_name("Block Placer");
    s->cube_height = 1;
    
    item_def(IG_UNKNOWN, "unknown");
    sprite_def(i0, 5,1);
    set_pretty_name("Unknown");

    item_def(IG_PLACER, "small_storage");
    iso_block_sprite_def("small_storage");
    s->block_type_id = t_map::dat_get_cube_id("small_storage");
    set_pretty_name("Small Storage Block");
    s->max_stack_size = 1;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("small_storage");
    s->container_type = CONTAINER_TYPE_STORAGE_BLOCK_SMALL;
    container_block_def("small_storage", CONTAINER_TYPE_STORAGE_BLOCK_SMALL);
    s->cube_height = 1;

    item_def(IG_PLACER, "small_crafting_bench");
    iso_block_sprite_def("small_crafting_bench");
    set_pretty_name("Small Crafting Bench");
    s->block_type_id = t_map::dat_get_cube_id("small_crafting_bench");
    s->max_stack_size = 1;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("small_crafting_bench");
    s->container_type = CONTAINER_TYPE_CRAFTING_BENCH_UTILITY;
    container_block_def("small_crafting_bench", CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    s->cube_height = 1;

    item_def(IG_RESOURCE, "copper_ore");
    sprite_def(i1, 3,3);
    set_pretty_name("Copper Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "copper_bar");
    sprite_def(i1, 3,2);
    set_pretty_name("Copper Bar");
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "copper_shovel");
    sprite_def(i1, 3,1);
    set_pretty_name("Copper Shovel");
    s->melee_fire_cooldown = 250;
    s->melee_damage = 2;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s->object_damage_min = 15;
    s->object_damage_max = 25;

    item_def(IG_RESOURCE, "iron_ore");
    sprite_def(i0, 5,7);
    set_pretty_name("Iron Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iron_bar");
    sprite_def(i1, 1,2);
    set_pretty_name("Iron Bar");
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "iron_shovel");
    sprite_def(i1, 1,1);
    set_pretty_name("Iron Shovel");
    s->melee_fire_cooldown = 250;
    s->melee_damage = 6;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 6);
    s->object_damage_min = 25;
    s->object_damage_max = 35;

    item_def(IG_RESOURCE, "gallium_ore");
    sprite_def(i1, 2,3);
    set_pretty_name("Gallium Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "gallium_bar");
    sprite_def(i1, 2,2);
    set_pretty_name("Gallium Bar");
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "gallium_shovel");
    sprite_def(i1, 2,1);
    set_pretty_name("Gallium Shovel");
    s->melee_fire_cooldown = 250;
    s->melee_damage = 4;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 20;
    s->object_damage_max = 30;

    item_def(IG_RESOURCE, "iridium_ore");
    sprite_def(i1, 4,3);
    set_pretty_name("Iridium Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iridium_bar");
    sprite_def(i1, 4,2);
    set_pretty_name("Iridium Bar");
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "iridium_shovel");
    sprite_def(i1, 4,1);
    set_pretty_name("Iridium Shovel");
    s->melee_fire_cooldown = 250;
    s->melee_damage = 30;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 8);
    block_damage_def(CUBE_MATERIAL_STONE, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 30;
    s->object_damage_max = 40;

    item_def(IG_FIST, "fist");
    sprite_def(i1, 6, 2);
    set_pretty_name("Fist");
    s->firing_range = 4.0f;
    s->firing_rate = 6;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s->object_damage_min = 5;
    s->object_damage_max = 10;

    item_def(IG_RESOURCE, "coal");
    sprite_def(i0, 5,2);
    set_pretty_name("Coal");
    s->max_stack_size = 64;
    s->fuel = true;
    s->fuel_burn_rate = 30 * 30; // 30 seconds
    
    item_def(IG_PLACER, "steel_block_1");
    iso_block_sprite_def("steel_block_1");
    set_pretty_name("Steel Block #1");
    s->block_type_id = t_map::dat_get_cube_id("steel_block_1");
    s->max_stack_size = 16;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_1");
    s->cube_height = 1;

    item_def(IG_PLACER, "steel_block_2");
    set_pretty_name("Steel block #2");
    iso_block_sprite_def("steel_block_2");
    s->block_type_id = t_map::dat_get_cube_id("steel_block_2");
    s->max_stack_size = 16;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_2");
    s->cube_height = 1;

    item_def(IG_PLACER, "steel_block_3");
    set_pretty_name("Steel Block #3");
    iso_block_sprite_def("steel_block_3");
    s->block_type_id = t_map::dat_get_cube_id("steel_block_3");
    s->max_stack_size = 16;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("steel_block_3");
    s->cube_height = 1;

    item_def(IG_CONSUMABLE, "small_charge_pack");
    sprite_def(i1, 6,1);
    set_pretty_name("Small Charge Pack");
    s->max_stack_size = 16;
    s->repair_agent_amount = 25;

    item_def(IG_PLACER, "cryofreezer_1");
    iso_block_sprite_def("cryofreezer_1");
    set_pretty_name("Small Cryofreezer");
    s->block_type_id = t_map::dat_get_cube_id("cryofreezer_1");
    s->max_stack_size = 1;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("cryofreezer_1");
    s->container_type = CONTAINER_TYPE_CRYOFREEZER_SMALL;
    container_block_def("cryofreezer_1", CONTAINER_TYPE_CRYOFREEZER_SMALL);
    s->cube_height = 1;

    item_def(IG_RESOURCE, "methane_ice");
    sprite_def(i0, 1,4);
    set_pretty_name("Methane Ice");
    s->max_stack_size = 16;
    s->gas = true;
    s->gas_lifetime = 30 * 60 * 5;   // 5 minutes
    s->fuel = true;
    s->fuel_burn_rate = 30 * 30; // 30 seconds

    item_def(IG_PLACER, "smelter_1");
    iso_block_sprite_def("smelter_1");
    set_pretty_name("Simple Smelter");
    s->block_type_id = t_map::dat_get_cube_id("smelter_1");
    s->max_stack_size = 1;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("smelter_1");
    s->container_type = CONTAINER_TYPE_SMELTER_ONE;
    container_block_def("smelter_1", CONTAINER_TYPE_SMELTER_ONE);
    s->cube_height = 1;

    item_def(IG_PLACER, "control_node");
    iso_block_sprite_def("control_node");
    set_pretty_name("Control Node");
    s->block_type_id = t_map::dat_get_cube_id("control_node");
    s->max_stack_size = 8;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("control_node");
    s->cube_height = 1;

    item_def(IG_ENERGY_TANK, "energy_tank");
    sprite_def(i0, 2,8);
    set_pretty_name("Energy Tank");
    s->max_stack_size = 1;

    item_def(IG_AGENT_SPAWNER, "agent_spawner");
    sprite_def(i1, 2,6);
    set_pretty_name("Spawner");
    s->max_stack_size = 1;
    s->cube_height = 2;

    item_def(IG_ENERGY_CORE, "energy_core");
    sprite_def(i1, 3,6);
    set_pretty_name("Energy Core");
    s->max_stack_size = 1;
    s->cube_height = 1;

    item_def(IG_MECH_PLACER, "crystal_seed");
    sprite_def(i1, 5,1);
    s->mech_type = t_mech::get_mech_type("red_crystal");
    set_pretty_name("Crystal Seed");
    s->max_stack_size = 8;

    item_def(IG_MECH_PLACER, "crystal_seed2");
    sprite_def(i1, 5,2);
    s->mech_type = t_mech::get_mech_type("blue_crystal");
    set_pretty_name("Crystal Seed");
    s->max_stack_size = 8;

    item_def(IG_PLACER, "crusher");
    iso_block_sprite_def("crusher");
    set_pretty_name("Crusher");
    s->block_type_id = t_map::dat_get_cube_id("crusher");
    s->max_stack_size = 1;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index("crusher");
    s->container_type = CONTAINER_TYPE_CRUSHER;
    container_block_def("crusher", CONTAINER_TYPE_CRUSHER);
    s->cube_height = 1;

    item_def(IG_RESOURCE, "powdered_regolith");
    sprite_def(i0, 4,6);
    set_pretty_name("Powdered Regolith");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iron_rod");
    sprite_def(i1, 1,7);
    set_pretty_name("Iron Rod");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "iron_star");
    sprite_def(i1, 4,7);
    set_pretty_name("Iron Star");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "iron_blade");
    sprite_def(i1, 4,6);
    set_pretty_name("Iron Blade");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "copper_wire");
    sprite_def(i1, 1,6);
    set_pretty_name("Copper Wire");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "iron_helmet");
    sprite_def(i1, 5,3);
    set_pretty_name("Iron Helmet");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "gallium_helmet");
    sprite_def(i1, 5,4);
    set_pretty_name("Gallium Helmet");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "copper_helmet");
    sprite_def(i1, 5,5);
    set_pretty_name("Copper Helmet");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "iridium_helmet");
    sprite_def(i1, 5,6);
    set_pretty_name("Iridium Helmet");
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "blue_crystal");
    sprite_def(i1, 3,4);
    set_pretty_name("Crystal");
    s->max_stack_size = 16;

    item_def(IG_RESOURCE, "red_crystal");
    sprite_def(i0, 1,3);
    set_pretty_name("Crystal");
    s->max_stack_size = 16;

    item_def(IG_RESOURCE, "green_crystal");
    sprite_def(i0, 2,4);
    set_pretty_name("Crystal");
    s->max_stack_size = 16;

    item_def(IG_MECH_PLACER, "acadia_seed");
    sprite_def(p0, 1,2);
    s->mech_type = t_mech::get_mech_type("acadia_flower");
    set_pretty_name("Acadia Seed");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "acadia_fruit");
    sprite_def(p0, 1,9);
    s->max_stack_size = 16;
    set_pretty_name("Acadia Fruit");

    item_def(IG_MECH_PLACER, "blob_seed");
    sprite_def(p0, 3,2);
    s->mech_type = t_mech::get_mech_type("blob_flower");
    set_pretty_name("Blob Seed");
    s->max_stack_size = 64;

    end_item_dat(); // finalize
    
    verify_item_dat();
}

void verify_item_dat()
{
    for (int i=0; i<MAX_ITEM_TYPES; i++)
    {
        class ItemAttribute* a = &item_attributes[i];
        
        if (!a->loaded) continue;

        GS_ASSERT_ABORT(a->item_type != NULL_ITEM_TYPE);

        GS_ASSERT_ABORT(is_valid_item_name(a->name));

        // make sure group is set
        GS_ASSERT_ABORT(a->group != IG_NONE);
    
        GS_ASSERT_ABORT(a->pretty_name[0] != '\0');

        // make sure all data types are within bounds
        GS_ASSERT_ABORT(a->max_durability > 0
                && a->max_durability <= MAX_DURABILITY);
                
        GS_ASSERT_ABORT(a->max_stack_size > 0
                && a->max_stack_size <= MAX_STACK_SIZE);

        // particle voxel texture shouldn't be set if its not a particle voxel -- likely mistake
        GS_ASSERT_ABORT(a->particle_voxel || (!a->particle_voxel && a->particle_voxel_texture == 0));
        // particle voxel should have a cube height of 1
        GS_ASSERT_ABORT(!a->particle_voxel || a->cube_height == 1);

        // gas lifetime should be set, if it is a gas
        GS_ASSERT_ABORT(!a->gas || a->gas_lifetime != NULL_GAS_LIFETIME);
    }
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
    GS_ASSERT_ABORT(_item_cube_iso_spritesheet_id != -1);

    int id = t_map::dat_get_cube_id(block_name);

    int xpos = id % 16;
    int ypos = id / 16;

    int index = LUA_blit_item_texture(_item_cube_iso_spritesheet_id, xpos+1, ypos+1);
    sprite_array[_current_item_index] = index; //check
    #endif
}

}   // Item
