#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/common/constant.hpp>
#include <item/common/struct.hpp>

#include <item/config/item_attribute.hpp>
#include <item/config/crafting_dat.hpp>

#include <item/container/_interface.hpp>
#include <item/container/container.hpp>
#include <item/container/config/_interface.hpp>

namespace Item
{

int sprite_array[MAX_ITEMS]; //maps item id to sprite
ItemGroup group_array[MAX_ITEMS];
class ItemAttribute* item_attribute_array = NULL;
class SynthesizerItem* synthesizer_item_array = NULL;
class CraftingRecipe* crafting_recipe_array = NULL;
class SmeltingRecipe* smelting_recipe_array = NULL;

ItemContainerType container_block_types[t_map::MAX_CUBES];    // maps block value -> container type

// buffers for condensing craft bench inputs to unique type,count pairs
int craft_input_types[CRAFT_BENCH_INPUTS_MAX];
int craft_input_totals[CRAFT_BENCH_INPUTS_MAX];
// buffers for recipe outputs available
class CraftingRecipe* craft_recipes_possible[CRAFT_BENCH_OUTPUTS_MAX];
int craft_recipes_possible_count = 0;

const int ITEM_NAME_MAX_LENGTH = 64;
char item_names[MAX_ITEMS*ITEM_NAME_MAX_LENGTH] = {'\0'};
int item_name_index[MAX_ITEMS] = {-1};

void init_properties()
{
    for (int i=0; i<MAX_ITEMS; sprite_array[i++] = ERROR_SPRITE);
    for (int i=0; i<MAX_ITEMS; group_array[i++] = IG_NONE);
    
    for (int i=0; i<t_map::MAX_CUBES; container_block_types[i++] = CONTAINER_TYPE_NONE);

    GS_ASSERT(item_attribute_array == NULL);
    item_attribute_array = new ItemAttribute[MAX_ITEMS];
    GS_ASSERT(synthesizer_item_array == NULL);
    synthesizer_item_array = new SynthesizerItem[ItemContainer::get_container_alt_max_slots(AGENT_SYNTHESIZER)];

    crafting_recipe_array = new CraftingRecipe[MAX_CRAFTING_RECIPE];
    smelting_recipe_array = new SmeltingRecipe[MAX_SMELTING_RECIPE];
}

void tear_down_properties()
{
    if (item_attribute_array    != NULL) delete[] item_attribute_array;
    if (synthesizer_item_array != NULL) delete[] synthesizer_item_array;
    if (crafting_recipe_array != NULL) delete[] crafting_recipe_array;
    if (smelting_recipe_array != NULL) delete[] smelting_recipe_array;
}

class ItemAttribute* get_item_attributes(int item_type)
{
    GS_ASSERT(item_type == NULL_ITEM_TYPE || (item_type >= 0 && item_type < MAX_ITEMS));
    if (item_type < 0 || item_type >= MAX_ITEMS) return NULL;
    return &item_attribute_array[item_type];
}

int get_item_fire_rate(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 8;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    return attr->firing_rate;
}

int get_sprite_index_for_id(ItemID id)
{
    GS_ASSERT(id < MAX_ITEMS && id >= 0);
    int type = get_item_type(id);
    return get_sprite_index_for_type(type);
}

int get_sprite_index_for_type(int type)
{
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    GS_ASSERT(type >= 0 && type < MAX_ITEMS);
    if (type < 0 || type >= MAX_ITEMS) return ERROR_SPRITE;
    return sprite_array[type];
}

//Names

void set_item_name(int id, const char* name, int length)
{
    GS_ASSERT(length > 0);
    GS_ASSERT(length < ITEM_NAME_MAX_LENGTH);
    GS_ASSERT(id >= 0 || id < MAX_ITEMS);    
    
    if (length <= 0 || length >= ITEM_NAME_MAX_LENGTH) return;
    if (id < 0 || id >= MAX_ITEMS) return;

    static int str_index = 0;

    for (int i=0; i<MAX_ITEMS; i++)    // no duplicate names
        if (item_name_index[i] >= 0)
            GS_ASSERT(strcmp(item_names+item_name_index[i], name));

    item_name_index[id] = str_index;

    memcpy(item_names+str_index, name, length);
    str_index += length;
    item_names[str_index++] = '\0';
}

void set_item_name(int id, const char* name)
{
    int length = (int)strlen(name);
    set_item_name(id, name, length);
}

char* get_item_name(int type)
{
    GS_ASSERT(type >= 0 || type < MAX_ITEMS);
    if (type < 0 || type >= MAX_ITEMS) return NULL;
    GS_ASSERT(item_name_index[type] >= 0);
    if (item_name_index[type] < 0) return NULL;
    GS_ASSERT(item_name_index[type] < MAX_ITEMS*ITEM_NAME_MAX_LENGTH);
    if (item_name_index[type] >= MAX_ITEMS*ITEM_NAME_MAX_LENGTH) return NULL;
    
    return (item_names + item_name_index[type]);
}

int get_item_type(const char* name)
{
    for (int i=0; i<MAX_ITEMS; i++)
    {
        char* cmp_name = get_item_name(i);
        if (cmp_name != NULL && strcmp(name, cmp_name) == 0)
            return i;
    }
    GS_ASSERT(false);
    printf("In function %s:%d -- No item for name %s\n", __FUNCTION__, __LINE__, name);
    return NULL_ITEM_TYPE;
}

char* get_item_pretty_name(int item_type)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return NULL;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL;
    GS_ASSERT(attr->pretty_name != NULL);
    char* name = NULL;
    if (attr->pretty_name == NULL) name = get_item_name(item_type); // use item name is no pretty name defined
    else name = attr->pretty_name;
    GS_ASSERT(name != NULL);
    return name;
}

