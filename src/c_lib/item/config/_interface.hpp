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

void set_attribute();

void iso_block_sprite_def(const char* block_name);
void container_block_def(const char* block_name, ItemContainerType container_type);


class ItemAttribute s;

int _current_item_id = 0;

void item_def(int type, ItemGroup group, const char* name)
{
    if(type != 0) _set_attribute(); //assumes first type defined is 0

    _current_item_id = type;

    s.load_defaults(group);
    
    if(group_array[type] != IG_ERROR)
    {
        printf("ITEM CONFIG ERROR: item type conflict, type= %i \n", type);
        GS_ABORT();
    }
    group_array[type] = group; //check
    
    set_item_name(type, (char*) name);
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
    if(xpos < 1 || ypos < 1)
    {
        printf("ITEM CONFIG ERROR: id= %i xpos,ypos less than one \n", _current_item_id);
        GS_ASSERT(false);
    }

    // check if we have already set this sprite
    GS_ASSERT(sprite_array[_current_item_id] == ERROR_SPRITE);

    int index = LUA_blit_item_texture(spritesheet, xpos, ypos);
    GS_ASSERT(index != 255);    // failure
    
    // check if we are already using this sprite
    if (index != ERROR_SPRITE)
        for (int i=0; i<MAX_ITEMS; i++) GS_ASSERT(sprite_array[i] != index);

    sprite_array[_current_item_id] = index; //check
}

void sprite_def(int alias)
{
    sprite_array[_current_item_id] = alias;
}

int sprite_alias(int spritesheet, int ypos, int xpos)
{
    if(xpos < 1 || ypos < 1)
    {
        printf("ITEM CONFIG ERROR: sprite alias xpos,ypos less than zero \n");
        GS_ASSERT(false);
    }
    return LUA_blit_item_texture(spritesheet, xpos, ypos);
}
#else
int texture_alias(const char* spritesheet) {return 0;}
void sprite_def(int spritesheet, int xpos, int ypos) {}
void sprite_def(int alias) {}
int sprite_alias(int spritesheet, int xpos, int ypos) { return 0; }
#endif

}   // Item


namespace Item
{

int crafting_recipe_count = 0;
int _current_reagent_id = 0;

class CraftingRecipe _cr;

void def_recipe(const char* item_name, int amount)
{
    _cr.output[_cr.output_num] = dat_get_item_type(item_name);
    _cr.output_stack[_cr.output_num] = amount;
    _cr.output_num++;
}

void def_recipe(const char* item_name)
{
    def_recipe(item_name, 1);
}


void set_reagent(const char* item_name, int quantity)
{
    GS_ASSERT(_current_reagent_id < CRAFT_BENCH_INPUTS_MAX);

    int type = dat_get_item_type(item_name);
    

    // require specifying item,quantity at once
    //for (int i=0; i<_current_reagent_id; i++)
        //GS_ASSERT(_cr.reagent[i] != type);
    
    // insert reagents sorted by type
    if (_current_reagent_id == 0)
    {   // degenerate case
        _cr.reagent[_current_reagent_id] = type;
        _cr.reagent_count[_current_reagent_id] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_current_reagent_id; i++)
        {
            if (_cr.reagent[i] <= type) continue;

            // shift forward
            for (int j=_current_reagent_id; j>i; j--) _cr.reagent[j] = _cr.reagent[j-1];
            for (int j=_current_reagent_id; j>i; j--) _cr.reagent_count[j] = _cr.reagent_count[j-1];
            
            // insert
            _cr.reagent[i] = type;
            _cr.reagent_count[i] = quantity;
            break;
        }
        
        if (i == _current_reagent_id)
        {   // append to end
            _cr.reagent[_current_reagent_id] = type;
            _cr.reagent_count[_current_reagent_id] = quantity;
        }
    }

    _current_reagent_id++;
}

void end_recipe()
{
    _cr.reagent_num = _current_reagent_id;
    _cr.id = crafting_recipe_count;
    crafting_recipe_array[crafting_recipe_count] = _cr;
    _cr.init();
    crafting_recipe_count++;
    _current_reagent_id = 0;
}

}   // Item


namespace Item
{

/*
    int item_id;
    int nanite_cost;

    int level;
    int xslot;
    int yslot;
*/

int _current_nanite_item_type = 0;
int _current_nanite_item_cost = 0;

void nanite_item_def(const char* item_name, int cost);
void nanite_item_set(int level, int xslot, int yslot);


void nanite_item_def(const char* item_name, int cost)
{
    _current_nanite_item_type = dat_get_item_type(item_name);
    _current_nanite_item_cost = cost;
}

void nanite_item_set(int level, int xslot, int yslot)
{
    class NaniteStoreItem* n = &nanite_store_item_array[_current_nanite_item_type];

    n->item_type = _current_nanite_item_type;
    n->nanite_cost = _current_nanite_item_cost;
    n->level = level;
    n->xslot = xslot;
    n->yslot = yslot;

    _current_nanite_item_type++;
}

}   // Item
