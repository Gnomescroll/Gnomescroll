#include "drop_dat.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_properties.hpp>
#include <t_map/config/_interface.hpp>

#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_map
{

static void add_drop_callback(int block_id)
{
    ASSERT_VALID_BLOCK(block_id);
    IF_INVALID_BLOCK(block_id) return;
    GS_ASSERT(cube_list != NULL);
    if (cube_list == NULL) return;
    cube_list[block_id].item_drop = true;
}

class Item::ItemDropConfig* block_drop_dat = NULL;

void init_block_drop_dat()
{
    GS_ASSERT(block_drop_dat == NULL);
    block_drop_dat = new Item::ItemDropConfig;
    block_drop_dat->init("block", MAX_CUBES);
    block_drop_dat->get_id_from_name = &dat_get_cube_id;
    block_drop_dat->get_name_from_id = &get_cube_name;
    block_drop_dat->add_drop_callback = &add_drop_callback;
}

void teardown_block_drop_dat()
{
    if (block_drop_dat != NULL) delete block_drop_dat;
}

void load_block_drop_dat()
{
    GS_ASSERT(block_drop_dat != NULL);
    if (block_drop_dat == NULL) return;
    class Item::ItemDropConfig* b = block_drop_dat;
    
    ////////////////////
    b->def_drop("regolith");

        b->add_drop("regolith", 1);
        b->set_drop(0.99f, 1);

        b->add_drop("synthesizer_coin", 3);
        b->set_drop(0.15f, 1);
        b->set_drop(0.04f, 2);
        b->set_drop(0.02f, 3);
            
    b->def_drop("rock");

        b->add_drop("rock", 1);
        b->set_drop(0.99f, 1);

        b->add_drop("synthesizer_coin", 3);
        b->set_drop(0.20f, 1);
        b->set_drop(0.07f, 2);
        b->set_drop(0.04f, 3);
            
    b->def_drop("small_storage");
        b->drop_always("small_storage");

    ////////////////////
    b->def_drop("small_crafting_bench");
        b->drop_always("small_crafting_bench");

    b->def_drop("cryofreezer_1");
        b->drop_always("cryofreezer_1");

    b->def_drop("steel_block_1");
        b->drop_always("steel_block_1");

    b->def_drop("steel_block_2");
        b->drop_always("steel_block_2");

    b->def_drop("steel_block_3");
        b->drop_always("steel_block_3");

    b->def_drop("control_node");
        b->drop_always("control_node");

    b->def_drop("smelter_1");
        b->drop_always("smelter_1");

    b->def_drop("crusher");
        b->drop_always("crusher");

    b->def_drop("methane_ice");
        b->add_drop("methane_ice", 4);
        b->set_drop(0.40f, 1);
        b->set_drop(0.30f, 2);
        b->set_drop(0.15f, 3);
        b->set_drop(0.05f, 4);
        //normalize_drops_to(1.0f);

    b->def_drop("iron_ore");
        b->add_drop("iron_ore", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.20f, 2);
        b->set_drop(0.10f, 3);

    b->def_drop("copper_ore");
        b->add_drop("copper_ore", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.20f, 2);
        b->set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);


    b->def_drop("gallium_ore");
        b->add_drop("gallium_ore", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.20f, 2);
        b->set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    b->def_drop("iridium_ore");
        b->add_drop("iridium_ore", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.20f, 2);
        b->set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    b->def_drop("coal");
        b->add_drop("coal", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.20f, 2);
        b->set_drop(0.10f, 3);
        //normalize_drops_to(1.0f);

    b->end();

    b->save_to_file();
}

void handle_block_drop(int x, int y, int z, int block_type)
{
    GS_ASSERT(block_drop_dat != NULL);
    if (block_drop_dat == NULL) return;
    
    for (int i=0; i < block_drop_dat->meta_drop_table[block_type].num_drop; i++)
    {
        struct Item::ItemDropTable* cidt = &block_drop_dat->item_drop_table[i+block_drop_dat->meta_drop_table[block_type].index];
        float p = randf();

        if (p <= cidt->drop_cumulative_probabilities[0]) continue;

        for (int j=1; j < cidt->drop_entries; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_cumulative_probabilities[j])
            {
                for (int k=0; k<cidt->item_drop_num[j]; k++)
                {
                    const float mom = 2.0f;
                    x = (float)((float)x + 0.5f + randf()*0.33f);
                    y = (float)((float)y + 0.5f + randf()*0.33f);
                    z = (float)((float)z + 0.05f);
                    ItemParticle::create_item_particle(cidt->item_type, x, y, z, 
                        (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom); 
                }
                break;
            }
        }
    }
}

}   // t_map