ItemGroup get_item_group_for_type(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return IG_NONE;
    GS_ASSERT(item_type >= 0 && item_type < MAX_ITEMS);
    if (item_type < 0 || item_type >= MAX_ITEMS) return IG_ERROR;
    return group_array[item_type];
}

int dat_get_item_type(const char* name)
{
    int type = get_item_type(name);
    GS_ASSERT(type != NULL_ITEM_TYPE);
    return type;
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

int get_max_energy(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return NULL_ENERGY;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_ENERGY;
    return attr->max_energy;
}

int get_max_durability(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return NULL_DURABILITY;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_DURABILITY;
    return attr->max_durability;
}

int get_placer_block_type_id(int item_type)
{
    if (item_type == NULL_ITEM_TYPE) return 0;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->placer_block_type_id;
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

int get_item_block_damage(int weapon_type, int block_type)
{
    if (weapon_type == NULL_ITEM_TYPE) return 0;
    // TODO -- block damage dependent on (weapon,block)
    ItemAttribute* attr = get_item_attributes(weapon_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->block_damage;
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
    int max = ItemContainer::get_container_alt_max_slots(AGENT_SYNTHESIZER);
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
    if (type == CONTAINER_TYPE_SMELTER_ONE) return true;
    return false;
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

class CraftingRecipe* get_selected_craft_recipe(int container_id, int slot)
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

int get_selected_craft_recipe_type(int container_id, int slot, bool* available)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    *available = recipe->available;
    return recipe->output;
}

int get_selected_craft_recipe_type(int container_id, int slot)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return NULL_ITEM_TYPE;
    return recipe->output;
}

int get_selected_craft_recipe_stack(int container_id, int slot)
{
    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, slot);
    if (recipe == NULL) return 1;
    return recipe->output_stack;
}

class SmeltingRecipe* get_selected_smelting_recipe(int container_id)
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

int* get_selected_smelting_recipe_types(int container_id, int* recipe_count)
{
    bool available;
    return get_selected_smelting_recipe_types(container_id, recipe_count, &available);
}

int* get_selected_smelting_recipe_types(int container_id, int* recipe_count, bool* available)
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

int* get_selected_smelting_recipe_stacks(int container_id, int* recipe_count)
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

ItemContainerType get_container_type_for_block(int block_value)
{
    GS_ASSERT(block_value >= 0 && block_value < t_map::MAX_CUBES);
    if (block_value < 0 || block_value >= t_map::MAX_CUBES) return CONTAINER_TYPE_NONE;
    return container_block_types[block_value];
}


}   // Item
