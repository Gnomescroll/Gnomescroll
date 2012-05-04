#pragma once

#include <item/common/enum.hpp>

#if DC_CLIENT
#include <SDL/texture_sheet_loader.hpp>
#endif

#include <item/properties.hpp>

namespace Item
{

int texture_alias(const char* spritesheet);
void item_def(int id, int group, const char* name);
void sprite_def(int spritesheet, int xpos, int ypos);
void sprite_def(int alias);
int sprite_alias(int spritesheet, int xpos, int ypos);
void _set_attribute();

void set_attribute();
void end_item_dat();

class ItemAttribute s;



int _current_item_id = 0;

void item_def(int type, int group, const char* name)
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

void end_item_dat()
{
    _set_attribute();
    
    #if DC_CLIENT
    LUA_save_item_texture();
    #endif
}

#if DC_CLIENT

int texture_alias(const char* spritesheet)
{
    return LUA_load_item_texture_sheet((char*) spritesheet);
}

void sprite_def(int spritesheet, int ypos, int xpos)
{
    ypos -= 1;
    xpos -= 1;

    if(xpos < 0 || ypos < 0)
    {
        printf("ITEM CONFIG ERROR: id= %i xpos,ypos less than zero \n", _current_item_id);
        assert(false);
    }

    int index = LUA_blit_item_texture(spritesheet, xpos, ypos);
    sprite_array[_current_item_id] = index; //check
}

void sprite_def(int alias)
{
    sprite_array[_current_item_id] = alias;
}

int sprite_alias(int spritesheet, int ypos, int xpos)
{
    ypos -= 1;
    xpos -= 1;

    if(xpos < 0 || ypos < 0)
    {
        printf("ITEM CONFIG ERROR: sprite alias xpos,ypos less than zero \n");
        assert(false);
    }
    return LUA_blit_item_texture(spritesheet, xpos, ypos);
}
#else
int texture_alias(const char* spritesheet) {return 0;}
void sprite_def(int spritesheet, int xpos, int ypos) {}
void sprite_def(int alias) {}
int sprite_alias(int spritesheet, int xpos, int ypos) { return 0; }
#endif

}
