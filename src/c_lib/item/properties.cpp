/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/common/constants.hpp>
#include <item/config/item_attribute.hpp>
#include <item/config/crafting_dat.hpp>
#include <item/container/_interface.hpp>
#include <item/container/container.hpp>
#include <item/container/config/_interface.hpp>
#include <common/dat/modifiers.hpp>

namespace Item
{

class DatNameMap* item_name_map = NULL;
class ItemAttributes* item_attributes = NULL;
class SynthesizerItem* synthesizer_item_array = NULL;
class CraftingRecipe* crafting_recipe_array = NULL;
class SmeltingRecipe* smelting_recipe_array = NULL;

// buffers for condensing craft bench inputs to unique type,count pairs
int craft_input_types[CRAFT_BENCH_INPUTS_MAX];
int craft_input_totals[CRAFT_BENCH_INPUTS_MAX];
// buffers for recipe outputs available
class CraftingRecipe* craft_recipes_possible[CRAFT_BENCH_OUTPUTS_MAX];
int craft_recipes_possible_count = 0;

void init_properties()
{
    GS_ASSERT(item_name_map == NULL);
    item_name_map = new DatNameMap(256, DAT_NAME_MAX_LENGTH);

    GS_ASSERT(item_attributes == NULL);
    item_attributes = new ItemAttributes;

    GS_ASSERT(crafting_recipe_array == NULL);
    crafting_recipe_array = new CraftingRecipe[MAX_CRAFTING_RECIPE];
    GS_ASSERT(smelting_recipe_array == NULL);
    smelting_recipe_array = new SmeltingRecipe[MAX_SMELTING_RECIPE];

    GS_ASSERT(synthesizer_item_array == NULL);
    const int synth_slots = ItemContainer::get_container_alt_max_slots(ItemContainer::name::synthesizer);
    GS_ASSERT_ABORT(synth_slots > 0);
    if (synth_slots <= 0) return;
    synthesizer_item_array = new SynthesizerItem[synth_slots];
}

void teardown_properties()
{
    delete item_name_map;
    delete item_attributes;
    delete[] synthesizer_item_array;
    delete[] crafting_recipe_array;
    delete[] smelting_recipe_array;
}

class ItemAttribute* get_item_attributes(ItemType item_type)
{
    return item_attributes->get(item_type);
}

class ItemAttribute* get_item_attributes(const char* name)
{
    return item_attributes->get(name);
}

bool type_used(ItemType item_type)
{
    class ItemAttribute* attr = get_item_attributes(item_type);
    return (attr != NULL);
}

int get_item_fire_rate(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 8;
    return attr->firing_rate;
}

EquipmentType get_item_equipment_type(ItemType type)
{
    ItemAttribute* attr = get_item_attributes(type);
    IF_ASSERT(attr == NULL) return NULL_EQUIPMENT_TYPE;
    return attr->equipment_type;
}

EquipmentType get_item_equipment_type(ItemID id)
{
    ItemType type = get_item_type(id);
    return get_item_equipment_type(type);
}

int get_max_charges(ItemType item_type)
{
    class ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 0;
    return attr->max_charges;
}

int get_recharge_rate(ItemType item_type)
{
    class ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 1;
    return attr->recharge_rate;
}

int get_sprite_index_for_id(ItemID id)
{
    ItemType type = get_item_type(id);
    return get_sprite_index_for_type(type);
}

int get_sprite_index_for_type(ItemType type)
{
    class ItemAttribute* attr = get_item_attributes(type);
    IF_ASSERT(attr == NULL) return ERROR_SPRITE;
    return attr->sprite;
}

const char* get_item_name(ItemType type)
{
    class ItemAttribute* attr = get_item_attributes(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->name;
}

ItemType get_item_type(const char* name)
{
    if (name[0] == '\0') return NULL_ITEM_TYPE;
    for (int i=0; i<(int)MAX_ITEM_TYPES; i++)
    {
        class ItemAttribute* attr = get_item_attributes((ItemType)i);
        if (attr != NULL && strcmp(attr->name, name) == 0)
            return (ItemType)i;
    }
    GS_ASSERT(false);
    printf("No item for name %s\n", name);
    return NULL_ITEM_TYPE;
}

const char* get_compatible_item_name(const char* name)
{
    const char* mapname = item_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_item_type(name) != NULL_ITEM_TYPE) return name;
    return NULL;
}

const char* get_item_pretty_name(ItemType item_type)
{
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return NULL;
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL;
    const char* name = NULL;
    IF_ASSERT(attr->pretty_name == NULL)
        name = get_item_name(item_type); // use item name is no pretty name defined
    else name = attr->pretty_name;
    IF_ASSERT(name == NULL)
        name = "NULL";
    return name;
}

ItemGroup get_item_group_for_type(ItemType item_type)
{
    if (item_type == NULL_ITEM_TYPE) return IG_NONE;
    class ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return IG_ERROR;
    return attr->group;
}

const class ModifierList* get_item_modifiers(ItemType item_type)
{
    IF_ASSERT(!isValid(item_type)) return NULL;
    class ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL;
    return &attr->modifiers;
}

const class ModifierList* get_item_modifiers(ItemID item_id)
{
    ItemType item_type = get_item_type(item_id);
    return get_item_modifiers(item_type);
}

bool item_type_is_voxel(ItemType item_type)
{
    if (item_type == NULL_ITEM_TYPE) return false;
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return false;
    return attr->particle_voxel;
}

int get_max_stack_size(ItemType item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 1;
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 1;
    return attr->max_stack_size;
}

int get_max_durability(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL_DURABILITY;
    return attr->max_durability;
}

CubeType get_cube_type(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return ERROR_CUBE;
    GS_ASSERT(attr->cube_type != NULL_CUBE)
    return attr->cube_type;
}

MechType get_mech_type(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL_MECH_TYPE;
    GS_ASSERT(attr->mech_type != NULL_MECH_TYPE)
    return attr->mech_type;
}

int get_particle_voxel_texture(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 0;
    return attr->particle_voxel_texture;
}

int get_item_cube_height(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 0;
    return attr->cube_height;
}

float get_weapon_range(ItemType weapon_type)
{
    ItemAttribute* attr = get_item_attributes(weapon_type);
    IF_ASSERT(attr == NULL) return 1.0f;
    GS_ASSERT(attr->firing_range != NULL_FIRING_RANGE);
    return attr->firing_range;
}

int get_item_block_damage(ItemType weapon_type, CubeType cube_type)
{
    IF_ASSERT(!isValid(cube_type)) return 0;
    ItemAttribute* attr = get_item_attributes(weapon_type);
    IF_ASSERT(attr == NULL) return 0;
    return attr->block_damage[cube_type];
}

int get_item_object_damage(ItemType weapon_type)
{
    ItemAttribute* attr = get_item_attributes(weapon_type);
    IF_ASSERT(attr == NULL) return 0;
    if (attr->object_damage) return attr->object_damage;
    GS_ASSERT(attr->object_damage_min < attr->object_damage_max);
    return randrange(attr->object_damage_min, attr->object_damage_max);
}

ItemType get_synthesizer_item(int xslot, int yslot)
{
    int cost;
    return get_synthesizer_item(xslot, yslot, &cost);
}

ItemType get_synthesizer_item(int xslot, int yslot, int* cost)
{
    int max = ItemContainer::get_container_alt_max_slots(ItemContainer::name::synthesizer);
    for (int i=0; i<max; i++)
    {
        class SynthesizerItem* n = &synthesizer_item_array[i];
        if (n->xslot == xslot && n->yslot == yslot)
        {
            *cost = n->cost;
            return n->item_type;
        }
    }
    *cost = 0;
    return NULL_ITEM_TYPE;
}

int get_gas_lifetime(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL_GAS_LIFETIME;
    return attr->gas_lifetime;
}

bool is_fuel(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return false;
    return attr->fuel;
}

bool is_smelter(ItemContainerType type)
{
    return (type == ItemContainer::name::smelter_basic);
}

AnimationType get_animation_type(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return NULL_ANIMATION_TYPE;
    return attr->animation_type;
}

bool can_be_booned(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    if (attr == NULL) return false;
    return attr->boonable;
}

bool can_be_placed(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    if (attr == NULL) return false;
    return attr->placeable;
}

bool does_knockback(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    if (attr == NULL) return false;
    return attr->knockback;
}

// total ticks to burn
int get_fuel_burn_rate(ItemType item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 30;
    return attr->fuel_burn_rate;
}

int get_smelting_recipe_creation_time(int recipe_id)
{
    class SmeltingRecipe* recipe = get_smelting_recipe(recipe_id);
    IF_ASSERT(recipe == NULL) return DEFAULT_SMELTING_RECIPE_CREATION_TIME;
    return recipe->creation_time;
}

class CraftingRecipe* get_craft_recipe(int recipe_id)
{
    IF_ASSERT(recipe_id < 0 || recipe_id >= crafting_recipe_count) return NULL;
    return &crafting_recipe_array[recipe_id];
}

class CraftingRecipe* get_selected_craft_recipe(ItemContainerID container_id, int slot)
{
    // get container
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    if (container == NULL) return NULL;

