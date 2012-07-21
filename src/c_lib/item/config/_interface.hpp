#pragma once

#include <item/common/enum.hpp>
#include <item/common/struct.hpp>

#if DC_CLIENT
#include <SDL/texture_sheet_loader.hpp>
#endif

#include <item/properties.hpp>

namespace Item
{

void start_item_dat();
void end_item_dat();

int texture_alias(const char* spritesheet);
void item_def(int id, ItemGroup group, const char* name);
void sprite_def(int spritesheet, int xpos, int ypos);
void sprite_def(int alias);
int sprite_alias(int spritesheet, int xpos, int ypos);
void _set_attribute();

void iso_block_sprite_def(const char* block_name);
void container_block_def(const char* block_name, ItemContainerType container_type);


class ItemAttribute s;

int _current_item_id = 0;

void item_def(int type, ItemGroup group, const char* name)
{
    GS_ASSERT(type != NULL_ITEM_TYPE);
    GS_ASSERT(type >= 0 && type < MAX_ITEMS);
    GS_ASSERT(group != IG_NONE);
    GS_ASSERT(name[0] != '\0');
    
    _set_attribute();

    // check that this type has not been set yet
    for (int i=0; i<MAX_ITEMS; i++)
        GS_ASSERT(item_attribute_array[i].item_type != type);

    _current_item_id = type;

    s.load_defaults(type, group);
    
    GS_ASSERT(group_array[type] == IG_NONE)
    group_array[type] = group;
    
    set_item_name(type, name);
}

void _set_attribute()
{
    item_attribute_array[_current_item_id] = s;
}

ItemContainerType _current_container_type = CONTAINER_TYPE_NONE;

void container_block_def(const char* block_name, ItemContainerType container_type)
{
    int val = t_map::get_cube_id((char*)block_name);
    GS_ASSERT(val >= 0 && val < t_map::MAX_CUBES);
    if (val < 0 || val >= t_map::MAX_CUBES) return;
    container_block_types[val] = container_type;
}

#if DC_CLIENT

int texture_alias(const char* spritesheet)
{
    return LUA_load_item_texture_sheet((char*) spritesheet);
}

void sprite_def(int spritesheet, int ypos, int xpos)
{
    GS_ASSERT(xpos >= 1 && ypos >= 1);
    // can't check maximums because they are unknown
    
    // check if we have already set this sprite
    GS_ASSERT(sprite_array[_current_item_id] == ERROR_SPRITE);

    int index = LUA_blit_item_texture(spritesheet, xpos, ypos);
    GS_ASSERT(index != 255);    // failure
    
    // check if we are already using this sprite
    if (index != ERROR_SPRITE)
        for (int i=0; i<MAX_ITEMS; i++) GS_ASSERT(sprite_array[i] != index);

    sprite_array[_current_item_id] = index; //check
}

#else
int texture_alias(const char* spritesheet) {return 0;}
void sprite_def(int spritesheet, int xpos, int ypos) {}
#endif

}   // Item


namespace Item
{

int crafting_recipe_count = 0;
class CraftingRecipe _cr;

void end_crafting_recipe();

void def_crafting_recipe(const char* item_name, int amount)
{
    if (_cr.reagent_num > 0) end_crafting_recipe();
    _cr.output = dat_get_item_type(item_name);
    _cr.output_stack = amount;
}

void def_crafting_recipe(const char* item_name)
{
    def_crafting_recipe(item_name, 1);
}

void set_crafting_reagent(const char* item_name, int quantity)
{
    GS_ASSERT(crafting_recipe_count < MAX_CRAFTING_RECIPE);
    if (crafting_recipe_count >= MAX_CRAFTING_RECIPE) return;
    
    GS_ASSERT(_cr.reagent_num < CRAFT_BENCH_INPUTS_MAX);
    if (_cr.reagent_num >= CRAFT_BENCH_INPUTS_MAX) return;

    int type = dat_get_item_type(item_name);
    GS_ASSERT(type != NULL_ITEM_TYPE);
    
    // Make sure we aren't adding two types of different stack values
    // Why? our sorting methods for doing recipe matches do not sort by
    // stack values per type, so there will be undefined behaviour
    for (int i=0; i<_cr.reagent_num; i++)
        GS_ASSERT(_cr.reagent[i] != type || _cr.reagent_count[i] == quantity);

    // insert reagents sorted by type
    if (_cr.reagent_num == 0)
    {   // degenerate case
        _cr.reagent[_cr.reagent_num] = type;
        _cr.reagent_count[_cr.reagent_num] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_cr.reagent_num; i++)
        {
            if (_cr.reagent[i] <= type) continue;

            // shift forward
            for (int j=_cr.reagent_num; j>i; j--) _cr.reagent[j] = _cr.reagent[j-1];
            for (int j=_cr.reagent_num; j>i; j--) _cr.reagent_count[j] = _cr.reagent_count[j-1];
            
            // insert
            _cr.reagent[i] = type;
            _cr.reagent_count[i] = quantity;
            break;
        }
        
        if (i == _cr.reagent_num)
        {   // append to end
            _cr.reagent[_cr.reagent_num] = type;
            _cr.reagent_count[_cr.reagent_num] = quantity;
        }
    }
    
