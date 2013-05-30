#include "item_dat.hpp"

#include <item/config/_interface.hpp>
#include <t_map/t_properties.hpp>
#include <t_map/common/types.hpp>
#include <SDL/texture_sheet_loader.hpp>
#include <animations/config/_interface.hpp>

namespace Item
{

void load_item_dat()
{
    using TextureSheetLoader::item_texture_alias;
    SpriteSheet i0 = item_texture_alias(MEDIA_PATH "sprites/item/i00.png");
    SpriteSheet i1 = item_texture_alias(MEDIA_PATH "sprites/item/i01.png");
    SpriteSheet rdn = item_texture_alias(MEDIA_PATH "sprites/item/rdn.png");
    //SpriteSheet iceflame = item_texture_alias(MEDIA_PATH "sprites/item/iceflame.png");
    SpriteSheet p0 = item_texture_alias(MEDIA_PATH "sprites/item/p00.png");
    //int i2 = item_texture_alias(MEDIA_PATH "sprites/item/i02.png");

    // Automatic block drop items are created at a later step, after the block drop dat
    // If you want to override any default properties for IG_PLACER blocks,
    // use item_block_def()
    // the pretty name will be automatically generated from the block name
    // replacing _ with space and capitalizing all first letters

    const float MELEE_FIRING_RANGE = 4.0f;

    // special purpose
    item_def(IG_ERROR, "error_item");
    set_pretty_name("Error");
    sprite_def(rdn, 1,1);
    s->boonable = false;

    // special purpose
    item_def(IG_UNKNOWN, "unknown");
    sprite_def(rdn, 2,1);
    s->boonable = false;

    // containers
    item_container_def("storage_block_small");
    set_pretty_name("Small Storage Block");

    item_container_def("crafting_bench_basic");
    set_pretty_name("Small Crafting Bench");

    item_container_def("smelter_basic");
    set_pretty_name("Simple Smelter");

    item_container_def("cryofreezer_small");
    set_pretty_name("Small Cryofreezer");

    item_container_def("crusher");

    // manually define these blocks, so we can set a custom pretty name
    item_block_def("steel_block_1");
    set_pretty_name("Steel Block #1");

    item_block_def("steel_block_2");
    set_pretty_name("Steel block #2");

    item_block_def("steel_block_3");
    set_pretty_name("Steel Block #3");

    // manually define these blocks, because their source block drops multiple things
    item_block_def("regolith");
    item_block_def("rock");

    item_def(IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);

    item_def(IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i1, 5,8);
    s->hitscan_fire_cooldown = 30;
    s->hitscan_damage = 5;
    s->hitscan_bullet_effect_enum = 0;
    s->max_durability = 100;
    s->max_stack_size = 1;
    s->object_damage_min = 15;
    s->object_damage_max = 25;
    s->firing_rate = (5 * ONE_SECOND) / 6;
    s->firing_range = 128.0f;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 0);