    // clear input buffers
    for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; craft_input_types[i++] = NULL_ITEM_TYPE);
    for (int i=0; i<CRAFT_BENCH_INPUTS_MAX; craft_input_totals[i++] = 0);

    // condense container contents to unique types/counts
    int unique_inputs = 0;
    for (int i=0; i<container->slot_max; i++)
    {
        // get slot content data
        ItemID item_id = container->get_item(i);
        if (item_id == NULL_ITEM) continue;
        ItemType item_type = get_item_type(item_id);
        GS_ASSERT(item_type != NULL_ITEM_TYPE);    // item type should exist here, because we are skipping empty slots
        int stack_size = get_stack_size(item_id);
        GS_ASSERT(stack_size >= 1);

        // insert into type buffer
        if (unique_inputs == 0)
        {   // degenerate case
            craft_input_types[unique_inputs] = item_type;
            craft_input_totals[unique_inputs] = stack_size;
        }
        else
        {   // keep buffer sorted
            int i=0;
            for (; i<unique_inputs; i++)
            {
                if (craft_input_types[i] <= item_type) continue;

                // shift forward
                for (int j=unique_inputs; j>i; j--) craft_input_types[j] = craft_input_types[j-1];
                for (int j=unique_inputs; j>i; j--) craft_input_totals[j] = craft_input_totals[j-1];

                // insert
                craft_input_types[i] = item_type;
                craft_input_totals[i] = stack_size;
                break;
            }
            if (i == unique_inputs)
            {   // append to end
                craft_input_types[unique_inputs] = item_type;
                craft_input_totals[unique_inputs] = stack_size;
            }
        }
        unique_inputs++;
    }

