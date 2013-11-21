/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "drop_dat.hpp"

#include <t_map/t_properties.hpp>
#include <t_map/config/_interface.hpp>

#include <item/properties.hpp>
#include <item/particle/_interface.hpp>

namespace t_map
{

DropDefinition* defined_drops = NULL;

#if DC_SERVER
class Item::ItemDropConfig* block_drop_dat = NULL;
#endif

void def_drop(const char* block_name)
{
    GS_ASSERT_ABORT(defined_drops != NULL);
    if (defined_drops == NULL) return;

    CubeType cube_type = get_cube_type(block_name);
    GS_ASSERT_ABORT(isValidCube(cube_type));
    IF_ASSERT(!isValidCube(cube_type)) return;

    GS_ASSERT_ABORT(defined_drops[cube_type] == DROP_UNDEFINED);

    defined_drops[cube_type] = DROP_DEFINED;

    #if DC_SERVER
    block_drop_dat->def_drop(block_name);
    #endif
}

void no_drop(const char* block_name)
{   // prevents block from automatically dropping itself
    GS_ASSERT_ABORT(defined_drops != NULL);
    if (defined_drops == NULL) return;

    CubeType cube_type = get_cube_type(block_name);
    GS_ASSERT_ABORT(isValidCube(cube_type));
    IF_ASSERT(!isValidCube(cube_type)) return;

    GS_ASSERT_ABORT(defined_drops[cube_type] == DROP_UNDEFINED);

    defined_drops[cube_type] = DROP_NEVER;
}

void drop_always(const char* item_name)
{
    #if DC_SERVER
    block_drop_dat->drop_always(item_name);
    #endif
}

#if DC_SERVER
static void add_drop_callback(int _cube_type)
{
    CubeType cube_type = (CubeType)_cube_type;
    IF_ASSERT(!isValid(cube_type)) return;
    IF_ASSERT(cube_properties == NULL) return;
    CubeProperty* p = get_cube_properties(cube_type);
    IF_ASSERT(p == NULL) return;
    p->item_drop = true;
}

// wrappers for the drop dat ptr config
static int get_cube_type_ptr(const char* name)
{
    return (int)get_cube_type(name);
}

static const char* get_cube_name_ptr(int cube_type)
{
    return get_cube_name((CubeType)cube_type);
}

void apply_automatic_block_drops()
{
    // this will make all blocks drop their associated block item,
    // if they have no other drops defined
    GS_ASSERT_ABORT(block_drop_dat != NULL);
    if (block_drop_dat == NULL) return;
    GS_ASSERT_ABORT(defined_drops != NULL);
    if (defined_drops == NULL) return;
    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!isValidCube((CubeType)i)) continue;  // skips empty and error blocks
        if (defined_drops[i] != DROP_UNDEFINED) continue;

        const char* name = get_cube_name((CubeType)i);
        GS_ASSERT_ABORT(name != NULL);
        if (name == NULL) continue;

        GS_ASSERT_ABORT(Item::get_item_type(name) != NULL_ITEM_TYPE);

        def_drop(name);
        drop_always(name);   // assumes the block name matches the item name
    }
}

void add_drop(const char* item_drop, int drop_entries)
{
    block_drop_dat->add_drop(item_drop, drop_entries);
}

void set_drop(float drop_probabilities, int drops)
{
    block_drop_dat->set_drop(drop_probabilities, drops);
}

void end_drop_dat()
{
    block_drop_dat->end();
    block_drop_dat->save_to_file();
    for (size_t i=0; i<cube_properties->max; i++)
        cube_properties->properties[i].item_drop = (defined_drops[i] == DROP_DEFINED);
}
#endif

#if DC_CLIENT
# define add_drop(x,y) ;
# define set_drop(x,y) ;
# define end_drop_dat() ;
# define save_drop_file() ;
#endif


