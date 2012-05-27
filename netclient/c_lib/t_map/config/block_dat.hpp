#pragma once


#include <t_map/config/_interface.hpp>

namespace t_map 
{

void cube_def(int id, int type, const char* name);
void iso_texture(int tex_id);
void iso_texture(int sheet_id, int ypos, int xpos);
void side_texture(int side, int tex_id);
void side_texture(int side, int sheet_id, int ypos, int xpos);
void hud_def(int hudy, int hudx, int tex_id);
void hud_def(int hudy,int hudx, int sheet_id, int ypos, int xpos);
int texture_alias(const char* spritesheet);
void blit_block_item_sheet();

void set_max_dmg(int dmg);

/*
--[[
t top
b bottom
n north
s south
w west
e east
]]
*/

/*
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
*/

void load_block_dat()
{

    int t0 = texture_alias("media/sprites/t00.png");
    int t1 = texture_alias("media/sprites/t01.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block =  sprite_alias(t0,1,1);

    cube_def(255, ErrorBlock, "error_block");
    //iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);
    push_pallete();

    hud_def(1,1, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

    cube_def(0, EmptyBlock, "empty_block");
    iso_texture(error_block);
    push_pallete();

    cube_def(1, SolidBlock, "terminal_blue");
    iso_texture(t0,1,3);
    push_pallete();

    hud_def(1,2, t0,1,1);
    set_max_dmg(5);

    cube_def(2, SolidBlock, "terminal_green");
    iso_texture(t0,1,4);
    push_pallete();

    hud_def(1,3, t0,1,2);
    set_max_dmg(5);

    cube_def(3, SolidBlock, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    push_pallete();

    hud_def(1,4, t0,1,3);


    cube_def(6, ItemContainerBlock, "crate_2");
    iso_texture(t1, 13,4);
    side_texture(T, t1, 1,3);
    side_texture(B, t1, 1,3);
    side_texture(N, t1, 13,5);
    push_oriented_pallete();

    hud_def(2,2,t1,13,5);
/*
    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    hud_def(2,2,t1,1,3);
*/

/*
    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1, 12,1);
    side_texture(T, t1, 12,3);
    side_texture(B, t1, 12,3);
    side_texture(N, t1, 12,2);
    hud_def(2,2,t1,12,2);
*/
    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1, 3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    push_oriented_pallete();

    hud_def(2,2,t1,3,5);

/*
    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,12,4);
    side_texture(T, t1, 12,6);
    side_texture(B, t1, 12,6);
    side_texture(N, t1, 12,5);
    hud_def(2,2,t1,12,2);
*/
/*
    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,13,1);
    side_texture(T, t1, 13,3);
    side_texture(B, t1, 13,3);
    side_texture(N, t1, 13,2);
    hud_def(2,2,t1,13,2);
*/

    cube_def(8, ItemContainerBlock, "steel_1");
    iso_texture(t1, 1,3);
    push_pallete();

    hud_def(5,1,t1,1,3);

    cube_def(9, ItemContainerBlock, "steel_2");
    iso_texture(t1, 1,4);
    push_pallete();

    hud_def(5,2,t1,1,4);

    cube_def(10, ItemContainerBlock, "steel_3");
    iso_texture(t1, 1,5);
    push_pallete();

    hud_def(5,3,t1,1,5);


    cube_def(16, SolidBlock, "methane_1");
    iso_texture(t1,2,2);
    push_pallete();

    hud_def(3,1, t1,2,2);
    set_max_dmg(5);

    cube_def(17, SolidBlock, "methane_2");
    iso_texture(t1,3,2);
    push_pallete();
    hud_def(3,2, t1,3,2);
    set_max_dmg(5);

    cube_def(18, SolidBlock, "methane_3");
    iso_texture(t1,4,3);
    push_pallete();

    hud_def(3,3, t1,4,3);
    set_max_dmg(5);

    cube_def(19, SolidBlock, "methane_4");
    iso_texture(t1,5,3);
    push_pallete();
    hud_def(3,4, t1,5,3);
    set_max_dmg(5);

    cube_def(20, SolidBlock, "methane_5");
    iso_texture(t1,4,1);
    push_pallete();
    hud_def(3,5, t1,4,1);
    set_max_dmg(5);

    cube_def(54, SolidBlock, "regolith");
    
    //pallete 0
    iso_texture(t1,10,5);
    push_pallete();

    //pallete 1
    iso_texture(t1,10,4);
    side_texture(T, t1,10,3);
    side_texture(B, t1,10,5);
    push_pallete();

/* 
    palette_def(1);
    palette_iso_texture(t1,10,3);
    palette_side_texture(N, t1,10,5);
    palette_def(1);


    end_palette_def();

    hud_def(4,1, t1,10,4);
    //color_type(1); //perlin
*/
    cube_def(55, SolidBlock, "carbon");
    iso_texture(t1,10,6);
    push_pallete();

    hud_def(4,2, t1,10,6);

	end_block_dat();
    //end_block_item_sheet();

}


}