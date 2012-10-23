#pragma once


#include <t_map/config/_interface.hpp>

namespace t_map 
{

void cube_def(CubeType type, const char* name);
void end_cube_def();

void iso_texture(int tex_id);
void iso_texture(int sheet_id, int ypos, int xpos);
void side_texture(int side, int tex_id);
void side_texture(int side, int sheet_id, int ypos, int xpos);
void hud_def(int yhud,int xhud, int sheet_id, int ypos, int xpos);
int texture_alias(const char* spritesheet);

void set_max_damage(int dmg);

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
    ErrorCube,
    EmptyCube,
    SolidCube,
    ItemContainerCube
*/

#if PRODUCTION
# define RUINS_DMG 32
#else
# define RUINS_DMG 4
#endif

void load_block_dat()
{

    int t0 = texture_alias("media/sprites/terrain/t00.png");
    int t1 = texture_alias("media/sprites/terrain/t01.png");
    int t2 = texture_alias("media/sprites/terrain/t02.png");
    int t3 = texture_alias("media/sprites/terrain/t03.png");
    //int t4 = texture_alias("media/sprites/terrain/t04.png");
    int t5 = texture_alias("media/sprites/terrain/t05.png");
    int t6 = texture_alias("media/sprites/terrain/t06.png");
    int t7 = texture_alias("media/sprites/terrain/t07.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block =  sprite_alias(t0,1,1);

    // MUST COME FIRST (error block goes last)
    cube_def(EmptyCube, "empty_block");
    iso_texture(error_block);
    push_texture();

    cube_def(SolidCube, "terminal_blue");
    iso_texture(t0,1,3);
    push_texture();

    hud_def(1,2, t0,1,3);
    set_max_damage(24);

    cube_def(SolidCube, "terminal_green");
    iso_texture(t0,1,4);
    push_texture();

    hud_def(1,3, t0,1,4);
    set_max_damage(24);

    cube_def(SolidCube, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    push_texture();
    hud_def(1,4, t0,2,3);

    cube_def(ItemContainerCube, "small_storage");
    iso_texture(t1, 13,4);
    side_texture(T, t1, 1,3);
    side_texture(B, t1, 1,3);
    side_texture(N, t1, 13,5);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(ItemContainerCube, "small_crafting_bench");
    iso_texture(t1, 3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(ItemContainerCube, "smelter_1");
    iso_texture(t1, 14,2);
    side_texture(T, t1, 14,2);
    side_texture(B, t1, 14,2);
    side_texture(N, t1, 14,1);
    push_oriented_texture();
    side_texture(N, t1, 14,3);  // smelter face on
    push_oriented_texture();
    set_max_damage(128);

    cube_def(ItemContainerCube, "cryofreezer_1");
    iso_texture(t1, 14,5);
    side_texture(T, t1, 14,6);
    side_texture(B, t1, 14,6);
    side_texture(N, t1, 14,4);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(ItemContainerCube, "crusher");
    iso_texture(t1, 11,7);
    side_texture(T, t1, 11,5);
    side_texture(B, t1, 11,5);
    side_texture(N, t1, 11,6);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(SolidCube, "steel_block_1");
    iso_texture(t1, 1,3);
    push_texture();
    set_max_damage(64);
    hud_def(5,1,t1,1,3);

    cube_def(SolidCube, "steel_block_2");
    iso_texture(t1, 1,5);
    push_texture();
    set_max_damage(64);
    hud_def(5,3,t1,1,5);

    cube_def(SolidCube, "steel_block_3");
    iso_texture(t1, 1,4);
    side_texture(T, t1,1,5);
    side_texture(B, t1,1,5);
    push_texture();
    set_max_damage(64);
    hud_def(5,2,t1,1,4);

    cube_def(SolidCube, "methane_ice"); // we only need one
    iso_texture(t1, 5,4);
    push_texture();
    set_max_damage(5);

    cube_def(SolidCube, "regolith", CUBE_MATERIAL_DIRT);
    // palette 0
    iso_texture(t7,4,4);
    push_texture();
    //palette 1
    iso_texture(t7,3,4);
    side_texture(T, t7,2,4);
    side_texture(B, t7,4,4);
    push_texture();

    cube_def(SolidCube, "rock", CUBE_MATERIAL_STONE);
    iso_texture(t1,9,9);
    push_texture();
    hud_def(6,1, t1,9,9);
    set_max_damage(48);

    cube_def(SolidCube, "coal", CUBE_MATERIAL_DIRT);
    iso_texture(t1,10,6);
    push_texture();
    set_max_damage(32);

    cube_def(SolidCube, "iron_ore", CUBE_MATERIAL_STONE);
    iso_texture(t1,8,3);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "copper_ore", CUBE_MATERIAL_STONE);
    iso_texture(t1,1,9);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "gallium_ore", CUBE_MATERIAL_STONE);
    iso_texture(t1,8,9);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "iridium_ore", CUBE_MATERIAL_STONE);
    iso_texture(t1,1,11);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "bedrock");
    iso_texture(t1, 10,7);
    push_texture();
    set_max_damage(INVINCIBLE_CUBE_DAMAGE);

