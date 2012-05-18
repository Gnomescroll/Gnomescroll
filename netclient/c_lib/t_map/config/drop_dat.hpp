#pragma once

#include <t_map/config/_interface.hpp>
#include <t_map/config/drop_table.hpp>

namespace t_map
{


    //add_drop(const char* item_name, float mean, float falloff, int max_drops)
    //float mean, float falloff, int max_drops
void load_map_drop_dat()
{

    def_drop("regolith");
    add_drop("regolith", 1.0, 0.2, 2);
    add_drop("copper_ore", 1.0, 0.2, 2);
    add_drop("food_rock-0", 2.0, 0.5, 8);

    end_drop_dat();
}


}   // t_map
