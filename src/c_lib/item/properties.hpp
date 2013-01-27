#pragma once

#include <common/dat/name_map.hpp>
#include <item/common/constants.hpp>
#include <t_mech/common/common.hpp>

namespace Item
{

extern class DatNameMap* item_name_map;
extern class ItemAttribute* item_attributes;

extern class SynthesizerItem* synthesizer_item_array;

extern class CraftingRecipe* crafting_recipe_array;
extern class SmeltingRecipe* smelting_recipe_array;

// buffers for condensing craft bench inputs to unique type,count pairs
extern int craft_input_types[CRAFT_BENCH_INPUTS_MAX];
extern int craft_input_totals[CRAFT_BENCH_INPUTS_MAX];

// buffers for recipe outputs available
extern class CraftingRecipe* craft_recipes_possible[CRAFT_BENCH_OUTPUTS_MAX];
extern int craft_recipes_possible_count;

void init_properties();

class ItemAttribute* get_item_attributes(int item_type);

bool type_used(int item_type);

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

const char* get_item_name(int item_type);
int get_item_type(const char* name);
const char* get_compatible_item_name(const char* name);

const char* get_item_pretty_name(int type);

ItemGroup get_item_group_for_type(int item_type);

bool item_type_is_voxel(int item_type);

int get_item_fire_rate(int item_type);

EquipmentType get_item_equipment_type(ItemID item_id);

int get_max_charges(int item_type);
int get_recharge_rate(int item_type);

int get_max_stack_size(int item_type);
int get_max_durability(int item_type);

int get_block_type_id(int item_type);
MechType get_mech_type(int item_type);

int get_particle_voxel_texture(int item_type);

int get_item_cube_height(int item_type);

float get_weapon_range(int weapon_type);
int get_item_block_damage(int weapon_type, CubeType cube_type);
int get_item_object_damage(int weapon_type);

int get_synthesizer_item(int xslot, int yslot);
int get_synthesizer_item(int xslot, int yslot, int* cost);

int get_gas_lifetime(int item_type);

bool is_fuel(int item_type);
bool is_smelter(ItemContainerType type);

int get_animation_id(int item_type);

int get_fuel_burn_rate(int item_type);
int get_smelting_recipe_creation_time(int recipe_id);

class CraftingRecipe* get_craft_recipe(int recipe_id);

class CraftingRecipe* get_selected_craft_recipe(ItemContainerID container_id, int slot);
int get_selected_craft_recipe_type(ItemContainerID container_id, int slot);
int get_selected_craft_recipe_type(ItemContainerID container_id, int slot, bool* available);
int get_selected_craft_recipe_stack(ItemContainerID container_id, int slot);

class SmeltingRecipe* get_smelting_recipe(int recipe_id);
class SmeltingRecipe* get_selected_smelting_recipe(ItemContainerID container_id);
int* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count);
int* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count, bool* available);
int* get_selected_smelting_recipe_stacks(ItemContainerID container_id, int* recipe_count);

ItemContainerType get_container_type_for_cube(CubeType cube_type);
}   // Item