    _cr.reagent_num++;
}

void end_crafting_recipe()
{
    GS_ASSERT(crafting_recipe_count <= MAX_CRAFTING_RECIPE);
    
    GS_ASSERT(_cr.reagent_num > 0);
    GS_ASSERT(_cr.output != NULL_ITEM_TYPE);
    GS_ASSERT(_cr.output_stack >= 1);
    for (int i=0; i<_cr.reagent_num; i++)
        GS_ASSERT(_cr.reagent[i] != NULL_ITEM_TYPE);
    
    // check that adding this recipe will not increase the total outputs
    // per recipe above limit
    int matching_recipes = 0;
    for (int i=0; i<crafting_recipe_count; i++)
    {
        if (crafting_recipe_array[i].reagent_num != _cr.reagent_num) continue;
        for (int j=0; j<crafting_recipe_array[i].reagent_num; j++)
        {
            if (crafting_recipe_array[i].reagent[j] != _cr.reagent[j]) break;
            matching_recipes++;
        }
    }
    GS_ASSERT(matching_recipes <= CRAFT_BENCH_OUTPUTS_MAX);
    
    // check that this recipe's type signature (input+output)
    // has not already been defined
    for (int i=0; i<crafting_recipe_count; i++)
    {
        if (crafting_recipe_array[i].reagent_num != _cr.reagent_num) continue;
        int j=0;
        for (; j<crafting_recipe_array[i].reagent_num; j++)
            if (crafting_recipe_array[i].reagent[j] != _cr.reagent[j]) break;
        if (j < crafting_recipe_array[i].reagent_num) continue;
        // reagents matched, check products
        GS_ASSERT(crafting_recipe_array[i].output != _cr.output);
    }
    
    // check if recursive recipe (reagents == output)
    if (_cr.reagent_num == 1) GS_ASSERT(_cr.reagent[0] != _cr.output);
    
    _cr.id = crafting_recipe_count;
    crafting_recipe_array[crafting_recipe_count] = _cr;
    _cr.init();
    crafting_recipe_count++;
}

}   // Item


namespace Item
{

int smelting_recipe_count = 0;
class SmeltingRecipe _sr;

void end_smelting_recipe();

void add_smelting_product(const char* item_name, int amount)
{
    GS_ASSERT(_sr.output_num < SMELTER_OUTPUTS_MAX);
    _sr.output[_sr.output_num] = dat_get_item_type(item_name);
    _sr.output_stack[_sr.output_num] = amount;
    _sr.output_num++;    
}

void add_smelting_product(const char* item_name)
{
    add_smelting_product(item_name, 1);
}

void def_smelting_recipe(const char* item_name, int amount)
{
    if (_sr.reagent_num > 0) end_smelting_recipe();
    add_smelting_product(item_name, amount);
}

void def_smelting_recipe(const char* item_name)
{
    if (_sr.reagent_num > 0) end_smelting_recipe();
    def_smelting_recipe(item_name, 1);
}

void set_smelting_reagent(const char* item_name, int quantity)
{
    GS_ASSERT(_sr.reagent_num < SMELTER_INPUTS_MAX);
    GS_ASSERT(_sr.output_num < SMELTER_OUTPUTS_MAX);

    int type = dat_get_item_type(item_name);
    GS_ASSERT(type != NULL_ITEM_TYPE);
    
    // Make sure we aren't adding two types of different stack values
    // Why? our sorting methods for doing recipe matches do not sort by
    // stack values per type, so there will be undefined behaviour
    for (int i=0; i<_sr.reagent_num; i++)
        GS_ASSERT(_sr.reagent[i] != type || _sr.reagent_count[i] == quantity);

    // insert reagents sorted by type
    if (_sr.reagent_num == 0)
    {   // degenerate case
        _sr.reagent[_sr.reagent_num] = type;
        _sr.reagent_count[_sr.reagent_num] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_sr.reagent_num; i++)
        {
            if (_sr.reagent[i] <= type) continue;

            // shift forward
            for (int j=_sr.reagent_num; j>i; j--) _sr.reagent[j] = _sr.reagent[j-1];
            for (int j=_sr.reagent_num; j>i; j--) _sr.reagent_count[j] = _sr.reagent_count[j-1];
            
            // insert
            _sr.reagent[i] = type;
            _sr.reagent_count[i] = quantity;
            break;
        }
        
        if (i == _sr.reagent_num)
        {   // append to end
            _sr.reagent[_sr.reagent_num] = type;
            _sr.reagent_count[_sr.reagent_num] = quantity;
        }
    }

