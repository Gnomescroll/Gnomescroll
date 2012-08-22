#pragma once

#include <item/common/constant.hpp>

namespace Item
{

extern int sprite_array[MAX_ITEMS];
extern int type_array[MAX_ITEMS];
extern ItemGroup group_array[MAX_ITEMS];
extern class ItemAttribute* item_attribute_array;

extern class SynthesizerItem* synthesizer_item_array;

extern class CraftingRecipe* crafting_recipe_array;
extern class SmeltingRecipe* smelting_recipe_array;

extern ItemContainerType container_block_types[t_map::MAX_CUBES]; // maps block value -> container type

// buffers for condensing craft bench inputs to unique type,count pairs
extern int craft_input_types[CRAFT_BENCH_INPUTS_MAX];
extern int craft_input_totals[CRAFT_BENCH_INPUTS_MAX];

// buffers for recipe outputs available
extern class CraftingRecipe* craft_recipes_possible[CRAFT_BENCH_OUTPUTS_MAX];
extern int craft_recipes_possible_count;

void init_properties();

class ItemAttribute* get_item_attributes(int item_type);
int get_item_fire_rate(int item_type);

int get_sprite_index_for_id(ItemID id);
int get_sprite_index_for_type(int type);

void set_item_name(int type, const char* name, int length);
void set_item_name(int type, const char* name);

char* get_item_name(int item_type);
int get_item_type(const char* name);

char* get_item_pretty_name(int type);

ItemGroup get_item_group_for_type(int item_type);

int dat_get_item_type(const char* name);

bool item_type_is_voxel(int item_type);

int get_max_stack_size(int item_type);
int get_max_energy(int item_type);
int get_max_durability(int item_type);

int get_placer_block_type_id(int item_type);
int get_particle_voxel_texture(int item_type);

int get_item_cube_height(int item_type);

float get_weapon_range(int weapon_type);
int get_item_block_damage(int weapon_type, int block_type);
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

class CraftingRecipe* get_selected_craft_recipe(int container_id, int slot);
int get_selected_craft_recipe_type(int container_id, int slot);
int get_selected_craft_recipe_type(int container_id, int slot, bool* available);
int get_selected_craft_recipe_stack(int container_id, int slot);

class SmeltingRecipe* get_smelting_recipe(int recipe_id);
class SmeltingRecipe* get_selected_smelting_recipe(int container_id);
int* get_selected_smelting_recipe_types(int container_id, int* recipe_count);
int* get_selected_smelting_recipe_types(int container_id, int* recipe_count, bool* available);
int* get_selected_smelting_recipe_stacks(int container_id, int* recipe_count);

ItemContainerType get_container_type_for_block(int block_value);

}   // Item