    // no inputs
    if (unique_inputs == 0) return NULL;

    // reset outputs buffer
    for (int i=0; i<CRAFT_BENCH_OUTPUTS_MAX; craft_recipes_possible[i++] = NULL);
    craft_recipes_possible_count = 0;

    // iterate available recipes
    // if types match exactly, add recipe to available recipes

    for (int i=0; i<crafting_recipe_count; i++)
    {
        CraftingRecipe* recipe = &crafting_recipe_array[i];
        GS_ASSERT(recipe->output != NULL_ITEM_TYPE);
        GS_ASSERT(recipe->reagent_num > 0);
        // make sure to set availability state to default
        recipe->available = true;

        // only match exactly
        if (recipe->reagent_num != unique_inputs) continue;

        // check if reagents match inputs
        bool match = true;
        bool can_craft = true;
        for (int j=0; j<recipe->reagent_num; j++)
        {
            if (recipe->reagent[j] == craft_input_types[j])
            {   // check for reagent counts
                if (recipe->reagent_count[j] > craft_input_totals[j]) can_craft = false;
            }
            else
            {
                match = false;
                break;
            }
        }
        if (!match) continue;

        if (!can_craft) recipe->available = false;
        craft_recipes_possible[craft_recipes_possible_count] = recipe;
        craft_recipes_possible_count++;
    }

