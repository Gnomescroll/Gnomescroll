#pragma once

#include <common/dat/name_map.hpp>
#include <item/common/constants.hpp>
#include <t_mech/common/common.hpp>
#include <animations/constants.hpp>

namespace Item
{

extern class DatNameMap* item_name_map;
extern class ItemAttributes* item_attributes;

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

class ItemAttribute* get_item_attributes(ItemType item_type);

bool type_used(ItemType item_type);

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(ItemType type);

const char* get_item_name(ItemType item_type);
ItemType get_item_type(const char* name);
const char* get_compatible_item_name(const char* name);

const char* get_item_pretty_name(ItemType type);

ItemGroup get_item_group_for_type(ItemType item_type);

const class ModifierList* get_item_modifiers(ItemType item_type);
const class ModifierList* get_item_modifiers(ItemID item_id);

bool item_type_is_voxel(ItemType item_type);

int get_item_fire_rate(ItemType item_type);

EquipmentType get_item_equipment_type(ItemID item_id);

int get_max_charges(ItemType item_type);
int get_recharge_rate(ItemType item_type);

int get_max_stack_size(ItemType item_type);
int get_max_durability(ItemType item_type);

CubeType get_cube_type(ItemType item_type);
MechType get_mech_type(ItemType item_type);

int get_particle_voxel_texture(ItemType item_type);

int get_item_cube_height(ItemType item_type);

float get_weapon_range(ItemType weapon_type);
int get_item_block_damage(ItemType weapon_type, CubeType cube_type);
int get_item_object_damage(ItemType weapon_type);

ItemType get_synthesizer_item(int xslot, int yslot);
ItemType get_synthesizer_item(int xslot, int yslot, int* cost);

int get_gas_lifetime(ItemType item_type);

bool is_fuel(ItemType item_type);
bool is_smelter(ItemContainerType type);

AnimationType get_animation_type(ItemType item_type);

bool can_be_booned(ItemType item_type);
bool can_be_placed(ItemType item_type);

int get_fuel_burn_rate(ItemType item_type);
int get_smelting_recipe_creation_time(int recipe_id);

class CraftingRecipe* get_craft_recipe(int recipe_id);

class CraftingRecipe* get_selected_craft_recipe(ItemContainerID container_id, int slot);
ItemType get_selected_craft_recipe_type(ItemContainerID container_id, int slot);
ItemType get_selected_craft_recipe_type(ItemContainerID container_id, int slot, bool* available);
int get_selected_craft_recipe_stack(ItemContainerID container_id, int slot);

class SmeltingRecipe* get_smelting_recipe(int recipe_id);
class SmeltingRecipe* get_selected_smelting_recipe(ItemContainerID container_id);
ItemType* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count);
ItemType* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count, bool* available);
int* get_selected_smelting_recipe_stacks(ItemContainerID container_id, int* recipe_count);

ItemContainerType get_container_type_for_cube(CubeType cube_type);
}   // Item
