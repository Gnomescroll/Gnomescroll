/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/config/_interface.hpp>
#include <SDL/texture_sheet_loader.hpp>
#include <item/container/config/names.hpp>

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
    change_block("raised_tile_gray",  "raised_tile1");
    change_block("raised_tile_blue",  "raised_tile2");
    change_block("raised_tile_green", "raised_tile3");
    change_block("raised_tile_red",   "raised_tile4");
    change_block("raised_tile1", "ruins_ceiling1");
    change_block("raised_tile2", "ruins_ceiling2");
    change_block("raised_tile3", "ruins_ceiling3");
    change_block("raised_tile4", "ruins_ceiling4");

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
    using TextureSheetLoader::cube_texture_alias;
    SpriteSheet c0 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_00.png");
    SpriteSheet c1 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_01.png");
    //SpriteSheet c2 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_02.png");
    //SpriteSheet c3 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_03.png");
    //SpriteSheet c4 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_04.png");
    //SpriteSheet c5 = cube_texture_alias(MEDIA_PATH "sprites/terrain/cell_05.png");
    SpriteSheet iceflame = cube_texture_alias(MEDIA_PATH "sprites/terrain/iceflame_06.png");
    SpriteSheet cc = cube_texture_alias(MEDIA_PATH "sprites/terrain/corpusc.png");
    SpriteSheet tgm = cube_texture_alias(MEDIA_PATH "sprites/terrain/tgmtex.png");


    //trees
    SpriteSheet tree_00 = cube_texture_alias(MEDIA_PATH "sprites/terrain/tree_00.png", 16);
    SpriteSheet tree_01 = cube_texture_alias(MEDIA_PATH "sprites/terrain/tree_01.png", 16);

    // Cell: this is an example of loading a 16x16 cube sheet into the 32x32 loader
    // just put 16 as a second argument and it will work
    //SpriteSheet c16 = cube_texture_alias(MEDIA_PATH "sprites/terrain/old/cell_16x16/t08.png", 16);

    const int T = 0; //Top
    const int B = 1; //Bottom
    const int N = 2; //North
    const int S = 3; //South
    const int W = 4; //West
    const int E = 5; //East

    int error_block = sprite_alias(c0, 1,1);

    cube_def_empty("empty_block");
    iso_texture(error_block);
    push_texture();
    set_description("");

    cube_def_error("error_block");
    iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);
    push_texture();
    set_description("This is an error block. If you know what caused it to appear, contact us!\n");

    cube_def(EmptyCube, "mech_light_empty"); //empty block which is light source
    iso_texture(error_block);
    push_texture();
    set_light_value(15);
    set_description("");

    cube_def_container("storage_block_small");
    iso_texture(    c1, 13,4);
    side_texture(T, c1, 1,3);
    side_texture(B, c1, 1,3);
    side_texture(N, c1, 13,5);
    push_oriented_texture();
    set_max_damage(128);
    set_description("This is a container block. Right-click it to open its inventory.\n"
                    "You can place items inside by dragging them with the mouse, and\n"
                    "can take items from it. This block is useful for storing items\n"
                    "if your inventory is full, but be sure to keep it well-hidden!\n");

    cube_def_container("crafting_bench_basic");
    iso_texture(    c1, 3,4);
    side_texture(T, c1, 2,5);
    side_texture(B, c1, 3,6);
    side_texture(N, c1, 3,5);
    push_oriented_texture();
    set_max_damage(128);
    set_description("A crafting bench is used for creating one item out of another. Place your items in the\n"
                    "item slots on the left and take out the item on the right. If nothing appears, your recipe\n"
                    "is wrong and you should research it again. These are some basic recipes:\n"
                    "any metal bar + another bar of the same metal -> a shovel\n"
                    "any metal bar + any crystal -> a laser\n"
                    "iron bar + iron bar + iron bar + iron bar -> a storage block\n"
                    "any metal bar + methane ice -> a charge pack(right-click to consume)\n"
                    "More recipes can be found online, or you can just ask other players.\n");

    cube_def_container("smelter_basic");
    iso_texture(c1, 14,2);
    side_texture(T, c1, 14,2);
    side_texture(B, c1, 14,2);
    side_texture(N, c1, 14,1);
    push_oriented_texture();
    side_texture(N, c1, 14,3);  // smelter face on
    push_oriented_texture();
    set_max_damage(128);
    set_description("A furnace is a block used for smelting. Right-click it to open its inventory. Place your items\n"
                    "in the slots by dragging them there with the left mouse button. Press E to close the inventory.\n"
                    "When the furnace runs out of fuel, the fire inside will stop burning. When that happens, open\n"
                    "the furnace inventory and take out your freshly-smelted items.\n");

    cube_def_container("cryofreezer_small");
    iso_texture(c1, 14,5);
    side_texture(T, c1, 14,6);
    side_texture(B, c1, 14,6);
    side_texture(N, c1, 14,4);
    push_oriented_texture();
    set_max_damage(128);
    set_description("This is a cryofreezer. It is used for storing materials that would evaporate under other conditions.\n"
                    "Right-click it to open its inventory.\n");

    cube_def_container("crusher");
    iso_texture(c1, 11,7);
    side_texture(T, c1, 11,5);
    side_texture(B, c1, 11,5);
    side_texture(N, c1, 11,6);
    push_oriented_texture();
    set_max_damage(128);
    set_description("This is a crusher. It is used for recycling items. Right-click it to open its inventory, put something\n"
                    "inside by dragging it with your mouse, and press the X on the lower box. The new item will jump out.\n");

    cube_def(SolidCube, "steel_block_1");
    iso_texture(c1, 1,3);
    push_texture();
    set_max_damage(64);
    hud_def    (c1, 1,3);
    //set_light_value(15);
    //set_rad_value(15);
    set_description("This is a steel block. It is made out of iron, and thus is very tough. Its main use is as a building\n"
                    "material, but it can also be used for safety shields when detonating explosives etc.\n");

    cube_def(SolidCube, "steel_block_2");
    iso_texture(c1, 1,5);
    push_texture();
    set_max_damage(64);
    hud_def    (c1, 1,5);
    set_description("This is a steel block. It is made out of iron, and thus is very tough. Its main use is as a building\n"
                    "material, but it can also be used for safety shields when detonating explosives etc.\n");

    cube_def(SolidCube, "steel_block_3");
    iso_texture    (c1, 1,4);
    side_texture(T, c1, 1,5);
    side_texture(B, c1, 1,5);
    push_texture();
    set_max_damage(64);
    hud_def        (c1, 1,4);
    set_description("This is a steel block. It is made out of iron, and thus is very tough. Its main use is as a\n"
                    "building material, but it can also be used for safety shields when detonating explosives etc.\n");

    cube_def(SolidCube, "methane_ice");
    iso_texture(c1, 5,4);;
    push_texture();
    set_max_damage(5);
    set_description("Methane ice is the crystalized form of CH4, an explosive gas used for smelting. If not put\n"
                    "in a cryofreezer, methane ice will slowly evaporate out of containers and the inventory.\n"
                    "Other uses include explosives and charge packs. The blocks are very wak and easy to mine.\n");

    cube_def(SolidCube, "regolith", CUBE_MATERIAL_DIRT);
    iso_texture(tgm, 3, 2); // when not the topmost regolith cube
    push_texture();
    iso_texture    (tgm, 2,2); // side with diff colored rim to match top
    side_texture(T, tgm, 1,2);
    side_texture(B, tgm, 3,2);
    push_texture();
    set_max_damage(24);
    set_description("Regolith is a kind of dust commonly found on the surface of astronomical objects,\n"
                    "including asteroids and the moon. In Gnomescroll, the block is probably the most\n"
                    "accessible building material, but also one of the weakest.\n");

    cube_def(SolidCube, "rock", CUBE_MATERIAL_STONE);
    iso_texture(tgm, 1,1);
    push_texture();
    //hud_def  (c1, 9,9);
    set_max_damage(48);
    set_description("Rock is a block found in the deeper parts of the asteroid. Its main use is as a\n"
                    "strong building material. It is better to mine it using a tool, due to the time\n"
                    "it takes to destroy a single rock block.\n");

    cube_def(SolidCube, "coal", CUBE_MATERIAL_DIRT);
    iso_texture(tgm, 6,1);
    push_texture();
    set_max_damage(32);
    set_description("Coal, in its crystal form - graphite, is a very useful smelting material, providing lots\n"
                    "of heat for a long time. It does not have the annoying habit of evaporating that methane.\n"
                    "ice has. Explosives can be made out of coal powdered in a crusher.\n");

    cube_def(SolidCube, "iron_ore", CUBE_MATERIAL_STONE);
    iso_texture(tgm, 2,1);
    push_texture();
    set_max_damage(64);
    set_description("Iron is a metal used for nearly everything. In Gnomescroll, you need to smelt it\n"
                    "in a furnace to create iron bars, ready for crafting.\n");

    cube_def(SolidCube, "battery2", CUBE_MATERIAL_STONE);
    side_texture(B, c0, 2,2);
    side_texture(N, c0, 2,3);
    side_texture(E, c0, 2,3);
    side_texture(S, c0, 2,3);
    side_texture(W, c0, 2,3);
    side_texture(T, c0, 2,4);
    push_texture();
    set_max_damage(64);
    set_description("This is the pearl of alien technology, the iridium-ion battery. It has got enough\n"
                    "power stored in its tiny inner circuits to supply a whole alien city for days.\n"
                    "After this type of battery was invented, energy research became much simpler,\n"
                    "speeding up the process of making new discoveries by alien scientists. Due to\n"
                    "this fact, nearly every lab on Gnome is equipped with a few of these, some piled\n"
                    "on top of each other to create enormous Kah'rix generators.\n");

    cube_def(SolidCube, "blue_glow", CUBE_MATERIAL_STONE);
    iso_texture(c0, 1,3);
    push_texture();
    set_max_damage(64);
    set_description("This glowing block is an alien-manufactured hovering platform, using antigravity\n"
                    "to keep itself afloat. You will find several of these peculiar blocks in alien cities.\n");

    cube_def(SolidCube, "green_glow", CUBE_MATERIAL_STONE);
    iso_texture(c0, 1,4);
    push_texture();
    set_max_damage(64);
    set_description("This glowing block is an alien-manufactured hovering platform, using antigravity\n"
                    "to keep itself afloat. You will find several of these peculiar blocks in alien cities.\n");

    cube_def(SolidCube, "copper_ore", CUBE_MATERIAL_STONE);
    iso_texture(tgm, 3,1);
    push_texture();
    set_max_damage(64);
    set_description("Copper is a golden-coloured mineral found underground. It is used mainly for copper wires,\n"
                    "but also for armor and crafting, mainly of electronic components. As all ores, you need to\n"
                    "smelt it in a furnace before crafting.\n");

    cube_def(SolidCube, "gallium_ore", CUBE_MATERIAL_STONE);
    iso_texture(tgm, 4,1);
    push_texture();
    set_max_damage(64);
    set_description("Gallium is a rare metal found deep underground, which melts very easily and is useful\n"
                    "for all sorts of things. It is very soft and thus makes weak shovels, but its value is\n"
                    "compensated by the varied range of circuits and components using it. Like the other ores,\n"
                    "smelting is required to obtain a gallium bar.\n");

    cube_def(SolidCube, "iridium_ore", CUBE_MATERIAL_STONE);
    iso_texture(tgm, 5,1);
    push_texture();
    set_max_damage(64);
    set_description("Iridium is a rare element used in nuclear reactors. In Gnomescroll, you can make the\n"
                    "finest shovels out of this material, as well as use it for other crafting recipes\n"
                    "such as lasers. After mining, you get the ore, which you need to smelt in a furnace.\n");

    cube_def(SolidCube, "silicon_ore", CUBE_MATERIAL_STONE);
    iso_texture(iceflame, 3,2);
    push_texture();
    set_max_damage(64);
    set_description("Silicon is an extremely useful element found only in meteors. The main use is electonics,\n"
                    "but the crystal dagger also requires silicon for its recipe. Its power can only be truly\n"
                    "witnessed from inside the legendary Sil'acia factory, where thousands of alien-designed\n"
                    "circuits are created every day.\n");

    cube_def(SolidCube, "bedrock");
    iso_texture(c1, 10,7);
    push_texture();
    set_max_damage(INVINCIBLE_CUBE_DAMAGE);
    set_description("This is bedrock, a block that no-one has ever managed to destroy. There are, however,\n"
                    "tales of the unspoken horror of the Subterranean Diver, who once found a hole in the\n"
                    "bedrock layer and decided to jump inside. It is said that the void underneath consumed\n"
                    "him whole, leaving only the memory in the eyes of a witness, who went mad soon after.\n");

    cube_def(SolidCube, "control_node", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 9,4);
    push_texture();
    //hud_def    (c1, 9,4);
    set_description("This is a control node, used for marking territory in case someone wants to infiltrate\n"
                    "your base and then say they did not know it was inhabited.\n");

    // terrain features
    cube_def(SolidCube, "space_tree_trunk1", CUBE_MATERIAL_DECORATION);
    iso_texture(cc, 8,8);
    push_texture();
    hud_def    (cc, 8,8);
    set_description("This is the trunk of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "space_tree_trunk2", CUBE_MATERIAL_DECORATION);
    iso_texture(cc, 8,7);
    push_texture();
    hud_def    (cc, 8,7);
    set_description("This is the trunk of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "space_tree_trunk3", CUBE_MATERIAL_DECORATION);
    iso_texture(cc, 8,6);
    push_texture();
    hud_def    (cc, 8,6);
    set_description("This is the trunk of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "leaves1", CUBE_MATERIAL_DECORATION);
    iso_texture(tree_00, 1,1); // maroon
    push_texture();
    set_max_damage(12);
    set_description("These are the leaves of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "leaves2", CUBE_MATERIAL_DECORATION);
    iso_texture(tree_00, 1,2); // blue
    push_texture();
    set_max_damage(12);
    set_description("These are the leaves of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "leaves3", CUBE_MATERIAL_DECORATION);
    iso_texture(tree_00, 1,3); // purple
    push_texture();
    set_max_damage(12);
    set_description("These are the leaves of a space tree. The Gnome Nature Preservation Authority kindly\n"
                    "requests you to respect the hundreds of years of growth and keep this tree alive\n"
                    "for a healthier and richer environment.\n");

    cube_def(SolidCube, "mushroom_stem1", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 1,1);
    push_texture();
    hud_def    (c1, 1,1);
    set_max_damage(20);
    set_description("This is the stem of a mushroom commonly found in the Delta Polaris system. Further\n"
                    "information on these species can be found on the Gnomescroll Wiki.\n");

    cube_def(SolidCube, "mushroom_stem2", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 2,2);
    push_texture();
    hud_def    (c1, 2,2);
    set_max_damage(20);
    set_description("This is the stem of a mushroom commonly found in the Delta Polaris system. Further\n"
                    "information on these species can be found on the Gnomescroll Wiki.\n");

    cube_def(SolidCube, "mushroom_cap1", CUBE_MATERIAL_DECORATION);
//  iso_texture(t8, 8,9);
    iso_texture(cc, 1,2);
    push_texture();
    set_max_damage(20);
    set_description("This is the cap of a mushroom commonly found in the Delta Polaris system. Further\n"
                    "information on these species can be found on the Gnomescroll Wiki.\n");

    cube_def(SolidCube, "mushroom_cap2", CUBE_MATERIAL_DECORATION);
//  iso_texture(t8, 8,9);
    iso_texture(cc, 1,4);
    push_texture();
    set_max_damage(20);
    set_description("This is the cap of a mushroom commonly found in the Delta Polaris system. Further\n"
                    "information on these species can be found on the Gnomescroll Wiki.\n");

    cube_def(SolidCube, "mushroom_cap3", CUBE_MATERIAL_DECORATION);
    iso_texture(cc, 1,5);
    push_texture();
    set_max_damage(20);
    set_description("This is the cap of a mushroom commonly found in the Delta Polaris system. Further\n"
                    "information on these species can be found on the Gnomescroll Wiki.\n");

    // ruins

    cube_def(SolidCube, "ruins_floor1");
    iso_texture(c0, 3,2);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_floor2");
    iso_texture(c0, 3,3);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_floor3");
    iso_texture(c0, 3,4);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_floor4");
    iso_texture(c1, 4,7);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");


    cube_def(SolidCube, "ruins_wall1");
    iso_texture(c1, 7,6); // peach
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_wall2");
    iso_texture(c1, 1,2);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_wall3");
    iso_texture(c1, 6,2);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_wall4");
    iso_texture(c1, 1,6);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");


    cube_def(SolidCube, "ruins_ceiling1");
    iso_texture(c1, 2,1);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_ceiling2");
    iso_texture(c1, 4,5);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_ceiling3");
    iso_texture(c1, 5,6);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_ceiling4");
    iso_texture(c1, 5,2);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_trim1");
    iso_texture(c1, 8,4);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_trim2");
    iso_texture(c1, 8,5);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_trim3");
    iso_texture(c1, 8,6);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    cube_def(SolidCube, "ruins_trim4");
    iso_texture(c1, 8,7);
    push_texture();
    set_max_damage(RUINS_DMG);
    set_description("This block makes up the ruins of an old dungeon. The Gnome History Preservation\n"
                    "Organisation asks you to avoid any damage to the structure, for the knowledge that\n"
                    "might save our civilization from destruction, like the previous one.\n");

    // explosives
    cube_def(SolidCube, "plasmagen");
    iso_texture(cc, 8, 1);
    push_texture();
    p->explosive = true;
    set_max_damage(4);
    set_description("This is plasmagen, an explosive block. Try not to touch it, because it might\n"
                    "explode in your face.\n");

    cube_def(SolidCube, "rock_landmine", CUBE_MATERIAL_STONE);
    iso_texture(iceflame, 2,8);
    push_texture();
    p->explosive = true;
    set_max_damage(4);
    set_description("Rock is a block found in the deeper parts of the asteroid. Its main use is as a\n"
                    "strong building material. It is better to mine it using a tool, due to the time\n"
                    "it takes to destroy a single rock block.\n");


    cube_def(SolidCube, "regolith_landmine", CUBE_MATERIAL_DIRT);
    iso_texture(iceflame, 2,10);
    side_texture(T, iceflame, 2, 9);
    side_texture(B, iceflame, 3, 1);
    push_texture();
    p->explosive = true;
    set_max_damage(2);
    set_description("Regolith is a kind of dust commonly found on the surface of astronomical objects,\n"
                    "including asteroids and the moon. In Gnomescroll, the block is probably the most\n"
                    "accessible building material, but also one of the weakest.\n");

    cube_def(SolidCube, "city_green");
    iso_texture(iceflame, 3, 3);
    push_texture();
    set_max_damage(48);
    set_description("This is an alien-manufactured middle-quality building materials, used in cities due\n"
                    "to its very low weight and the easy of connecting two of these.\n");

    cube_def(SolidCube, "city_red");
    iso_texture(iceflame, 3, 4);
    push_texture();
    set_max_damage(48);
    set_description("This is an alien-manufactured middle-quality building materials, used in cities due\n"
                    "to its very low weight and the easy of connecting two of these.\n");

    cube_def(SolidCube, "city_blue");
    iso_texture(iceflame, 3, 5);
    push_texture();
    set_max_damage(48);
    set_description("This is an alien-manufactured middle-quality building materials, used in cities due\n"
                    "to its very low weight and the easy of connecting two of these.\n");

    cube_def(SolidCube, "city_gray");
    iso_texture(iceflame, 3, 6);
    push_texture();
    set_max_damage(48);
    set_description("This is an alien-manufactured middle-quality building materials, used in cities due\n"
                    "to its very low weight and the easy of connecting two of these.\n");

    cube_def(SolidCube, "temple_runes1");
    iso_texture(iceflame, 3, 7);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes2");
    iso_texture(iceflame, 3, 8);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes3");
    iso_texture(iceflame, 3, 9);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes4");
    iso_texture(iceflame, 3, 10);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes5");
    iso_texture(iceflame, 4, 1);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes6");
    iso_texture(iceflame, 4, 2);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes7");
    iso_texture(iceflame, 4, 3);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes8");
    iso_texture(iceflame, 4, 4);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes9");
    iso_texture(iceflame, 4, 5);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes10");
    iso_texture(iceflame, 4, 6);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes11");
    iso_texture(iceflame, 4, 7);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes12");
    iso_texture(iceflame, 4, 8);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes13");
    iso_texture(iceflame, 4, 9);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "temple_runes14");
    iso_texture(iceflame, 4, 10);
    push_texture();
    set_max_damage(56);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

    cube_def(SolidCube, "tile_floor");
    iso_texture(iceflame, 5, 1);
    push_texture();
    set_max_damage(64);
    set_description("These are runes in the alien alphabet. They are mainly used for religious purposes.\n");

/*

*/

    cube_def(SolidCube, "light_gem", CUBE_MATERIAL_DECORATION);
    iso_texture(c1, 9,4);
    push_texture();
    hud_def    (c1, 9,4);
    set_light_value(15);
    set_description("This is a light gem. It emits streams of photons due to its slight radioactivity. The\n"
                    "exact chemical composition is a secret, and the name LightGem is trademark of Ra'mex Inc.\n");
/*
    plant_example
*/

    cube_def(SolidCube, "plant_example_master", CUBE_MATERIAL_STONE);
    set_max_damage(128);
    set_description("This is a plant master.\n");

    iso_texture(    tree_01, 1, 1);
    side_texture(T, tree_01, 2, 2);
    side_texture(B, tree_01, 2, 2);
    push_texture();

    iso_texture(    tree_01, 2, 1);
    side_texture(T, tree_01, 2, 2);
    side_texture(B, tree_01, 2, 2);
    push_texture();

    iso_texture(    tree_01, 3, 1);
    side_texture(T, tree_01, 3, 2);
    side_texture(B, tree_01, 3, 2);
    push_texture();

    iso_texture(    tree_01, 4, 1);
    side_texture(T, tree_01, 4, 2);
    side_texture(B, tree_01, 4, 2);
    push_texture();

    cube_def(SolidCube, "plant_example_root", CUBE_MATERIAL_STONE);
    iso_texture(    tree_01, 1, 2);
    push_texture();
    set_description("This is a plant root.\n");

    cube_def(SolidCube, "plant_example_trunk", CUBE_MATERIAL_STONE);
    iso_texture(    tree_01, 1, 3);
    push_texture();
    set_description("This is a plant trunk.\n");

    cube_def(SolidCube, "plant_example_trunk_dead", CUBE_MATERIAL_STONE);
    iso_texture(    tree_01, 2, 3);
    push_texture();
    set_description("This is a dead plant trunk.\n");

    cube_def(SolidCube, "plant_example_leaves", CUBE_MATERIAL_STONE);
    iso_texture(    tree_01, 1, 4);
    push_texture();
    iso_texture(    tree_01, 2, 4);
    push_texture();
    set_description("These are the leaves of a plant.\n");

    cube_def(SolidCube, "plant_example_leaves_dead", CUBE_MATERIAL_STONE);
    iso_texture(    tree_01, 3, 4);
    push_texture();
    set_description("These are dead leaves.\n");


    end_cube_def();  // finalize

    apply_block_dat_changes();

    verify_config(); // validate

    save_cube_names();  // safe to save cube names now
}

#undef RUINS_DMG

}   // t_map