    item_def(IG_MINING_LASER, "blue_mining_laser");
    sprite_def(i0, 3,5);
    set_pretty_name("Mining Laser");
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    s->max_durability = (ONE_MINUTE * 10) / s->firing_rate; // 10 minutes of continuous use
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 6);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 15;
    s->object_damage_max = 20;
    s->animation_type = Animations::get_animation_type("blue_mining_laser");

    item_def(IG_MINING_LASER, "red_mining_laser");
    sprite_def(i0, 3,7);
    set_pretty_name("Mining Laser");
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    s->max_durability = (ONE_MINUTE * 10) / s->firing_rate; // 10 minutes of continuous use
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 8);
    block_damage_def(CUBE_MATERIAL_DECORATION, 12);
    s->object_damage_min = 20;
    s->object_damage_max = 25;
    s->animation_type = Animations::get_animation_type("red_mining_laser");

    item_def(IG_MINING_LASER, "green_mining_laser");
    sprite_def(i0, 4,7);
    set_pretty_name("Mining Laser");
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    s->max_durability = (ONE_MINUTE * 10) / s->firing_rate; // 10 minutes of continuous use
    block_damage_def(6);
    block_damage_def(CUBE_MATERIAL_DIRT, 7);
    block_damage_def(CUBE_MATERIAL_STONE, 10);
    block_damage_def(CUBE_MATERIAL_DECORATION, 16);
    s->object_damage_min = 25;
    s->object_damage_max = 30;
    s->animation_type = Animations::get_animation_type("green_mining_laser");

    item_def(IG_SYNTHESIZER_COIN, "synthesizer_coin");
    sprite_def(i1, 2, 7);
    set_pretty_name("Synthesizer Chip");
    s->max_stack_size = 999;

    item_def(IG_GRENADE_LAUNCHER, "plasma_grenade");
    sprite_def(i0, 1,7);
    s->max_stack_size = 100;

    item_def(IG_DEBUG, "location_pointer");
    sprite_def(rdn, 1,2);
    s->boonable = false;

    item_def(IG_DEBUG, "block_placer");
    sprite_def(rdn, 1,5);
    s->cube_height = 1;
    s->boonable = false;

    item_def(IG_RESOURCE, "copper_ore_piece");
    sprite_def(i1, 3,3);
    set_pretty_name("Copper Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "copper_bar");
    sprite_def(i1, 3,2);
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "copper_shovel");
    sprite_def(i1, 3,1);
    s->melee_fire_cooldown = 250;
    s->melee_damage = 2;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 5);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s->object_damage_min = 15;
    s->object_damage_max = 25;

    item_def(IG_RESOURCE, "iron_ore_piece");
    sprite_def(i0, 5,7);
    set_pretty_name("Iron Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iron_bar");
    sprite_def(i1, 1,2);
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "iron_shovel");
    sprite_def(i1, 1,1);
    s->melee_fire_cooldown = 250;
    s->melee_damage = 6;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 6);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 6);
    s->object_damage_min = 25;
    s->object_damage_max = 35;

    item_def(IG_RESOURCE, "gallium_ore_piece");
    sprite_def(i1, 2,3);
    set_pretty_name("Gallium Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "gallium_bar");
    sprite_def(i1, 2,2);
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "gallium_shovel");
    sprite_def(i1, 2,1);
    s->melee_fire_cooldown = 250;
    s->melee_damage = 4;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 7);
    block_damage_def(CUBE_MATERIAL_STONE, 1);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 20;
    s->object_damage_max = 30;

    item_def(IG_RESOURCE, "iridium_ore_piece");
    sprite_def(i1, 4,3);
    set_pretty_name("Iridium Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iridium_bar");
    sprite_def(i1, 4,2);
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "iridium_shovel");
    sprite_def(i1, 4,1);
    s->melee_fire_cooldown = 250;
    s->melee_damage = 30;
    s->max_durability = 400;
    s->max_stack_size = 1;
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(3);
    block_damage_def(CUBE_MATERIAL_DIRT, 8);
    block_damage_def(CUBE_MATERIAL_STONE, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 8);
    s->object_damage_min = 30;
    s->object_damage_max = 40;

    item_def(IG_RESOURCE, "silicon_ore_piece");
    sprite_def(rdn, 3,2);
    set_pretty_name("Silicon Ore");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "silicon_crystal");
    sprite_def(rdn, 3,1);
    set_pretty_name("Refined Silicon");
    s->max_stack_size = 64;

    item_def(IG_SHOVEL, "silicon_dagger");
    sprite_def(rdn, 3,3);
    set_pretty_name("Crystal Dagger");
    s->melee_fire_cooldown = 500;
    s->melee_damage = 60;
    s->max_durability = 100;
    s->max_stack_size = 1;
    s->firing_range = 3.0f;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(4);
    block_damage_def(CUBE_MATERIAL_STONE, 0);
    s->object_damage_min = 50;
    s->object_damage_max = 75;

    item_def(IG_RESOURCE, "silicon_powder");
    sprite_def(rdn, 3,4);
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "glass_piece");
    set_pretty_name("Piece Of Glass");
    sprite_def(i1, 1,5);
    s->max_stack_size = 16;

    item_def(IG_RESOURCE, "glass_lens");
    sprite_def(i1, 3,5);
    s->max_stack_size = 8;

    item_def(IG_TOOL, "glass_scope");
    sprite_def(rdn, 4,1);
    s->max_stack_size = 1;

    item_def(IG_HITSCAN_WEAPON, "scoped_laser_rifle");
    sprite_def(i1, 6,8);
    s->hitscan_fire_cooldown = 30;
    s->hitscan_damage = 5;
    s->hitscan_bullet_effect_enum = 0;
    s->max_durability = 100;
    s->max_stack_size = 1;
    s->object_damage_min = 15;
    s->object_damage_max = 25;
    s->firing_rate = (5 * ONE_SECOND) / 6;
    s->firing_range = 128.0f;
    block_damage_def(2);
    block_damage_def(CUBE_MATERIAL_DIRT, 4);
    block_damage_def(CUBE_MATERIAL_STONE, 0);

    item_def(IG_FIST, "fist");
    sprite_def(i1, 6, 2);
    s->firing_range = MELEE_FIRING_RANGE;
    s->firing_rate = ONE_SECOND / 3;
    block_damage_def(1);
    block_damage_def(CUBE_MATERIAL_DIRT, 2);
    block_damage_def(CUBE_MATERIAL_DECORATION, 4);
    s->object_damage_min = 5;
    s->object_damage_max = 10;
    s->boonable = false;

    item_def(IG_RESOURCE, "coal_nugget");
    sprite_def(i1, 1,3);
    set_pretty_name("Coal");
    s->max_stack_size = 64;
    s->fuel = true;
    s->fuel_burn_rate = ONE_SECOND * 30;

    item_def(IG_CONSUMABLE, "small_charge_pack");
    sprite_def(i1, 6,1);
    s->max_stack_size = 16;
    set_modifier_amount("health", 10);
    set_modifier_instant();

    item_def(IG_RESOURCE, "methane_ice_chunk");
    sprite_def(i0, 1,4);
    set_pretty_name("Methane Ice");
    s->max_stack_size = 16;
    s->gas = true;
    s->gas_lifetime = ONE_MINUTE * 5;
    s->fuel = true;
    s->fuel_burn_rate = ONE_SECOND * 30;

    item_def(IG_ENERGY_TANK, "energy_tank");
    sprite_def(i0, 2,8);
    set_pretty_name("Energy Shield");
    s->max_stack_size = 1;
    s->max_durability = 30;

    item_def(IG_AGENT_SPAWNER, "agent_spawner");
    sprite_def(i1, 2,6);
    set_pretty_name("Spawner");
    s->max_stack_size = 1;
    s->cube_height = 2;

    item_def(IG_ENERGY_CORE, "energy_core");
    sprite_def(i1, 3,6);
    s->max_stack_size = 1;
    s->cube_height = 1;

    item_def(IG_MECH_PLACER, "crystal_seed");
    sprite_def(i1, 5,1);
    s->mech_type = t_mech::get_mech_type("red_crystal");
    s->max_stack_size = 8;

    item_def(IG_MECH_PLACER, "crystal_seed2");
    sprite_def(i1, 5,2);
    s->mech_type = t_mech::get_mech_type("blue_crystal");
    s->max_stack_size = 8;

    item_def(IG_RESOURCE, "powdered_regolith");
    sprite_def(rdn, 2,2);
    set_pretty_name("Fine Regolith");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "crushed_rock");
    sprite_def(rdn, 2,4);
    set_pretty_name("Crushed Stone");
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iron_rod");
    sprite_def(i1, 1,7);
    s->max_stack_size = 64;

    item_def(IG_RESOURCE, "iron_star");
    sprite_def(i1, 4,7);
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "iron_blade");
    sprite_def(i1, 4,6);
    s->max_stack_size = 1;

    item_def(IG_RESOURCE, "copper_wire");
    sprite_def(i1, 1,6);
    s->max_stack_size = 1;

    item_def(IG_MECH_PLACER, "copper_ladder");
    sprite_def(i0, 5,2);
    s->mech_type = t_mech::get_mech_type("copper_ladder");
    s->max_stack_size = 16;

    item_def(IG_EQUIPMENT, "copper_helmet");
    sprite_def(i1, 5,5);
    s->equipment_type = EQUIPMENT_TYPE_HEAD;
    set_modifier_amount("health_max", 5);

    item_def(IG_EQUIPMENT, "iron_helmet");
    sprite_def(i1, 5,3);
    s->equipment_type = EQUIPMENT_TYPE_HEAD;
    set_modifier_amount("health_max", 10);

    item_def(IG_EQUIPMENT, "gallium_helmet");
    sprite_def(i1, 5,4);
    s->equipment_type = EQUIPMENT_TYPE_HEAD;
    set_modifier_amount("health_max", 15);

    item_def(IG_EQUIPMENT, "iridium_helmet");
    sprite_def(i1, 5,6);
    s->equipment_type = EQUIPMENT_TYPE_HEAD;
    set_modifier_amount("health_max", 20);

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
    s->mech_type = t_mech::get_mech_type("acadia_flower_stage_0");
    s->max_stack_size = 16;

    item_def(IG_CONSUMABLE, "acadia_fruit");
    sprite_def(p0, 1,1);
    s->max_stack_size = 16;
    set_modifier_amount("hunger", -2);
    set_modifier_instant();

    item_def(IG_MECH_PLACER, "blob_seed");
    sprite_def(p0, 2,1);
    s->mech_type = t_mech::get_mech_type("blob_flower");
    s->max_stack_size = 64;

    item_def(IG_MECH_PLACER, "cryptid_larvae");
    sprite_def(p0, 3,1);
    s->mech_type = t_mech::get_mech_type("cryptid_larvae_stage_0");
    s->max_stack_size = 16;

    item_def(IG_CONSUMABLE, "cooked_cryptid_larvae");
    sprite_def(i1, 9,2);
    s->max_stack_size = 16;
    set_modifier_amount("hunger", -3);
    set_modifier_instant();

    item_def(IG_RESOURCE, "blackpowder");
    sprite_def(rdn, 2,3);
    s->max_stack_size = 64;

    item_block_def("plasmagen");
    item_block_def("rock_landmine");
    item_block_def("regolith_landmine");

    item_def(IG_SPECIAL, "boon_crank");
    sprite_def(rdn, 2,5);
    s->max_charges = 3;
    s->boonable = false;
    #if PRODUCTION
    s->recharge_rate = ONE_HOUR * 4;
    #else
    s->recharge_rate = ONE_SECOND * 5;
    #endif

    item_def(IG_PLANT_PLACER, "placer_plant_example");
    sprite_def(i1, 7,6);
    set_pretty_name("placer plant example");
    s->max_stack_size = 16;

    item_def(IG_MECH_PLACER_ORIENTED, "led_torch");
    sprite_def(rdn, 4, 2);
    set_pretty_name("LED");
    s->max_stack_size = 64;
    s->mech_type = t_mech::get_mech_type("light_crystal");

    item_attributes->done_loading();
}

