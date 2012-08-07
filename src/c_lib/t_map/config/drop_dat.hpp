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

        add_drop("regolith", 1);
        set_drop(0.99f, 1);

        add_drop("synthesizer_coin", 3);
        set_drop(0.15f, 1);
        set_drop(0.04f, 2);
        set_drop(0.02f, 3);
        //add_drop("control_node", 2);
        //set_drop(0.40f, 1);
        //set_drop(0.40f, 2);
            
    def_drop("small_storage");
        drop_always("small_storage");

    ////////////////////
    def_drop("small_crafting_bench");
        drop_always("small_crafting_bench");

    def_drop("cryofreezer_1");
        drop_always("cryofreezer_1");

    def_drop("steel_block_1");
        drop_always("steel_block_1");

    def_drop("steel_block_2");
        drop_always("steel_block_2");

    def_drop("steel_block_3");
        drop_always("steel_block_3");

    def_drop("control_node");
        drop_always("control_node");

    def_drop("smelter_1");
        drop_always("smelter_1");

    def_drop("recycler");
        drop_always("recycler");

    def_drop("methane_ice");
        add_drop("methane_ice", 4);
        set_drop(0.40f, 1);
        set_drop(0.30f, 2);
        set_drop(0.15f, 3);
        set_drop(0.05f, 4);
        //normalize_drops_to(1.0f);

    def_drop("iron_ore");
        add_drop("iron_ore", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("copper_ore");
        add_drop("copper_ore", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);


    def_drop("gallium_ore");
        add_drop("gallium_ore", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    def_drop("iridium_ore");
        add_drop("iridium_ore", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    def_drop("graphite");
        add_drop("graphite", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    end_drop_dat();
    #if DC_CLIENT
    save_drop_dat_to_file();
    #endif
}


}   // t_map
