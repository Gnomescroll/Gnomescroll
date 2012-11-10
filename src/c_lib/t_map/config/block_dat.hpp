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
# define RUINS_DMG 4
#endif

void load_block_dat()
{
    SpriteSheet t8 = texture_alias("media/sprites/terrain/t08.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block = sprite_alias(t8,12,5);

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
    iso_texture(t8, 1,9);
    side_texture(T, t8, 1,11);
    side_texture(B, t8, 1,11);
    side_texture(N, t8, 1,10);
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("crafting_bench_basic", CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    iso_texture(t8, 5,10);
    side_texture(T, t8, 5,11);
    side_texture(B, t8, 5,11);
    side_texture(N, t8, 5,9);
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("smelter_basic", CONTAINER_TYPE_SMELTER_ONE);
    iso_texture(t8, 3,5);
    side_texture(N, t8, 4,5);
    push_oriented_texture();
    side_texture(N, t8, 5,5);  // smelter face on
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("cryofreezer_small", CONTAINER_TYPE_CRYOFREEZER_SMALL);
    iso_texture(t8, 5,6);
    push_oriented_texture();
    set_max_damage(128);

    cube_def_container("crusher", CONTAINER_TYPE_CRUSHER);
    iso_texture(t8, 7,10);
    side_texture(T, t8, 7,11);
    side_texture(B, t8, 7,11);
    side_texture(N, t8, 7,9);
    push_oriented_texture();
    set_max_damage(128);

    cube_def(SolidCube, "steel_block_1");
    iso_texture(t8, 9,5);
    push_texture();
    set_max_damage(64);
    hud_def(t8,9,5);

    cube_def(SolidCube, "steel_block_2");
    iso_texture(t8, 10,5);
    push_texture();
    set_max_damage(64);
    hud_def(t8,10,5);

    cube_def(SolidCube, "steel_block_3");
    iso_texture(t8, 11,5);
    push_texture();
    set_max_damage(64);
    hud_def(t8,11,5);

    cube_def(SolidCube, "methane_ice");
    iso_texture(t8, 15,1);
    push_texture();
    set_max_damage(5);

    cube_def(SolidCube, "regolith", CUBE_MATERIAL_DIRT);
    // palette 0
    iso_texture(t8,4,4);
    push_texture();
    //palette 1
    iso_texture(t8,3,4);
    side_texture(T, t8,2,4);
    side_texture(B, t8,4,4);
    push_texture();
    set_max_damage(24);

    cube_def(SolidCube, "rock", CUBE_MATERIAL_STONE);
    iso_texture(t8,5,4);
    push_texture();
    hud_def(t8, 5,4);
    set_max_damage(48);

    cube_def(SolidCube, "coal", CUBE_MATERIAL_DIRT);
    iso_texture(t8,6,4);
    push_texture();
    set_max_damage(32);

    cube_def(SolidCube, "iron_ore", CUBE_MATERIAL_STONE);
    iso_texture(t8,7,1);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "copper_ore", CUBE_MATERIAL_STONE);
    iso_texture(t8,9,2);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "gallium_ore", CUBE_MATERIAL_STONE);
    iso_texture(t8,11,2);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "iridium_ore", CUBE_MATERIAL_STONE);
    iso_texture(t8,9,3);
    push_texture();
    set_max_damage(64);

    cube_def(SolidCube, "bedrock");
    iso_texture(t8, 14,4);
    push_texture();
    set_max_damage(INVINCIBLE_CUBE_DAMAGE);

    cube_def(SolidCube, "control_node", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 12,6);
    push_texture();
    hud_def(t8,12,6);

    cube_def(SolidCube, "space_tree_trunk1", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 7,15);
    push_texture();
    hud_def(t8,7,15);

    cube_def(SolidCube, "space_tree_trunk2", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 5,15);
    push_texture();
    hud_def(t8, 5,15);

    cube_def(SolidCube, "mushroom1", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 8,9);
    push_texture();
    hud_def(t8,8,9);

    cube_def(SolidCube, "mushroom2", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 8,11);
    push_texture();
    hud_def(t8,8,11);

    cube_def(SolidCube, "leaves1", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 6,15);
    push_texture();

    cube_def(SolidCube, "leaves2", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 8,10);
    push_texture();

    cube_def(SolidCube, "leaves3", CUBE_MATERIAL_DECORATION);
    iso_texture(t8, 8,12);
    push_texture();

    // ruins
    cube_def(SolidCube, "raised_tile1");
    iso_texture(t8,14,5);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile2");
    iso_texture(t8,14,6);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile3");
    iso_texture(t8,15,5);
    push_texture();
    set_max_damage(RUINS_DMG);

    cube_def(SolidCube, "raised_tile4");
    iso_texture(t8,15,6);
    push_texture();
    set_max_damage(RUINS_DMG);

    end_cube_def();  // finalize

    apply_block_dat_changes();

    verify_config(); // validate
    
    save_cube_names();  // safe to save cube names now
}

#undef RUINS_DMG

}   // t_map