    _sr.reagent_num++;
}

// in total ticks to synthesize
void set_smelting_creation_time(int creation_time)
{
    _sr.creation_time = creation_time;
}

void end_smelting_recipe()
{
    GS_ASSERT(smelting_recipe_count <= MAX_SMELTING_RECIPE);
    
    GS_ASSERT(_sr.reagent_num > 0);
    GS_ASSERT(_sr.output_num > 0);

    // make sure no other recipe has same type signature
    for (int i=0; i<smelting_recipe_count; i++)
    {
        if (smelting_recipe_array[i].reagent_num != _sr.reagent_num) continue;
        int j=0;
        for(; j<smelting_recipe_array[i].reagent_num; j++)
            if (smelting_recipe_array[i].reagent[j] != _sr.reagent[j])
                break;
        GS_ASSERT(j < smelting_recipe_array[i].reagent_num);
    }
    
    // check that inputs != outputs
    if (_sr.reagent_num == _sr.output_num)
    {
        int i=0;
        for (; i<_sr.reagent_num; i++)
            if (_sr.reagent[i] != _sr.output[i]) break;
        GS_ASSERT(i < _sr.reagent_num);
    }
    
    _sr.reagent_num = _sr.reagent_num;
    _sr.id = smelting_recipe_count;
    smelting_recipe_array[smelting_recipe_count] = _sr;
    _sr.init();
    smelting_recipe_count++;
}

}   // Item


namespace Item
{

int _current_synthesizer_item = 0;
int _current_synthesizer_item_type = NULL_ITEM_TYPE;
int _current_synthesizer_item_cost = NULL_COST;

void synthesizer_item_def(const char* item_name, int cost);
void synthesizer_item_set(int xslot, int yslot);


void synthesizer_item_def(const char* item_name, int cost)
{
    GS_ASSERT(cost != NULL_COST && cost > 0 && count_digits(cost) <= SYNTHESIZER_ITEM_COST_MAX_STRLEN);
    _current_synthesizer_item_type = dat_get_item_type(item_name);
    _current_synthesizer_item_cost = cost;
}

void synthesizer_item_set(int xslot, int yslot)
{
    GS_ASSERT(_current_synthesizer_item < MAX_SYNTHESIZER_OUTPUTS);

    class SynthesizerItem* n = &synthesizer_item_array[_current_synthesizer_item];

    GS_ASSERT(_current_synthesizer_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(xslot >= 0 && xslot < AGENT_SYNTHESIZER_SHOPPING_X);
    GS_ASSERT(yslot >= 0 && yslot < AGENT_SYNTHESIZER_SHOPPING_Y);
    GS_ASSERT(_current_synthesizer_item_cost > 0);
    GS_ASSERT(count_digits(_current_synthesizer_item_cost) <= SYNTHESIZER_ITEM_COST_MAX_STRLEN);

    // make sure we are not defining a purchase twice for the same item
    // or overwriting an existing purchase slot
    for (int i=0; i<_current_synthesizer_item; i++)
    {
        GS_ASSERT(synthesizer_item_array[i].item_type != _current_synthesizer_item_type);
        GS_ASSERT(synthesizer_item_array[i].xslot != xslot || synthesizer_item_array[i].yslot != yslot);
    }

    n->item_type = _current_synthesizer_item_type;
    n->cost = _current_synthesizer_item_cost;
    n->xslot = xslot;
    n->yslot = yslot;

    _current_synthesizer_item_type = NULL_ITEM_TYPE;
    _current_synthesizer_item_cost = NULL_COST;
    _current_synthesizer_item++;
}

}   // Item
