#pragma once

#include <item/config/_interface.hpp>

#include <item/common/struct.hpp>
//#include <t_map/t_properties.hpp>

namespace Item
{

void def_recipe(const char* item_name);
void set_reagent(const char* item_name, int quantity);
void end_recipe();

}   // Item

namespace Item
{

void load_crafting_dat()
{

    // Ore
    def_recipe("copper_bar");
    set_reagent("copper_ore", 1);
    set_reagent("copper_ore", 1);
    end_recipe();

    def_recipe("gallium_bar");
    set_reagent("gallium_ore", 1);
    set_reagent("gallium_ore", 1);
    end_recipe();

    def_recipe("iron_bar");
    set_reagent("iron_ore", 1);
    set_reagent("iron_ore", 1);
    end_recipe();
    
    def_recipe("iridium_bar");
    set_reagent("iridium_ore", 1);
    set_reagent("iridium_ore", 1);
    end_recipe();
    
    // Blocks
    def_recipe("steel_1", 4);
    set_reagent("iron_bar", 1);
    end_recipe();
    
    def_recipe("steel_2", 4);
    set_reagent("iron_bar", 1);
    set_reagent("graphite", 1);
    end_recipe();

    // Shovel
    def_recipe("copper_shovel");
    set_reagent("copper_bar", 1);
    set_reagent("copper_bar", 1);
    end_recipe();

    def_recipe("gallium_shovel");
    set_reagent("gallium_bar", 1);
    set_reagent("gallium_bar", 1);
    end_recipe();

    def_recipe("iron_shovel");
    set_reagent("iron_bar", 1);
    set_reagent("iron_bar", 1);
    end_recipe();

    def_recipe("iridium_shovel");
    set_reagent("iridium_bar", 1);
    set_reagent("iridium_bar", 1);
    end_recipe();

    // Other
    def_recipe("food_rock-0");
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    set_reagent("regolith", 1);
    end_recipe();

    def_recipe("food_rock-0");
    set_reagent("regolith", 2);
    set_reagent("regolith", 2);
    end_recipe();

    // Weapons
    def_recipe("mining_laser");
    set_reagent("copper_bar", 1);
    set_reagent("blue_cystal", 1);
    end_recipe();

    def_recipe("laser_rifle");
    set_reagent("copper_bar", 1);
    set_reagent("quartz_crystal", 1);
    end_recipe();

}

}   // Item