    // slot is out of recipe range
    if (craft_recipes_possible_count < slot) return NULL;

    return craft_recipes_possible[slot];
}

ItemType get_selected_craft_recipe_type(ItemContainerID container_id, int slot, bool* available)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    *available = recipe->available;
    return recipe->output;
}

ItemType get_selected_craft_recipe_type(ItemContainerID container_id, int slot)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    return recipe->output;
}

int get_selected_craft_recipe_stack(ItemContainerID container_id, int slot)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return 1;
    return recipe->output_stack;
}

class SmeltingRecipe* get_selected_smelting_recipe(ItemContainerID container_id)
{
    // get container
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    if (container == NULL) return NULL;
    IF_ASSERT(!is_smelter(container->type)) return NULL;
    ItemContainer::ItemContainerSmelter* smelter = (ItemContainer::ItemContainerSmelter*)container;

    unsigned int max_inputs = smelter->get_max_input_slots();
    IF_ASSERT(max_inputs <= 0) return NULL;
    //ItemID inputs[max_inputs];
    MALLOX(ItemID, inputs, max_inputs); //type, name, size

    int n_inputs = smelter->get_sorted_inputs(inputs, max_inputs);
    if (n_inputs <= 0) return NULL;

    //int input_types[n_inputs];
    //int input_stacks[n_inputs];
    MALLOX(int, input_types, n_inputs); //type, name, size
    MALLOX(int, input_stacks, n_inputs); //type, name, size

    for (int i=0; i<n_inputs; i++)
    {
        input_types[i] = get_item_type(inputs[i]);
        input_stacks[i] = get_stack_size(inputs[i]);
    }

    // iterate available recipes
    // if types match exactly, return recipe (only one recipe per combination for the smelter

    GS_ASSERT(smelting_recipe_count > 0);
    for (int i=0; i<smelting_recipe_count; i++)
    {
        SmeltingRecipe* recipe = &smelting_recipe_array[i];
        IF_ASSERT(recipe->output_num <= 0) continue;
        GS_ASSERT(recipe->output[0] != NULL_ITEM_TYPE);
        GS_ASSERT(recipe->reagent_num > 0);
        // make sure to set availability state to default
        recipe->available = true;

        // only match exactly
        if (recipe->reagent_num != n_inputs) continue;

        // check if reagents match inputs
        bool match = true;
        bool can_smelt = true;
        for (int j=0; j<recipe->reagent_num; j++)
        {
            GS_ASSERT(recipe->reagent[j] != NULL_ITEM_TYPE);
            if (recipe->reagent[j] == input_types[j])
            {   // check for reagent counts
                if (recipe->reagent_count[j] > input_stacks[j]) can_smelt = false;
            }
            else
            {
                match = false;
                break;
            }
        }
        if (!match) continue;

        if (!can_smelt) recipe->available = false;
        return recipe;
    }
    return NULL;
}

class SmeltingRecipe* get_smelting_recipe(int recipe_id)
{
    GS_ASSERT(recipe_id != NULL_SMELTING_RECIPE);
    IF_ASSERT(recipe_id < 0 || recipe_id >= smelting_recipe_count) return NULL;
    return &smelting_recipe_array[recipe_id];
}

ItemType* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count)
{
    bool available;
    return get_selected_smelting_recipe_types(container_id, recipe_count, &available);
}

ItemType* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count, bool* available)
{
    class SmeltingRecipe* recipe = get_selected_smelting_recipe(container_id);
    if (recipe == NULL)
    {
        *recipe_count = 0;
        return NULL;
    }
    *recipe_count = recipe->output_num;
    *available = recipe->available;
    return recipe->output;
}

int* get_selected_smelting_recipe_stacks(ItemContainerID container_id, int* recipe_count)
{
    class SmeltingRecipe* recipe = get_selected_smelting_recipe(container_id);
    if (recipe == NULL)
    {
        *recipe_count = 0;
        return NULL;
    }
    *recipe_count = recipe->output_num;
    return recipe->output_stack;
}

}   // Item
