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
/*
    Add Steel bar -> Chain Link
    Add Chainlink armor
*/
namespace Item
{

void load_crafting_dat()
{
    def_crafting_recipe("steel_block_1", 4);
    set_crafting_reagent("iron_bar", 1);
    
    def_crafting_recipe("steel_block_2", 4);
    set_crafting_reagent("iron_bar", 1);
    
    def_crafting_recipe("steel_block_3", 4);
    set_crafting_reagent("iron_bar", 1);

    // ore -> bar recipes
    // these take 2x as much as it would take to smelt
    // but we need it to circumvent the smelter prerequisite introduced
    // if ore is only made by smelting
    def_crafting_recipe("copper_bar");
    set_crafting_reagent("copper_ore", 1);
    set_crafting_reagent("copper_ore", 1);

    def_crafting_recipe("iron_bar");
    set_crafting_reagent("iron_ore", 1);
    set_crafting_reagent("iron_ore", 1);

    // Shovel
    def_crafting_recipe("copper_shovel");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("copper_bar", 1);

    def_crafting_recipe("gallium_shovel");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("gallium_bar", 1);

    def_crafting_recipe("iron_shovel");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    def_crafting_recipe("iridium_shovel");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("iridium_bar", 1);

    def_crafting_recipe("small_storage");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    // Energy
    def_crafting_recipe("small_charge_pack");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("methane_ice", 1);
    
    def_crafting_recipe("small_charge_pack");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("methane_ice", 1);
    
    def_crafting_recipe("small_charge_pack");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("methane_ice", 1);
    
    def_crafting_recipe("small_charge_pack");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("methane_ice", 1);

    def_crafting_recipe("energy_tank");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("methane_ice", 4);
    
    def_crafting_recipe("energy_tank");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("methane_ice", 4);
    
    def_crafting_recipe("energy_tank");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("methane_ice", 4);
    
    def_crafting_recipe("energy_tank");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("methane_ice", 4);
    
    def_crafting_recipe("agent_spawner");
    set_crafting_reagent("copper_bar", 4);
    set_crafting_reagent("iron_bar", 4);
    set_crafting_reagent("gallium_bar", 4);
    set_crafting_reagent("iridium_bar", 4);
    
    def_crafting_recipe("energy_core");
    set_crafting_reagent("methane_ice", 16);
    set_crafting_reagent("coal", 8);
    set_crafting_reagent("iridium_bar", 4);
    set_crafting_reagent("copper_bar", 4);
    
    def_crafting_recipe("iron_rod", 2);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    def_crafting_recipe("iron_star");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    def_crafting_recipe("iron_blade");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    def_crafting_recipe("copper_wire");
    set_crafting_reagent("copper_bar", 1);

    def_crafting_recipe("iron_helmet");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("iron_bar", 1);

    def_crafting_recipe("gallium_helmet");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("gallium_bar", 1);

    def_crafting_recipe("copper_helmet");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("copper_bar", 1);

    def_crafting_recipe("iridium_helmet");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("iridium_bar", 1);

    // Turquoise mining laser. any bar + blue
    def_crafting_recipe("blue_mining_laser");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("blue_crystal", 1);
    
    def_crafting_recipe("blue_mining_laser");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("blue_crystal", 1);
    
    def_crafting_recipe("blue_mining_laser");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("blue_crystal", 1);
    
    def_crafting_recipe("blue_mining_laser");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("blue_crystal", 1);

    // Ruby mining laser. any bar + red
    def_crafting_recipe("red_mining_laser");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("red_crystal", 1);
    
    def_crafting_recipe("red_mining_laser");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("red_crystal", 1);
    
    def_crafting_recipe("red_mining_laser");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("red_crystal", 1);
    
    def_crafting_recipe("red_mining_laser");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("red_crystal", 1);

    // Malachite mining laser. any bar + green
    def_crafting_recipe("green_mining_laser");
    set_crafting_reagent("copper_bar", 1);
    set_crafting_reagent("green_crystal", 1);
    
    def_crafting_recipe("green_mining_laser");
    set_crafting_reagent("iron_bar", 1);
    set_crafting_reagent("green_crystal", 1);
    
    def_crafting_recipe("green_mining_laser");
    set_crafting_reagent("gallium_bar", 1);
    set_crafting_reagent("green_crystal", 1);
    
    def_crafting_recipe("green_mining_laser");
    set_crafting_reagent("iridium_bar", 1);
    set_crafting_reagent("green_crystal", 1);

    end_crafting_recipe(); // finalize
}

}   // Item
