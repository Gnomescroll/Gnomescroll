#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/common/constants.hpp>
#include <item/common/struct.hpp>

#include <item/config/item_attribute.hpp>
#include <item/config/crafting_dat.hpp>

#include <item/container/_interface.hpp>
#include <item/container/container.hpp>
#include <item/container/config/_interface.hpp>

namespace Item
{

class DatNameMap* item_name_map = NULL;
class ItemAttribute* item_attributes = NULL;
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
    item_attributes = new ItemAttribute[MAX_ITEM_TYPES];

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
    if (item_name_map          != NULL) delete item_name_map;
    if (item_attributes        != NULL) delete[] item_attributes;
    if (synthesizer_item_array != NULL) delete[] synthesizer_item_array;
    if (crafting_recipe_array  != NULL) delete[] crafting_recipe_array;
    if (smelting_recipe_array  != NULL) delete[] smelting_recipe_array;
}

class ItemAttribute* get_item_attributes(int item_type)
{
    ASSERT_VALID_ITEM_TYPE(item_type);
    IF_INVALID_ITEM_TYPE(item_type) return NULL;
    if (!item_attributes[item_type].loaded) return NULL;
    return &item_attributes[item_type];
}

bool type_used(int item_type)
{
    class ItemAttribute* attr = get_item_attributes(item_type);
    return (attr != NULL);
}

int get_item_fire_rate(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 8;
    ItemAttribute* attr = get_item_attributes(item_type);
    IF_ASSERT(attr == NULL) return 8;
    return attr->firing_rate;
}

int get_sprite_index_for_id(ItemID id)
{
    GS_ASSERT(isValid(id));
    int type = get_item_type(id);
    return get_sprite_index_for_type(type);
}

int get_sprite_index_for_type(int type)
{
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    class ItemAttribute* attr = get_item_attributes(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return ERROR_SPRITE;
    return attr->sprite;
}

const char* get_item_name(int type)
{
    class ItemAttribute* attr = get_item_attributes(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL;
    return attr->name;
}

int get_item_type(const char* name)
{
    if (name[0] == '\0') return NULL_ITEM_TYPE;
    for (int i=0; i<MAX_ITEM_TYPES; i++)
    {
        class ItemAttribute* attr = get_item_attributes(i);
        if (attr != NULL && strcmp(attr->name, name) == 0)
            return i;
    }
    GS_ASSERT(false);
    printf("In function %s:%d -- No item for name %s\n", __FUNCTION__, __LINE__, name);
    return NULL_ITEM_TYPE;
}

const char* get_compatible_item_name(const char* name)
{
    const char* mapname = item_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_item_type(name) != NULL_ITEM_TYPE) return name;
    return NULL;
}

const char* get_item_pretty_name(int item_type)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return NULL;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL;
    GS_ASSERT(attr->pretty_name != NULL);
    const char* name = NULL;
    if (attr->pretty_name == NULL) name = get_item_name(item_type); // use item name is no pretty name defined
    else name = attr->pretty_name;
    GS_ASSERT(name != NULL);
    return name;
}

ItemGroup get_item_group_for_type(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return IG_NONE;
    class ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return IG_ERROR;
    return attr->group;
}

bool item_type_is_voxel(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return false;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;
    return attr->particle_voxel;
}

int get_max_stack_size(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 1;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 1;
    return attr->max_stack_size;
}

int get_max_durability(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return NULL_DURABILITY;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_DURABILITY;
    return attr->max_durability;
}

int get_block_type_id(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    GS_ASSERT(attr->cube_type != -1)
    return attr->cube_type;
}

MechType get_mech_type(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return NULL_MECH_TYPE;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_MECH_TYPE;
    GS_ASSERT(attr->mech_type != NULL_MECH_TYPE)
    return attr->mech_type;
}

int get_particle_voxel_texture(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->particle_voxel_texture;
}

int get_item_cube_height(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->cube_height;
}

float get_weapon_range(int weapon_type)
{
    if (weapon_type == NULL_ITEM_TYPE) return 1.0f;
    ItemAttribute* attr = get_item_attributes(weapon_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 1.0f;
    GS_ASSERT(attr->firing_range != NULL_FIRING_RANGE);
    return attr->firing_range;
}

int get_item_block_damage(int weapon_type, CubeType cube_type)
{
    ASSERT_VALID_CUBE_TYPE(cube_type);
    IF_INVALID_CUBE_TYPE(cube_type) return 0;
    if (weapon_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(weapon_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->block_damage[cube_type];
}

int get_item_object_damage(int weapon_type)
{
    if (weapon_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(weapon_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    if (attr->object_damage) return attr->object_damage;
    GS_ASSERT(attr->object_damage_min < attr->object_damage_max);
    return randrange(attr->object_damage_min, attr->object_damage_max);
}

int get_synthesizer_item(int xslot, int yslot)
{
    int cost;
    return get_synthesizer_item(xslot, yslot, &cost);
}

int get_synthesizer_item(int xslot, int yslot, int* cost)
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

int get_gas_lifetime(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return NULL_GAS_LIFETIME;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_GAS_LIFETIME;
    return attr->gas_lifetime;
}

bool is_fuel(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return false;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;
    return attr->fuel;
}

bool is_smelter(ItemContainerType type)
{
    if (type == ItemContainer::name::smelter_basic) return true;
    return false;
}

int get_animation_id(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->animation_id;
}

// total ticks to burn
int get_fuel_burn_rate(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 30;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 30;
    return attr->fuel_burn_rate;
}

int get_smelting_recipe_creation_time(int recipe_id)
{
    GS_ASSERT(recipe_id != NULL_SMELTING_RECIPE);
    if (recipe_id == NULL_SMELTING_RECIPE) return DEFAULT_SMELTING_RECIPE_CREATION_TIME;
    class SmeltingRecipe* recipe = get_smelting_recipe(recipe_id);
    GS_ASSERT(recipe != NULL);
    if (recipe == NULL) return DEFAULT_SMELTING_RECIPE_CREATION_TIME;
    return recipe->creation_time;
}

class CraftingRecipe* get_craft_recipe(int recipe_id)
{
    GS_ASSERT(recipe_id >= 0 && recipe_id < crafting_recipe_count);
    if (recipe_id < 0 || recipe_id >= crafting_recipe_count) return NULL;
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
        int item_type = get_item_type(item_id);
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

int get_selected_craft_recipe_type(ItemContainerID container_id, int slot, bool* available)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    *available = recipe->available;
    return recipe->output;
}

int get_selected_craft_recipe_type(ItemContainerID container_id, int slot)
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
    GS_ASSERT(is_smelter(container->type));
    if (!is_smelter(container->type)) return NULL;
    ItemContainer::ItemContainerSmelter* smelter = (ItemContainer::ItemContainerSmelter*)container;

    unsigned int max_inputs = smelter->get_max_input_slots();
    GS_ASSERT(max_inputs > 0);
    if (max_inputs <= 0) return NULL;
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
        GS_ASSERT(recipe->output_num > 0);
        if (recipe->output_num <= 0) continue;
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
    GS_ASSERT(recipe_id >= 0 && recipe_id < smelting_recipe_count);
    if (recipe_id < 0 || recipe_id >= smelting_recipe_count) return NULL;
    return &smelting_recipe_array[recipe_id];
}

int* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count)
{
    bool available;
    return get_selected_smelting_recipe_types(container_id, recipe_count, &available);
}

int* get_selected_smelting_recipe_types(ItemContainerID container_id, int* recipe_count, bool* available)
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
