#include "drop_dat.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_mech
{

static void add_drop_callback(int id)
{
    IF_ASSERT(!isValid((MechType)id)) return;
    IF_ASSERT(mech_attributes == NULL) return;
    mech_attributes[id].item_drop = true;
}

class Item::ItemDropConfig* drop_dat = NULL;

static int get_mech_type_ptr(const char* name)
{   // hides the MechType from the drop dat
    return (int)get_mech_type(name);
}

static const char* get_mech_name_ptr(int mech_type)
{   // hides the MechType from the drop dat
    return get_mech_name((MechType)mech_type);
}

void init_drop_dat()
{
    GS_ASSERT(drop_dat == NULL);
    drop_dat = new Item::ItemDropConfig;
    drop_dat->init("mech", MAX_MECHS);  // TODO -- accept NULL_ID parameter
    drop_dat->get_id_from_name = &get_mech_type_ptr;
    drop_dat->get_name_from_id = &get_mech_name_ptr;
    drop_dat->add_drop_callback = &add_drop_callback;
}

void teardown_drop_dat()
{
    delete drop_dat;
}

void load_drop_dat()
{
    IF_ASSERT(drop_dat == NULL) return;
    class Item::ItemDropConfig* b = drop_dat;

    ////////////////////
    // look at t_map/config/drop_dat.cpp for more configuration examples

    b->def_drop("blue_crystal");
        b->add_drop("blue_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    b->def_drop("red_crystal");
        b->add_drop("red_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    b->def_drop("green_crystal");
        b->add_drop("green_crystal", 3);
        b->set_drop(0.70f, 1);
        b->set_drop(0.15f, 2);
        b->set_drop(0.075f, 3);

    //acadia flower stages
    b->def_drop("acadia_flower_stage_0");
        b->add_drop("acadia_seed", 1);
        b->set_drop(0.99f, 1);

    b->def_drop("acadia_flower_stage_1");
        b->add_drop("acadia_seed", 2);
        b->set_drop(0.80f, 1);
        b->set_drop(0.19f, 2);

        b->add_drop("acadia_fruit", 1);
        b->set_drop(0.50f, 1);

    b->def_drop("acadia_flower_stage_2");
        b->add_drop("acadia_seed", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);

        b->add_drop("acadia_fruit", 3);
        b->set_drop(0.50f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);


    b->def_drop("cryptid_larvae_stage_0");
        b->add_drop("cryptid_larvae", 1);
        b->set_drop(0.99f, 1);

    b->def_drop("cryptid_larvae_stage_1");
        b->add_drop("cryptid_larvae", 2);
        b->set_drop(0.50f, 1);
        b->set_drop(0.24f, 2);

    b->def_drop("cryptid_larvae_stage_2");
        b->add_drop("cryptid_larvae", 3);
        b->set_drop(0.25f, 1);
        b->set_drop(0.25f, 2);
        b->set_drop(0.24f, 3);

    b->def_drop("cryptid_larvae_stage_3");
        b->add_drop("cryptid_larvae", 3);
        b->set_drop(0.33f, 1);
        b->set_drop(0.33f, 2);
        b->set_drop(0.33f, 3);

    b->end();

    b->save_to_file();
}

void handle_drop(const Vec3i& position, int type)
{
    IF_ASSERT(drop_dat == NULL) return;

    const float MOMENTUM_FORCE = 2.0f;
    Vec3 pos = vec3_add(vec3_init(position), vec3_init(0.5f, 0.5f, 0.05f));

    for (int i=0; i < drop_dat->meta_drop_table[type].num_drop; i++)
    {
        int index = i + drop_dat->meta_drop_table[type].index;
        struct Item::ItemDropTable* cidt = &drop_dat->item_drop_table[index];
        float p = randf();

        if (p <= cidt->drop_cumulative_probabilities[0]) continue;

        for (int j=1; j < cidt->drop_entries; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_cumulative_probabilities[j])
            {
                for (int k=0; k<cidt->item_drop_num[j]; k++)
                {
                    Vec3 mom = vec3_scalar_mult(vec3_rand_center(), MOMENTUM_FORCE);
                    mom.z = MOMENTUM_FORCE;
                    Vec3 perturb = vec3_scalar_mult(vec3_rand(), 0.33f);
                    perturb.z = 0.0f;
                    Vec3 q = vec3_add(pos, perturb);
                    ItemParticle::create_item_particle(cidt->item_type, q, mom);
                }
                break;
            }
        }
    }
}

}   // t_mech
