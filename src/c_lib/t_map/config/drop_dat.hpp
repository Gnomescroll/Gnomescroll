#pragma once

#include <t_map/config/_interface.hpp>
#include <t_map/config/drop_table.hpp>

namespace t_map
{


    //add_drop(const char* item_name, float mean, float falloff, int max_drops)
    //float mean, float falloff, int max_drops
void load_map_drop_dat()
{
    ////////////////////
    def_drop("regolith");

        add_drop("regolith", 2);
        set_drop(0.35, 1);
        set_drop(0.20, 2);

        add_drop("copper_ore", 4);
        set_drop(0.06, 1);
        set_drop(0.02, 2);
        set_drop(0.01, 3);
        set_drop(0.01, 5);

        add_drop("gallium_ore", 1);
        set_drop(0.10, 1);

        add_drop("iron_ore", 2);
        set_drop(0.03, 1);
        set_drop(0.01, 2);

        add_drop("iridium_ore", 2);
        set_drop(0.03, 1);
        set_drop(0.01, 2);

        add_drop("food_rock-0", 2);
        set_drop(0.10, 1);
        set_drop(0.08, 2);
    ////////////////////
    def_drop("crate_2");
        drop_always("crate_2");

    ////////////////////
    def_drop("crate_3");
        drop_always("crate_3");

    def_drop("cryofreezer_1");
        drop_always("cryofreezer_1");

    def_drop("methane_ice");
        add_drop("methane_ice", 3);
        set_drop(0.50, 1);
        set_drop(0.35, 2);
        set_drop(0.15, 3);

    end_drop_dat();
    #if DC_CLIENT
    save_drop_dat_to_file();
    #endif
}


}   // t_map