void load_block_drop_dat()
{

    // if no drops defined for a block, it will automatically drop itself
    // if you want it to drop absolutely nothing, use no_drop(const char*)

    // if you need to override this behaviour (e.g. regolith drops regolith + synthesizer coins)
    // then you need to define the block-item yourself in item_dat. it will not be automatically
    // generated

    no_drop("bedrock");
    no_drop("plasmagen");
    no_drop("rock_landmine");
    no_drop("regolith_landmine");

    ////////////////////
    def_drop("regolith");
        add_drop("regolith", 1);
        set_drop(0.99f, 1);

        add_drop("synthesizer_coin", 3);
        set_drop(0.15f, 1);
        set_drop(0.04f, 2);
        set_drop(0.02f, 3);

    def_drop("rock");
        add_drop("rock", 1);
        set_drop(0.99f, 1);

        add_drop("synthesizer_coin", 3);
        set_drop(0.20f, 1);
        set_drop(0.07f, 2);
        set_drop(0.04f, 3);

    def_drop("methane_ice");
        add_drop("methane_ice_chunk", 4);
        set_drop(0.40f, 1);
        set_drop(0.30f, 2);
        set_drop(0.15f, 3);
        set_drop(0.05f, 4);

    def_drop("iron_ore");
        add_drop("iron_ore_piece", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("copper_ore");
        add_drop("copper_ore_piece", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("gallium_ore");
        add_drop("gallium_ore_piece", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("iridium_ore");
        add_drop("iridium_ore_piece", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("silicon_ore");
        add_drop("silicon_ore_piece", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);

    def_drop("coal");
        add_drop("coal_nugget", 3);
        set_drop(0.50f, 1);
        set_drop(0.20f, 2);
        set_drop(0.10f, 3);
}

void init_block_drop_dat()
{
    GS_ASSERT(defined_drops == NULL);
    defined_drops = (DropDefinition*)malloc(MAX_CUBES * sizeof(DropDefinition));
    for (int i=0; i<MAX_CUBES; defined_drops[i++] = DROP_UNDEFINED);

    #if DC_SERVER
    GS_ASSERT(block_drop_dat == NULL);
    block_drop_dat = new Item::ItemDropConfig;
    block_drop_dat->init("block", MAX_CUBES);
    block_drop_dat->get_id_from_name = &get_cube_type_ptr;
    block_drop_dat->get_name_from_id = &get_cube_name_ptr;
    block_drop_dat->add_drop_callback = &add_drop_callback;
    #endif
}

void teardown_block_drop_dat()
{
    free(defined_drops);
    #if DC_SERVER
    delete block_drop_dat;
    #endif
}

#if DC_SERVER
void handle_block_drop(const Vec3i& position, CubeType cube_type)
{
    IF_ASSERT(block_drop_dat == NULL) return;
    IF_ASSERT(!isValid(cube_type)) return;

    const float MOMENTUM_SCALE = 2.0f;
    static const Vec3 perturb = vec3_init(0.33f, 0.33f, 0.0f);
    Vec3 pos = vec3_add(vec3_init(position), vec3_init(0.5f, 0.5f, 0.05f));

    for (int i=0; i < block_drop_dat->meta_drop_table[cube_type].num_drop; i++)
    {
        struct Item::ItemDropTable* cidt = &block_drop_dat->item_drop_table[i+block_drop_dat->meta_drop_table[cube_type].index];
        float p = randf();

        if (p <= cidt->drop_cumulative_probabilities[0]) continue;

        for (int j=1; j < cidt->drop_entries; j++)
        {
            //check each drop until commulative probability table hit
            if (p <= cidt->drop_cumulative_probabilities[j])
            {
                for (int k=0; k<cidt->item_drop_num[j]; k++)
                {
                    Vec3 mom = vec3_scalar_mult(vec3_rand_center(), MOMENTUM_SCALE);
                    mom.z = MOMENTUM_SCALE;
                    Vec3 q = vec3_add(pos, vec3_mult(vec3_rand(), perturb));
                    ItemParticle::create_item_particle(cidt->item_type, q, mom);
                }
                break;
            }
        }
    }
}
#endif

}   // t_map
