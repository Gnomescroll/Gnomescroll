#pragma once


#if DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif

/*
    BLOCK DAT
*/

namespace t_map
{
enum CubeType
{
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
};

/*
struct cubeProperties 
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop; //does block drop items
    bool item_container;  //inventory and crafting bench blocks
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;
    unsigned char color_type; //flat, discrete and perlin
    //bool solid;
    //bool gravity;
};
*/

/*
enum CubeType
{
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
};
*/

int _current_cube_id = 0;

void cube_def(int id, int type, const char* name)
{ 
    _current_cube_id = id;
    set_cube_name(id, (char*) name);


    struct cubeProperties p;
    p.active = true;
    p.solid = true;
    p.occludes = true;
    p.transparent = false;
    p.item_drop = false;
    p.item_container = false;

    p.max_damage = 32;
    p.color_type = 0;

    switch(type)
    {
    	case ErrorBlock:
    	break;

        case EmptyBlock:
        p.active = false;
        p.solid = false;
        p.occludes = false;
        break;
    	case SolidBlock:
    	break;

    	case ItemContainerBlock:
    	p.item_container = true;
    	break;

    	default:
    	GS_ABORT();
    }


    //extern struct cubeProperties* cube_list;
    cube_list[id] = p;
}

void iso_texture(int tex_id)
{
#ifdef DC_CLIENT
	set_cube_side_texture(_current_cube_id, 0, tex_id);
	set_cube_side_texture(_current_cube_id, 1, tex_id);
	set_cube_side_texture(_current_cube_id, 2, tex_id);
	set_cube_side_texture(_current_cube_id, 3, tex_id);
	set_cube_side_texture(_current_cube_id, 4, tex_id);
	set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}


void iso_texture(int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Error: iso_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 1: %i %i %i \n", sheet_id, xpos, ypos);
	int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
	//set cube side textures
	set_cube_side_texture(_current_cube_id, 0, tex_id);
	set_cube_side_texture(_current_cube_id, 1, tex_id);
	set_cube_side_texture(_current_cube_id, 2, tex_id);
	set_cube_side_texture(_current_cube_id, 3, tex_id);
	set_cube_side_texture(_current_cube_id, 4, tex_id);
	set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}

void side_texture(int side, int tex_id)
{
#ifdef DC_CLIENT 
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void side_texture(int side, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: side_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void hud_def(int hudy, int hudx, int tex_id)
{
#ifdef DC_CLIENT

    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def1 index on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
#endif
}

void hud_def(int hudy,int hudx, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def2, hudx,hudy on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: hud_def2, xpos,ypos on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    //printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
#endif
}


int texture_alias(const char* spritesheet) 
{ 
#if DC_CLIENT
    return LUA_load_cube_texture_sheet((char*) spritesheet); 
#else
    return 0;
#endif
}

int sprite_alias(int sheet_id, int ypos, int xpos)
{
#if DC_CLIENT
    //printf("Blit 3: %i %i %i \n", sheet_id, xpos, ypos);
    return LUA_blit_cube_texture(sheet_id, xpos, ypos); 
#else
    return 0;
#endif
}

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
