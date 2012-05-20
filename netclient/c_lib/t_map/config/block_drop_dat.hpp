#pragma once

#include "config_util.hpp"
#include "drop_table.hpp"

namespace t_map
{


    //add_drop(const char* item_name, float mean, float falloff, int max_drops)
    //float mean, float falloff, int max_drops
void load_map_drop_dat()
{

    def_drop("regolith");
    add_drop("regolith", 2.0, 0.9, 2);
    add_drop("copper_ore", 2.0, 0.9, 2);
    add_drop("food_rock-0", 2.0, 0.8, 8);

    end_drop_dat();
}


}   // t_map
 