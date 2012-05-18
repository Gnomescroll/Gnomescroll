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

    hud_def(1,1, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

    cube_def(0, EmptyBlock, "empty_block");
    iso_texture(error_block);


    cube_def(1, SolidBlock, "terminal_blue");
    iso_texture(t0,1,3);
    hud_def(1,2, t0,1,1);

    cube_def(2, SolidBlock, "terminal_green");
    iso_texture(t0,1,4);
    hud_def(1,3, t0,1,2);


    cube_def(3, SolidBlock, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    hud_def(1,4, t0,1,3);


    cube_def(6, ItemContainerBlock, "crate_2");
    iso_texture(t1,2,6);
    side_texture(T, t1, 1,7);
    side_texture(B, t1, 1,7);
    side_texture(B, t1, 1,7);
    hud_def(2,1, t1,1,3);

    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    hud_def(2,2,t1,1,3);

    cube_def(16, SolidBlock, "methane_1");
    iso_texture(t1,2,2);
    hud_def(3,1, t1,2,2);

    cube_def(17, SolidBlock, "methane_2");
    iso_texture(t1,3,2);
    hud_def(3,2, t1,3,2);

    cube_def(18, SolidBlock, "methane_3");
    iso_texture(t1,4,3);
    hud_def(3,3, t1,4,3);

    cube_def(19, SolidBlock, "methane_4");
    iso_texture(t1,5,3);
    hud_def(3,4, t1,5,3);

    cube_def(20, SolidBlock, "methane_5");
    iso_texture(t1,4,1);
    hud_def(3,5, t1,4,1);


    cube_def(54, SolidBlock, "regolith");
    iso_texture(t1,10,4);
    side_texture(T, t1,10,3);
    side_texture(B, t1,10,5);
    hud_def(4,1, t1,10,4);

    cube_def(55, SolidBlock, "carbon");
    iso_texture(t1,10,6);
    hud_def(4,2, t1,10,6);

	end_block_dat();
    //end_block_item_sheet();

}


}