void create_items_from_blocks()
{
    // this will create items for all blocks that will drop themselves when destroyed
    // if you need non-default properties for these items, define them in the item_dat
    // with item_block_def(block_name);
    // pretty names are automatically generated from the internal name
    // (overriding the pretty name is the most common use case for overriding this auto item)

    // TODO -- client does not have access to drop dat
    // Need to give it enough access to figure out which automatic items to make

    GS_ASSERT_ABORT(t_map::defined_drops != NULL);
    IF_ASSERT(t_map::defined_drops == NULL) return;

    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!t_map::isValidCube((CubeType)i)) continue;  // skips empty and error blocks
        const char* name = t_map::get_cube_name((CubeType)i);
        GS_ASSERT_ABORT(name != NULL);
        IF_ASSERT(name == NULL) continue;

        // ignore blocks that have been overriden
        bool already_defined = false;
        for (size_t i=0; i<item_attributes->max; i++)
            if (item_attributes->properties[i].loaded &&
                strcmp(name, item_attributes->properties[i].name) == 0)
            {
                if (item_attributes->properties[i].group != IG_PLACER)
                    printf("This name is already in use by a block: %s\n", name);
                // an item with this name has been defined, but it is not a block
                GS_ASSERT_ABORT(item_attributes->properties[i].group == IG_PLACER);
                already_defined = true;
                break;
            }
        if (already_defined) continue;

        CubeType cube_type = t_map::get_cube_type(name);
        GS_ASSERT_ABORT(t_map::isValidCube(cube_type));
        if (!t_map::isValidCube(cube_type)) continue;

        // blocks that have drops defined (at this point) will not be dropping themselves
        if (t_map::defined_drops[cube_type] != t_map::DROP_UNDEFINED) continue;

        bool defined = item_block_def(name);
        GS_ASSERT_ABORT(defined);
    }

    item_attributes->done_loading();
}

