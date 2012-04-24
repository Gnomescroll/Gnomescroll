#include "hud.hpp"

#include <c_lib/items/inventory/include.hpp>

static bool point_in_inventory(int x, int y, HudInventory::InventoryRender* inventory)
{
    return (point_in_rect(x,y, inventory->x, inventory->y, inventory->w, inventory->h));
}

// returns closest inventory intersected
// NULL if none match
static Inventory* get_inventory_slot_selection(int x, int y, int* row, int* col)
{
    // track the closest
    Inventory* closest_inventory = NULL;
    float closest = 100000.0f;
    HudInventory::InventoryRender* closest_render = NULL;

    // pack inventories
    const int n_inventories = 4;
    Inventory* inventories[n_inventories] = {
        Items::agent_inventory,
        Items::agent_toolbelt,
        Items::nanite_inventory,
        Items::craft_bench_inventory,
    };

    HudInventory::InventoryRender* render;
    // find the closest
    for (int i=0; i<n_inventories; i++)
    {
        if (inventories[i] == NULL) continue;
        render = HudInventory::get_inventory_hud_element(inventories[i]->hud);
        if (render->visible) continue;
        if (point_in_inventory(x,y, render) && render->z < closest)
        {
            closest_inventory = inventories[i];
            closest_render = render;
            closest = render->z;
        }
    }

    if (closest_inventory == NULL) return NULL;

    // get row,col for slot intersected
    HudInventory::get_screen_inventory_row_col(closest_render, x,y, row, col);
    return closest_inventory;
}

void hud_handle_left_mouse_click(int x, int y)
{
    int row,col;
    Inventory* inventory = get_inventory_slot_selection(x,y, &row, &col);
    inventory->select_slot(row, col);

    t_hud::handle_left_mouse_click(x,y);
}

void hud_handle_right_mouse_click(int x, int y)
{
    int row,col;
    Inventory* inventory = get_inventory_slot_selection(x,y, &row, &col);
    inventory->select_and_remove_action(row, col);

    t_hud::handle_right_mouse_click(x,y);
}