    cube_def(SolidCube, "control_node", CUBE_MATERIAL_DECORATION);
    iso_texture(t1, 9,4);
    push_texture();
    hud_def(2,1, t1,9,4);

    cube_def(SolidCube, "space_tree_trunk", CUBE_MATERIAL_DECORATION);
    iso_texture(t2, 1,1);
    push_texture();
    hud_def(6,4, t2,1,1);

    cube_def(SolidCube, "space_tree_trunk2", CUBE_MATERIAL_DECORATION);
    iso_texture(t2, 2,1);
    push_texture();
    hud_def(6,5, t2,2,1);

    // were these meant to be tree leaves?  using them as such for now
    cube_def(SolidCube, "leaves", CUBE_MATERIAL_DECORATION);
    iso_texture(t2, 1,3);
    push_texture();

    cube_def(SolidCube, "leaves2", CUBE_MATERIAL_DECORATION);
    iso_texture(t2, 2,3);
    push_texture();

    cube_def(SolidCube, "leaves_red", CUBE_MATERIAL_DECORATION);
    iso_texture(t2, 1,2);
    push_texture();

    // ruins
    cube_def(SolidCube, "raised_tile_gray");
    iso_texture(t1,6,3);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile_blue");
    iso_texture(t1,6,4);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile_green");
    iso_texture(t1,6,5);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile_red");
    iso_texture(t1,6,6);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "ruins_1");
    iso_texture(t5,1,1);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "ruins_2");
    iso_texture(t5,1,2);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "ruins_3");
    iso_texture(t5,1,3);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "ruins_4");
    iso_texture(t5,1,4);
    push_texture();
    set_max_damage(RUINS_DMG);


    // random textures from cell to fill the block placer
    DEFAULT_MAX_DAMAGE = 16;
    
    cube_def(SolidCube, "cell1", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 1,1);
    push_texture();
    hud_def(1,5, t3,1,1);

    cube_def(SolidCube, "cell2", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 2,1);
    push_texture();
    hud_def(1,6, t3,2,1);

    cube_def(SolidCube, "cell3", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 3,1);
    push_texture();
    hud_def(1,7, t3,3,1);

    cube_def(SolidCube, "cell4", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 4,1);
    push_texture();
    hud_def(1,8, t3,4,1);

    cube_def(SolidCube, "cell5", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 5,1);
    push_texture();
    hud_def(2,4, t3,5,1);

    cube_def(SolidCube, "cell6", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 6,1);
    push_texture();
    hud_def(2,5, t3,6,1);

    cube_def(SolidCube, "cell8", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 8,1);
    push_texture();
    hud_def(2,7, t3,8,1);

    cube_def(SolidCube, "cell9", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 1,2);
    push_texture();
    hud_def(2,8, t3,1,2);
    
    cube_def(SolidCube, "cell10", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 2,2);
    push_texture();
    hud_def(3,1, t3,2,2);

    cube_def(SolidCube, "cell11", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 3,2);
    push_texture();
    hud_def(3,2, t3,3,2);

    cube_def(SolidCube, "cell12", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 4,2);
    push_texture();
    hud_def(3,3, t3,4,2);

    cube_def(SolidCube, "cell13", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 5,2);
    push_texture();
    hud_def(3,4, t3,5,2);
    
    cube_def(SolidCube, "cell14", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 6,2);
    push_texture();
    hud_def(3,5, t3,6,2);
    
    cube_def(SolidCube, "cell15", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 7,2);
    push_texture();
    hud_def(3,7, t3,7,2);

    cube_def(SolidCube, "cell16", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 8,2);
    push_texture();
    hud_def(3,8, t3,8,2);
    
    cube_def(SolidCube, "cell17", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 1,3);
    push_texture();
    hud_def(4,1, t3,1,3);
    
    cube_def(SolidCube, "cell18", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 2,3);
    push_texture();
    hud_def(4,2, t3,2,3);
    
    cube_def(SolidCube, "cell19", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 3,3);
    push_texture();
    hud_def(4,3, t3,3,3);
    
    cube_def(SolidCube, "cell20", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 4,3);
    push_texture();
    hud_def(4,4, t3,4,3);
    
    cube_def(SolidCube, "cell21", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 5,3);
    side_texture(T, t3, 5,2);
    side_texture(B, t3, 5,4);
    push_texture();
    hud_def(4,5, t3,5,3);
    
    cube_def(SolidCube, "cell22", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 6,3);
    push_texture();
    hud_def(4,6, t3,6,3);

    cube_def(SolidCube, "cell23", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 7,3);
    side_texture(T, t3, 7,2);
    side_texture(B, t3, 7,4);
    push_texture();
    hud_def(4,7, t3,7,3);
    
    cube_def(SolidCube, "cell24", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 8,3);
    push_texture();
    hud_def(4,8, t3,8,3);
    
    cube_def(SolidCube, "cell25", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 1,4);
    push_texture();
    hud_def(5,4, t3,1,4);
    
    cube_def(SolidCube, "cell26", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 2,4);
    push_texture();
    hud_def(5,5, t3,2,4);
    
    cube_def(SolidCube, "cell27", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 3,4);
    push_texture();
    hud_def(5,6, t3,3,4);

    cube_def(SolidCube, "cell28", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 4,4);
    push_texture();
    hud_def(5,7, t3,4,4);

    cube_def(SolidCube, "cell29", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 5,4);
    push_texture();
    hud_def(5,8, t3,5,4);

    cube_def(SolidCube, "cell30", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 6,4);
    push_texture();
    hud_def(6,3, t3,6,4);

    cube_def(SolidCube, "cell31", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 7,4);
    push_texture();
    hud_def(1,1, t3,7,4);

    cube_def(SolidCube, "cell32", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 8,4);
    push_texture();
    hud_def(6,2, t3,8,4);

    cube_def(SolidCube, "cell33", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 1,8);
    push_texture();
    hud_def(2,2, t3,1,8);
    
    cube_def(SolidCube, "cell34", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 4,7);
    push_texture();
    hud_def(2,3, t3,4,7);
    
    cube_def(SolidCube, "cell35", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 5,8);
    push_texture();
    hud_def(3,6, t3,5,8);
    
    cube_def(SolidCube, "cell36", CUBE_MATERIAL_DECORATION);
    iso_texture(t3, 8,7);
    push_texture();
    hud_def(2,6, t3,8,7);

    // IceFlame's stuff
    
    cube_def(SolidCube, "iceflame01", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,1);
    push_texture();
    hud_def(6,6, t6,1,1);
    
    cube_def(SolidCube, "iceflame02", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,2);
    push_texture();
    hud_def(6,7, t6,1,2);
    
    cube_def(SolidCube, "iceflame03", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,3);
    push_texture();
    hud_def(6,8, t6,1,3);
    
    cube_def(SolidCube, "iceflame04", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,4);
    push_texture();
    hud_def(7,1, t6,1,4);
    
    cube_def(SolidCube, "iceflame05", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,5);
    push_texture();
    hud_def(7,2, t6,1,5);
    
    cube_def(SolidCube, "iceflame06", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,6);
    push_texture();
    hud_def(7,3, t6,1,6);
    
    cube_def(SolidCube, "iceflame07", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,7);
    push_texture();
    hud_def(7,4, t6,1,7);
    
    cube_def(SolidCube, "iceflame08", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 1,8);
    push_texture();
    hud_def(7,5, t6,1,8);

    // skip the rest of the lava sprites...
    
    cube_def(SolidCube, "iceflame09", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 2,6);
    push_texture();
    hud_def(7,6, t6,2,6);
    
    cube_def(SolidCube, "iceflame10", CUBE_MATERIAL_DECORATION);
    iso_texture(t6, 2,7);
    push_texture();
    hud_def(7,7, t6,2,7);

    // MUST COME LAST
    cube_def(ErrorCube, "error_block");
    iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);
    push_texture();

    end_cube_def();  // finalize
    verify_config(); // validate
}

#undef RUINS_DMG

}   // t_map