#define ITEM_NAME_FILE_ACTIVE   "item_names.active"
#define ITEM_NAME_FILE_INACTIVE "item_names.inactive"

void verify_item_dat()
{
    int errct = 0;  // count error items
    for (size_t i=0; i<item_attributes->max; i++)
    {
        class ItemAttribute* a = &item_attributes->properties[i];

        if (!a->loaded) continue;

        GS_ASSERT_ABORT(a->type != NULL_ITEM_TYPE);
        GS_ASSERT_ABORT(a->type == i);

        GS_ASSERT_ABORT(is_valid_name(a->name));

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

        GS_ASSERT_ABORT(!a->particle_voxel || (a->cube_type != NULL_CUBE && a->cube_type != ERROR_CUBE && a->cube_type != EMPTY_CUBE));

        // gas lifetime should be set, if it is a gas
        GS_ASSERT_ABORT(!a->gas || a->gas_lifetime != NULL_GAS_LIFETIME);

        GS_ASSERT_ABORT(a->max_charges == NULL_CHARGES || (a->max_charges > 0 && a->max_charges < MAX_MAX_CHARGES));
        GS_ASSERT_ABORT(a->recharge_rate > 0);

        #if DC_CLIENT
        GS_ASSERT_ABORT(a->group == IG_ERROR || a->particle_voxel || a->sprite != ERROR_SPRITE);
        #endif

        GS_ASSERT_ABORT(a->cube_type == NULL_CUBE || t_map::isValidCube(a->cube_type));

        GS_ASSERT_ABORT((a->group == IG_EQUIPMENT && a->equipment_type != NULL_EQUIPMENT_TYPE) ||
                        (a->group != IG_EQUIPMENT && a->equipment_type == NULL_EQUIPMENT_TYPE));

        GS_ASSERT_ABORT(a->group != IG_EQUIPMENT || a->max_stack_size == 1)
        GS_ASSERT_ABORT(a->group != IG_EQUIPMENT || a->max_charges == NULL_CHARGES)

        if (a->group == IG_ERROR) errct++;
    }

    // only 1 error item allowed
    GS_ASSERT_ABORT(errct == 1);

    for (size_t i=0; i<item_attributes->max-1; i++)
    for (size_t j=i+1; j<item_attributes->max; j++)
    {
        class ItemAttribute* a = &item_attributes->properties[i];
        class ItemAttribute* b = &item_attributes->properties[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT_ABORT(strcmp(a->name, b->name) != 0);
        IF_ASSERT(a->sprite != ERROR_SPRITE && a->sprite == b->sprite)
        {
            printf("%s and %s have the same sprite (%d == %d)\n", a->name, b->name, a->sprite, b->sprite);
            exit(1);
        }
        GS_ASSERT_ABORT(a->type != b->type);
        GS_ASSERT_ABORT(a->cube_type == NULL_CUBE || a->cube_type != b->cube_type);
    }

    GS_ASSERT_ABORT(item_name_map->condensed);

    // check inactive names against active
    for (size_t i=0; i<item_attributes->max; i++)
        if (item_attributes->properties[i].loaded)
        {
            GS_ASSERT_ABORT(item_name_map->get_mapped_name(item_attributes->properties[i].name) == NULL);
        }

    // check inactive name destinations against active
    for (size_t i=0; i<item_name_map->size; i++)
    {
        GS_ASSERT_ABORT(get_item_type(item_name_map->get_replacement(i)) != NULL_ITEM_TYPE);
    }

    #if DC_SERVER || !PRODUCTION
    // either both files must be missing or both must exist
    bool active_dat = file_exists(DATA_PATH ITEM_NAME_FILE_ACTIVE);
    bool inactive_dat = file_exists(DATA_PATH ITEM_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT((active_dat && inactive_dat) || (!active_dat && !inactive_dat));

    if (active_dat && inactive_dat)
    {   // check that all names declared a valid with respect to past name definitions
        // but only if the files are present
        GS_ASSERT_ABORT(name_changes_valid(
            DATA_PATH ITEM_NAME_FILE_ACTIVE, DATA_PATH ITEM_NAME_FILE_INACTIVE,
            DAT_NAME_MAX_LENGTH,
            item_attributes->properties, MAX_ITEM_TYPES, item_name_map));
    }
    #endif
}

void save_item_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(item_attributes->properties,
        MAX_ITEM_TYPES, DAT_NAME_MAX_LENGTH, DATA_PATH ITEM_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = item_name_map->save(DATA_PATH ITEM_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}

#undef ITEM_NAME_FILE_ACTIVE
#undef ITEM_NAME_FILE_INACTIVE

// Use this to remove or rename a item
void change_item(const char* original, const char* replacement)
{
    GS_ASSERT_ABORT(is_valid_name(original));
    GS_ASSERT_ABORT(is_valid_name(replacement));
    bool mapped = item_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

void apply_item_dat_changes()
{
    //change_item("old_name", "new_name");

    // deletions
    const size_t buflen = 32;
    char name[buflen] = {'\0'};
    for (int i=1; i<=36; i++)
    {
        if (i == 7) continue;
        snprintf(name, buflen, "cell%d", i);
        name[buflen-1] = '\0';
        change_item(name, "regolith");
    }
    for (int i=1; i<=10; i++)
    {
        snprintf(name, buflen, "iceflame%02d", i);
        name[buflen-1] = '\0';
        change_item(name, "regolith");
    }

    change_item("terminal_blue", "regolith");
    change_item("terminal_green", "regolith");
    change_item("battery", "regolith");
    change_item("ruins_1", "rock");
    change_item("ruins_2", "rock");
    change_item("ruins_3", "rock");
    change_item("ruins_4", "rock");

    // renames
    change_item("space_tree_trunk", "space_tree_trunk1");
    change_item("leaves", "leaves1");
    change_item("leaves_red", "leaves3");
    change_item("raised_tile_gray",  "raised_tile1");
    change_item("raised_tile_blue",  "raised_tile2");
    change_item("raised_tile_green", "raised_tile3");
    change_item("raised_tile_red",   "raised_tile4");
    change_item("raised_tile1", "ruins_ceiling1");
    change_item("raised_tile2", "ruins_ceiling2");
    change_item("raised_tile3", "ruins_ceiling3");
    change_item("raised_tile4", "ruins_ceiling4");
    change_item("mushroom1", "mushroom_stem1");
    change_item("mushroom2", "mushroom_stem2");

    item_name_map->condense();  // finalize
}

void end_item_dat()
{
    item_attributes->done_loading();
    item_attributes->set_pretty_names();
    apply_item_dat_changes();
    verify_item_dat();
    save_item_names();
}

}   // Item
