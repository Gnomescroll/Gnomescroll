/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "crusher_dat.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <item/config/item_drop.hpp>

namespace ItemContainer
{

float crusher_item_jump_out_velocity()
{
    static const float m = 1.5f;
    return ((randf()*0.5f) + 1.0f) * m;
}

static class Item::ItemDrop* drops;

/* Configuration Loader */

static class Item::ItemDrop* d = NULL;

static void set_crusher_drop(ItemType item_type)
{
    GS_ASSERT_ABORT(drops != NULL);
    if (drops == NULL) return;

    IF_ASSERT(!isValid(item_type))
    {
        GS_ASSERT_ABORT(false);
        return;
    }

    GS_ASSERT_ABORT(!drops[item_type].is_loaded());
    if (drops[item_type].is_loaded()) return;
    d = &drops[item_type];
}

static void crusher_def(const char* name)
{
    ItemType item_type = Item::get_item_type(name);
    GS_ASSERT_ABORT(item_type != NULL_ITEM_TYPE);
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

        d->set_max_drop_amounts("copper_ore_piece", 1);
        d->add_drop("copper_ore_piece", 1, 0.005f);

        d->set_max_drop_amounts("iron_ore_piece", 1);
        d->add_drop("iron_ore_piece", 1, 0.005f);

        d->set_max_drop_amounts("gallium_ore_piece", 1);
        d->add_drop("gallium_ore_piece", 1, 0.0025f);

        d->set_max_drop_amounts("iridium_ore_piece", 1);
        d->add_drop("iridium_ore_piece", 1, 0.0025f);

        d->set_max_drop_amounts("coal_nugget", 1);
        d->add_drop("coal_nugget", 1, 0.005f);

    crusher_def("rock");
        d->set_max_drop_types(6);

        d->set_max_drop_amounts("crushed_rock", 3);
        d->add_drop("crushed_rock", 1, 0.85f);
        d->add_drop("crushed_rock", 2, 0.12f);
        d->add_drop("crushed_rock", 3, 0.03f);

        d->set_max_drop_amounts("copper_ore_piece", 1);
        d->add_drop("copper_ore_piece", 1, 0.015f);

        d->set_max_drop_amounts("iron_ore_piece", 1);
        d->add_drop("iron_ore_piece", 1, 0.015f);

        d->set_max_drop_amounts("gallium_ore_piece", 1);
        d->add_drop("gallium_ore_piece", 1, 0.01f);

        d->set_max_drop_amounts("iridium_ore_piece", 1);
        d->add_drop("iridium_ore_piece", 1, 0.01f);

        d->set_max_drop_amounts("coal_nugget", 1);
        d->add_drop("coal_nugget", 1, 0.01f);

    crusher_def("iron_rod");
        d->set_max_drop_types(1);
        d->set_max_drop_amounts("iron_bar", 1);
        d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("iron_star");
        d->set_max_drop_types(1);
        d->set_max_drop_amounts("iron_bar", 1);
        d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("iron_blade");
        d->set_max_drop_types(1);
        d->set_max_drop_amounts("iron_bar", 1);
        d->add_drop("iron_bar", 1, 1.0f);

    crusher_def("coal_nugget");
        d->set_max_drop_types(1);
        d->set_max_drop_amounts("blackpowder", 5);
        d->add_drop("blackpowder", 1, 0.10f);
        d->add_drop("blackpowder", 3, 0.20f);
        d->add_drop("blackpowder", 5, 0.40f);
        d->add_drop("blackpowder", 7, 0.20f);
        d->add_drop("blackpowder", 9, 0.10f);

    crusher_def("silicon_crystal");
        d->set_max_drop_types(1);
        d->set_max_drop_amounts("silicon_powder", 4);
        d->add_drop("silicon_powder", 1, 0.900f);
        d->add_drop("silicon_powder", 2, 0.085f);
        d->add_drop("silicon_powder", 3, 0.010f);
        d->add_drop("silicon_powder", 4, 0.005f);
}

void validate_crusher_settings()
{
}

void load_crusher_dat()
{
    register_crusher_settings();
    validate_crusher_settings();
}


/* Public Interface */

void init_crusher_dat()
{
    GS_ASSERT_ABORT(drops == NULL);
    drops = new class Item::ItemDrop[MAX_ITEM_TYPES];
}

void teardown_crusher_dat()
{
    delete[] drops;
}

class Item::ItemDrop* get_crusher_drop(ItemType item_type)
{
    IF_ASSERT(!isValid(item_type)) return NULL;
    if (!drops[item_type].is_loaded()) return NULL;
    return &drops[item_type];
}

}   // ItemContainer
