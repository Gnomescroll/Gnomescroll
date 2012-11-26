#pragma once

#include <t_map/config/_interface.hpp>

namespace t_map
{

void apply_block_dat_changes()
{
    // NEVER REUSE A NAME!!
    //change_block("old_block", "new_block");

    // Note: if you define any name changes for a block that automatically creates an item or container,
    // you have to manually define the name change in the item or container dat

    // deletions
    const size_t buflen = 32;
    char name[buflen] = {'\0'};
    for (int i=1; i<=36; i++)
    {
        if (i == 7) continue;
        snprintf(name, buflen, "cell%d", i);
        name[buflen-1] = '\0';
        change_block(name, "regolith");
    }
    for (int i=1; i<=10; i++)
    {
        snprintf(name, buflen, "iceflame%02d", i);
        name[buflen-1] = '\0';
        change_block(name, "regolith");
    }

    change_block("terminal_blue", "regolith");
    change_block("terminal_green", "regolith");
    change_block("battery", "regolith");
    change_block("ruins_1", "rock");
    change_block("ruins_2", "rock");
    change_block("ruins_3", "rock");
    change_block("ruins_4", "rock");

    // renames
    change_block("mushroom1", "mushroom_stem1");
    change_block("mushroom2", "mushroom_stem2");
    change_block("space_tree_trunk", "space_tree_trunk1");
    change_block("leaves", "leaves1");
    change_block("leaves_red", "leaves3");
    change_block("raised_tile_gray", "raised_tile1");
    change_block("raised_tile_blue", "raised_tile2");
    change_block("raised_tile_green", "raised_tile3");
    change_block("raised_tile_red", "raised_tile4");

    cube_name_map->condense();  // finalize
}

/*
--[[
* side_texture(D, tex);
    T top
    B bottom
    N north
    S south
    W west
    E east
]]
*/

#if PRODUCTION
# define RUINS_DMG 32
#else
# define RUINS_DMG 2
#endif

void load_block_dat()
{
    SpriteSheet c0 = texture_alias("media/sprites/terrain/cell_00.png");
    SpriteSheet c1 = texture_alias("media/sprites/terrain/cell_01.png");
    SpriteSheet c2 = texture_alias("media/sprites/terrain/cell_02.png");
    SpriteSheet c3 = texture_alias("media/sprites/terrain/cell_03.png");
    //SpriteSheet c4 = texture_alias("media/sprites/terrain/cell_04.png");  // no alpha
    //SpriteSheet c5 = texture_alias("media/sprites/terrain/cell_05.png");  // no alpha
    SpriteSheet c6 = texture_alias("media/sprites/terrain/cell_06.png");
    SpriteSheet cc = texture_alias("media/sprites/terrain/corpusc.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block = sprite_alias(c0, 1,1);

    cube_def_empty("empty_block");
    iso_texture(error_block);
    push_texture();

    cube_def_error("error_block");
    iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);
    push_texture();

    cube_def_container("storage_block_small", CONTAINER_TYPE_STORAGE_BLOCK_SMALL);
    iso_texture(c1, 14,4);
    side_texture(T, c1, 1,3);
    side_texture(B, c1, 1,3);
    side_texture(N, c1, 14,5);
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("crafting_bench_basic", CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    iso_texture(c1, 3,4);
    side_texture(T, c1, 2,5);
    side_texture(B, c1, 3,6);
    side_texture(N, c1, 3,5);
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("smelter_basic", CONTAINER_TYPE_SMELTER_ONE);
    iso_texture(c1, 14,2);
    side_texture(T, c1, 14,2);
    side_texture(B, c1, 14,2);
    side_texture(N, c1, 14,1);
    push_oriented_texture();
    side_texture(N, c1, 14,3);  // smelter face on
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("cryofreezer_small", CONTAINER_TYPE_CRYOFREEZER_SMALL);
    iso_texture(c1, 14,5);
    side_texture(T, c1, 14,6);
    side_texture(B, c1, 14,6);
    side_texture(N, c1, 14,4);
	push_oriented_texture();
    set_max_damage(128);

    cube_def_container("crusher", CONTAINER_TYPE_CRUSHER);
    iso_texture(c1, 11,7);
    side_texture(T, c1, 11,5);
    side_texture(B, c1, 11,5);
    side_texture(N, c1, 11,6);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(SolidCube, "steel_block_1");
    iso_texture(c1, 1,3);
    push_texture();
    set_max_damage(64);
    hud_def    (c1, 1,3);

    cube_def(SolidCube, "steel_block_2");
    iso_texture(c1, 1,5);
    push_texture();
    set_max_damage(64);
    hud_def    (c1, 1,5);

    cube_def(SolidCube, "steel_block_3");
    iso_texture    (c1, 1,4);
    side_texture(T, c1, 1,5);
    side_texture(B, c1, 1,5);
	push_texture();
    set_max_damage(64);
    hud_def        (c1, 1,4);

    cube_def(SolidCube, "methane_ice");
    iso_texture(c1, 5,4);;
    push_texture();
    set_max_damage(5);

    cube_def(SolidCube, "regolith", CUBE_MATERIAL_DIRT);
    // palette 0
    iso_texture    (c1, 12,12);
    push_texture();
    //palette 1
    iso_texture    (c1, 12,11);
    side_texture(T, c1, 12,10);
    side_texture(B, c1, 12,12);
    push_texture();
    set_max_damage(24);

    cube_def(SolidCube, "rock", CUBE_MATERIAL_STONE);
    iso_texture(c1, 9,9);
    push_texture();
    hud_def    (c1, 9,9);
    set_max_damage(48);

    cube_def(SolidCube, "coal", CUBE_MATERIAL_DIRT);
    iso_texture(c1, 10,6);
    push_texture();
    set_max_damage(32);

    cube_def(SolidCube, "iron_ore", CUBE_MATERIAL_STONE);
    iso_texture(c1, 8,3);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "copper_ore", CUBE_MATERIAL_STONE);
    iso_texture(c1, 1,9);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "gallium_ore", CUBE_MATERIAL_STONE);
    iso_texture(c1, 8,9);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "iridium_ore", CUBE_MATERIAL_STONE);
    iso_texture(c1, 4,9);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "bedrock");
    iso_texture(c1, 10,7);
    push_texture();
    set_max_damage(INVINCIBLE_CUBE_DAMAGE);

    cube_def(SolidCube, "control_node", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 9,4);
    push_texture();
    hud_def    (c1, 9,4);

    // terrain features
	cube_def(SolidCube, "space_tree_trunk1", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 6,8);
    push_texture();
    hud_def    (c1, 6,8);

    cube_def(SolidCube, "space_tree_trunk2", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 2,8);
    push_texture();
    hud_def    (c1, 2,8);

    cube_def(SolidCube, "space_tree_trunk3", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 2,4);
    push_texture();
    hud_def    (c1, 2,4);

    cube_def(SolidCube, "leaves1", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 2,9);
    push_texture();

    cube_def(SolidCube, "leaves2", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 8,1);
    push_texture();

    cube_def(SolidCube, "leaves3", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 5,3);
    push_texture();

    cube_def(SolidCube, "mushroom_stem1", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 1,1);
    push_texture();
    hud_def    (c1, 1,1);

    cube_def(SolidCube, "mushroom_stem2", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 2,2);
    push_texture();
    hud_def    (c1, 2,2);

    cube_def(SolidCube, "mushroom_cap1", CUBE_MATERIAL_DECORATION);
//    iso_texture(t8, 8,9);
    iso_texture(c1, 5,5);
    push_texture();

    cube_def(SolidCube, "mushroom_cap2", CUBE_MATERIAL_DECORATION);
//    iso_texture(t8, 8,9);
    iso_texture(c1, 6,2);
    push_texture();

    cube_def(SolidCube, "mushroom_cap3", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 1,2);
    push_texture();

    // ruins
    cube_def(SolidCube, "raised_tile1");
    iso_texture(c1, 8,4);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile2");
    iso_texture(c1, 8,5);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile3");
    iso_texture(c1, 8,6);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile4");
    iso_texture(c1, 8,7);
    push_texture();
    set_max_damage(RUINS_DMG);

    end_cube_def();  // finalize

    apply_block_dat_changes();

    verify_config(); // validate

    save_cube_names();  // safe to save cube names now
}

#undef RUINS_DMG

}   // t_map
