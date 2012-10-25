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

static void add_drop_callback(int block_id)
{
    ASSERT_VALID_CUBE_ID(block_id);
    IF_INVALID_CUBE_ID(block_id) return;
    GS_ASSERT(cube_properties != NULL);
    if (cube_properties == NULL) return;
    cube_properties[block_id].item_drop = true;
}

// wrappers for the drop dat ptr config
static int get_cube_id_ptr(const char* name)
{
    return (int)get_cube_id(name);
}

static const char* get_cube_name_ptr(int cube_id)
{
    return get_cube_name((CubeID)cube_id);
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
        if (!isValidCube((CubeID)i)) continue;  // skips empty and error blocks
        if (defined_drops[i] != DROP_UNDEFINED) continue;

        const char* name = get_cube_name((CubeID)i);
        GS_ASSERT(name != NULL);
        if (name == NULL) continue;
        
        block_drop_dat->def_drop(name);
        GS_ASSERT(Item::get_item_type(name) != NULL_ITEM_TYPE);
        block_drop_dat->drop_always(name);   // assumes the block name matches the item name
    }
}
#endif

void def_drop(const char* block_name)
{
    GS_ASSERT_ABORT(defined_drops != NULL);
    if (defined_drops == NULL) return;

    CubeID cube_id = get_cube_id(block_name);
    GS_ASSERT_ABORT(isValidCube(cube_id));
    if (!isValidCube(cube_id)) return;
    
    GS_ASSERT_ABORT(defined_drops[cube_id] == DROP_UNDEFINED);
    
    defined_drops[cube_id] = DROP_DEFINED;

    #if DC_SERVER
    block_drop_dat->def_drop(block_name);
    #endif
}

void no_drop(const char* block_name)
{   // prevents block from automatically dropping itself
    GS_ASSERT_ABORT(defined_drops != NULL);
    if (defined_drops == NULL) return;

    CubeID cube_id = get_cube_id(block_name);
    GS_ASSERT_ABORT(isValidCube(cube_id));
    if (!isValidCube(cube_id)) return;

    GS_ASSERT_ABORT(defined_drops[cube_id] == DROP_UNDEFINED);
    
    defined_drops[cube_id] = DROP_NEVER; 
}

void drop_always(const char* item_name)
{
    #if DC_SERVER
    block_drop_dat->drop_always(item_name);
    #endif
}

#if DC_SERVER
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
    block_drop_dat->get_id_from_name = &get_cube_id_ptr;
    block_drop_dat->get_name_from_id = &get_cube_name_ptr;
    block_drop_dat->add_drop_callback = &add_drop_callback;
    #endif
}

void teardown_block_drop_dat()
{
    if (defined_drops != NULL) free(defined_drops);
    
    #if DC_SERVER
    if (block_drop_dat != NULL) delete block_drop_dat;
    #endif
}

#if DC_SERVER
void handle_block_drop(int x, int y, int z, CubeID cube_id)
{
    GS_ASSERT(block_drop_dat != NULL);
    if (block_drop_dat == NULL) return;
    
    for (int i=0; i < block_drop_dat->meta_drop_table[cube_id].num_drop; i++)
    {
        struct Item::ItemDropTable* cidt = &block_drop_dat->item_drop_table[i+block_drop_dat->meta_drop_table[cube_id].index];
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
#endif

}   // t_map
