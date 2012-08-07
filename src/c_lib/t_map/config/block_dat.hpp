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
    int t2 = texture_alias("media/sprites/t02.png");

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
    push_texture();

    hud_def(1,1, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

    cube_def(0, EmptyBlock, "empty_block");
    iso_texture(error_block);
    push_texture();

    cube_def(1, SolidBlock, "terminal_blue");
    iso_texture(t0,1,3);
    push_texture();

    hud_def(1,2, t0,1,3);
    set_max_dmg(5);

    cube_def(2, SolidBlock, "terminal_green");
    iso_texture(t0,1,4);
    push_texture();

    hud_def(1,3, t0,1,4);
    set_max_dmg(5);

    cube_def(3, SolidBlock, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    push_texture();

    hud_def(1,4, t0,1,3);


    cube_def(6, ItemContainerBlock, "small_storage");
    iso_texture(t1, 13,4);
    side_texture(T, t1, 1,3);
    side_texture(B, t1, 1,3);
    side_texture(N, t1, 13,5);
    push_oriented_texture();

    //hud_def(2,1,t1,13,5);
    set_max_dmg(128);


    cube_def(7, ItemContainerBlock, "small_crafting_bench");
    iso_texture(t1, 3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    push_oriented_texture();

    //hud_def(2,2,t1,3,5);
    set_max_dmg(128);


    cube_def(8, ItemContainerBlock, "smelter_1");
    iso_texture(t1, 14,2);
    side_texture(T, t1, 14,2);
    side_texture(B, t1, 14,2);
    side_texture(N, t1, 14,1);
    push_oriented_texture();
    side_texture(N, t1, 14,3);  // smelter face on
    push_oriented_texture();

    //hud_def(2,3,t1,14,1);
    set_max_dmg(128);


    cube_def(9, ItemContainerBlock, "cryofreezer_1");
    iso_texture(t1, 14,5);
    side_texture(T, t1, 14,6);
    side_texture(B, t1, 14,6);
    side_texture(N, t1, 14,4);
    push_oriented_texture();

    //hud_def(2,5,t1,14,6);
    set_max_dmg(128);

    cube_def(10, ItemContainerBlock, "recycler");
    iso_texture(t1, 11,7);
    side_texture(T, t1, 11,5);
    side_texture(B, t1, 11,5);
    side_texture(N, t1, 11,6);
    push_oriented_texture();

    //hud_def(2,5,t1,14,6);
    set_max_dmg(128);

    cube_def(16, SolidBlock, "steel_block_1");
    iso_texture(t1, 1,3);
    push_texture();
    set_max_dmg(64);

    hud_def(5,1,t1,1,3);

    cube_def(17, SolidBlock, "steel_block_2");
    iso_texture(t1, 1,5);
    push_texture();
    set_max_dmg(64);

    hud_def(5,3,t1,1,5);

    cube_def(18, SolidBlock, "steel_block_3");
    iso_texture(t1, 1,4);
    side_texture(T, t1,1,5);
    side_texture(B, t1,1,5);
    push_texture();
    set_max_dmg(64);
    
    hud_def(5,2,t1,1,4);

    cube_def(23, SolidBlock, "methane_ice"); // we only need one
    iso_texture(t1, 5,4);
    push_texture();
    hud_def(3,6, t1, 5,4);
    set_max_dmg(5);

    cube_def(24, SolidBlock, "regolith");
    
    //pallete 0
    iso_texture(t1,10,5);
    push_texture();

    //pallete 1
    iso_texture(t1,10,4);
    side_texture(T, t1,10,3);
    side_texture(B, t1,10,5);
    push_texture();

    cube_def(25, SolidBlock, "rock_layer");
    iso_texture(t1,1,8);
    push_texture();
    hud_def(6,1, t1,1,8);

    cube_def(26, SolidBlock, "graphite");
    iso_texture(t1,10,6);
    push_texture();
    set_max_dmg(16);

    //hud_def(4,2, t1,10,6);

    cube_def(27, SolidBlock, "iron_ore");
    iso_texture(t1,8,3);
    push_texture();
    hud_def(6,2, t1,8,3);
    set_max_dmg(16);

    cube_def(28, SolidBlock, "copper_ore");
    iso_texture(t1,1,9);
    push_texture();
    hud_def(6,3, t1,1,9);
    set_max_dmg(16);

    cube_def(29, SolidBlock, "gallium_ore");
    iso_texture(t1,2,4);
    push_texture();
    hud_def(6,4, t1,2,4);
    set_max_dmg(16);

    cube_def(30, SolidBlock, "iridium_ore");
    iso_texture(t1,1,11);
    push_texture();
    set_max_dmg(16);

    //hud_def(6,5, t1,1,11);

    cube_def(31, SolidBlock, "bedrock");
    iso_texture(t1, 10,7);
    push_texture();
    set_max_dmg(255);


    cube_def(32, SolidBlock, "control_node");
    iso_texture(t1, 9,4);
    push_texture();
    set_max_dmg(5); //255
    hud_def(2,1, t1,9,4);

    cube_def(33, SolidBlock, "space_tree_trunk");
    iso_texture(t2, 1,1);
    push_texture();
    set_max_dmg(64); //255
    hud_def(2,2, t2,1,1);

    cube_def(34, SolidBlock, "space_tree_trunk2");
    iso_texture(t2, 2,1);
    push_texture();
    set_max_dmg(32); //255
    hud_def(2,3, t2,1,2);

    end_block_dat();

}


}
