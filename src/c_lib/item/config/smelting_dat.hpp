#pragma once

#include <item/config/_interface.hpp>
#include <item/common/struct.hpp>

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
    set_smelting_reagent("copper_ore", 1);
    set_smelting_creation_time(30 * 3);
    end_smelting_recipe();

    def_smelting_recipe("iron_bar");
    set_smelting_reagent("iron_ore", 1);
    set_smelting_creation_time(30 * 5);
    end_smelting_recipe();
    
    def_smelting_recipe("gallium_bar");
    set_smelting_reagent("gallium_ore", 1);
    set_smelting_creation_time(30 * 10);
    end_smelting_recipe();

    def_smelting_recipe("iridium_bar");
    set_smelting_reagent("iridium_ore", 1);
    set_smelting_creation_time(30 * 15);
    end_smelting_recipe();
}

}   // Item
