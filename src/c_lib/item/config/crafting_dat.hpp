#pragma once

#include <item/config/_interface.hpp>

#include <item/common/struct.hpp>
//#include <t_map/t_properties.hpp>

namespace Item
{

void def_crafting_recipe(const char* item_name);
void set_crafting_reagent(const char* item_name, int quantity);
void end_crafting_recipe();

}   // Item

namespace Item
{

void load_crafting_dat()
{
    def_crafting_recipe("steel_block_1", 4);
    set_crafting_reagent("iron_bar", 1);
    end_crafting_recipe();
    
    def_crafting_recipe("steel_block_2", 4);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("graphite", 1);
    end_crafting_recipe();
    
    def_crafting_recipe("steel_block_3", 4);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("graphite", 1);
    end_crafting_recipe();

    // Shovel
    def_crafting_recipe("copper_shovel");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("copper_bar", 1);
    end_crafting_recipe();

    def_crafting_recipe("gallium_shovel");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("gallium_bar", 1);
    end_crafting_recipe();

    def_crafting_recipe("iron_shovel");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    end_crafting_recipe();

    def_crafting_recipe("iridium_shovel");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("iridium_bar", 1);
    end_crafting_recipe();

    // Other
    def_crafting_recipe("food_rock-0");
    set_crafting_reagent("regolith", 1);
    set_crafting_reagent("regolith", 1);
    end_crafting_recipe();

    def_crafting_recipe("food_rock-0");
    set_crafting_reagent("regolith", 2);
    end_crafting_recipe();

    // Weapons
    //def_crafting_recipe("mining_laser");
    //set_crafting_reagent("copper_bar", 1);
    //set_crafting_reagent("blue_crystal", 1);
    //end_crafting_recipe();

    def_crafting_recipe("laser_rifle");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("quartz_crystal", 1);
    end_crafting_recipe();
}

}   // Item
