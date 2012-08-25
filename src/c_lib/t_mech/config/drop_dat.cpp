#include "drop_dat.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_mech
{

static void add_drop_callback(int id)
{
    ASSERT_VALID_MECH_TYPE(id);
    IF_INVALID_MECH_TYPE(id) return;
    GS_ASSERT(mech_attribute != NULL);
    if (mech_attribute == NULL) return;
    mech_attribute[id].item_drop = true;
}

class Item::ItemDropConfig* drop_dat = NULL;

void init_drop_dat()
{
    GS_ASSERT(drop_dat == NULL);
    drop_dat = new Item::ItemDropConfig;
    drop_dat->init("mech", MAX_MECHS);
    drop_dat->get_id_from_name = &get_mech_type;
    drop_dat->get_name_from_id = &get_mech_name;
    drop_dat->add_drop_callback = &add_drop_callback;
}

void teardown_drop_dat()
{
    if (drop_dat != NULL) delete drop_dat;
}

void load_drop_dat()
{
    GS_ASSERT(drop_dat != NULL);
    if (drop_dat == NULL) return;
    class Item::ItemDropConfig* b = drop_dat;
    
    ////////////////////
    // look at t_map/config/drop_dat.cpp for more configuration examples

    // TODO -- ore drop style

    b->def_drop("red_crystal");
        b->drop_always("red_crystal");

    b->def_drop("blue_crystal");
        b->drop_always("blue_crystal");

    b->def_drop("green_crystal");
        b->drop_always("green_crystal");

    b->end();

    b->save_to_file();
}

void handle_drop(int x, int y, int z, int type)
{
    GS_ASSERT(drop_dat != NULL);
    if (drop_dat == NULL) return;
    
    for (int i=0; i < drop_dat->meta_drop_table[type].num_drop; i++)
    {
        class Item::ItemDropTable* cidt = &drop_dat->item_drop_table[i+drop_dat->meta_drop_table[type].index];
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

}   // t_mech
