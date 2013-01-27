#pragma once

#include <item/config/_interface.hpp>

namespace Item
{

void def_smelting_recipe(const char* item_name);
void add_smelting_product(const char* item_name);
void set_smelting_reagent(const char* item_name, int quantity);
void end_smelting_recipe();

}   // Item

namespace Item
{

void load_smelting_dat()
{
    // Ore
    def_smelting_recipe("copper_bar");
    set_smelting_reagent("copper_ore_piece", 1);
    set_smelting_creation_time(30 * 3);

    def_smelting_recipe("iron_bar");
    set_smelting_reagent("iron_ore_piece", 1);
    set_smelting_creation_time(30 * 5);

    def_smelting_recipe("gallium_bar");
    set_smelting_reagent("gallium_ore_piece", 1);
    set_smelting_creation_time(30 * 10);

    def_smelting_recipe("iridium_bar");
    set_smelting_reagent("iridium_ore_piece", 1);
    set_smelting_creation_time(30 * 15);

    def_smelting_recipe("silicon_crystal");
    set_smelting_reagent("silicon_ore_piece", 1);
    set_smelting_creation_time(30 * 8);

    end_smelting_recipe();  // finalize
}

}   // Item
