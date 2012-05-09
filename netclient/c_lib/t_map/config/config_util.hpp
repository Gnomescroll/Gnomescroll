#pragma once


#if DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif

/*
    BLOCK DAT
*/

namespace t_map
{

int texture_alias(const char* spritesheet);
void cube_def(int id, int type, const char* name);
int sprite_def(int spritesheet, int xpos, int ypos);

#if DC_CLIENT
int _current_cube_id = 0;
int texture_alias(const char* spritesheet) 
{ 
    return LUA_load_cube_texture_sheet((char*) spritesheet); 
}

void cube_def(int id, int type, const char* name)
{ 
    _current_cube_id = id;
    set_cube_name(id, (char*) name);
}

int sprite_def(int spritesheet, int xpos, int ypos)
{
    return LUA_blit_cube_texture(spritesheet, xpos, ypos); 
}
#endif

#if DC_SERVER
int texture_alias(const char* spritesheet) {  return 0; }
void cube_def(int id, int type, const char* name) {}
int sprite_def(int spritesheet, int xpos, int ypos) {return 0;}
#endif

void end_block_dat()
{
    #if DC_CLIENT
    LUA_save_cube_texture();
    #endif
}

}   // t_map

/*
    DROP DAT
*/

namespace t_map
{

void end_drop_dat()
{

}

}   // t_map
