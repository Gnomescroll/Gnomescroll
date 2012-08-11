#include "crusher_dat.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/config/item_drop.hpp>

namespace ItemContainer
{

float crusher_item_jump_out_velocity()
{
    static const float m = 1.5f;
    return ((randf()*0.5f) + 1.0f) * m;
}

static class Item::ItemDrop** drops = NULL;

/* Configuration Loader */ 
    
static class Item::ItemDrop* d = NULL;

static void set_crusher_drop(int item_type)
{
    GS_ASSERT(drops != NULL);
    if (drops == NULL) return;

    GS_ASSERT(item_type != NULL_ITEM);
    GS_ASSERT(item_type >= 0 && item_type < MAX_ITEMS);
    if (item_type < 0 || item_type >= MAX_ITEMS) return;

    GS_ASSERT(drops[item_type] == NULL);
    if (drops[item_type] != NULL) return;
    drops[item_type] = new Item::ItemDrop;
    d = drops[item_type];
}

static void crusher_def(const char* name)
{
    int item_type = Item::get_item_type(name);
    GS_ASSERT(item_type != NULL_ITEM);
    set_crusher_drop(item_type);
}

static void register_crusher_settings()
{
    crusher_def("regolith");
    d->set_max_drop_types(6);
    
    d->set_max_drop_amounts("powdered_regolith", 3);
    d->add_drop("powdered_regolith", 1, 0.85f);
    d->add_drop("powdered_regolith", 2, 0.12f);
    d->add_drop("powdered_regolith", 3, 0.03f);

    d->set_max_drop_amounts("copper_ore", 1);
    d->add_drop("copper_ore", 1, 0.005f);
    
    d->set_max_drop_amounts("iron_ore", 1);
    d->add_drop("iron_ore", 1, 0.005f);

    d->set_max_drop_amounts("gallium_ore", 1);
    d->add_drop("gallium_ore", 1, 0.0025f);

    d->set_max_drop_amounts("iridium_ore", 1);
    d->add_drop("iridium_ore", 1, 0.0025f);

    d->set_max_drop_amounts("graphite", 1);
    d->add_drop("graphite", 1, 0.005f);
    
}

void validate_crusher_settings()
{
    for (int i=0; i<MAX_ITEMS; i++)
        GS_ASSERT(drops[i] == NULL || drops[i]->is_loaded() != 0);
}

void load_crusher_dat()
{
    _started = 0;
    register_crusher_settings();
    validate_crusher_settings();
}


/* Public Interface */

void init_crusher_dat()
{
    GS_ASSERT(drops == NULL);
    drops = (class Item::ItemDrop**)calloc(MAX_ITEMS, sizeof(class Item::ItemDrop*));
}

void teardown_crusher_dat()
{
    if (drops != NULL)
    {
        for (int i=0; i<MAX_ITEMS; i++)
            if (drops[i] != NULL) delete drops[i];
        free(drops);
    }
}

class Item::ItemDrop* get_crusher_drop(int item_type)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    GS_ASSERT(item_type >= 0 && item_type < MAX_ITEMS);
    if (item_type < 0 || item_type >= MAX_ITEMS) return NULL;
    return drops[item_type];
}

}   // ItemContainer
