#include "handlers.hpp"

/* Handlers for inventory events that require cutting across modules */

#if DC_CLIENT

#include <c_lib/hud/inventory.hpp>
#include <c_lib/items/inventory/interface.hpp>
#include <c_lib/items/inventory/inventory.hpp>

namespace Items
{

static void agent_inventory_received(Inventory* inventory)
{
    // get owner, for attaching
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    assert(a != NULL); // this function should not be called if the owner did not exist

    // attach to agent status
    if (a->status.inventory != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.inventory = inventory;

    // attach to global pointer
    Items::agent_inventory = inventory;

    // set hud element enum
    HudElementType hud = HUD_ELEMENT_AGENT_INVENTORY;
    inventory->hud = hud;

    // copy metadata to hud inventory element
    HudInventory::InventoryRender* render = HudInventory::get_inventory_hud_element(hud);
    assert(render != NULL);
    render->xdim = inventory->width();
    render->ydim = inventory->height();
}

static void agent_toolbelt_received(Inventory* inventory)
{
    // get owner, for attaching
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    assert(a != NULL); // this function should not be called if the owner did not exist

    // attach to agent status
    if (a->status.toolbelt != NULL)
        printf("WARNING: reassigned agent inventory\n");
    a->status.toolbelt = inventory;

    // attach to global pointer
    Items::agent_toolbelt = inventory;

    // set hud element enum
    HudElementType hud = HUD_ELEMENT_AGENT_TOOLBELT;
    inventory->hud = hud;

    // copy metadata to hud inventory element
    HudInventory::InventoryRender* render = HudInventory::get_inventory_hud_element(hud);
    assert(render != NULL);
    render->xdim = inventory->width();
    render->ydim = inventory->height();
}

static void nanite_inventory_received(Inventory* inventory)
{
    // attach to global pointer
    Items::nanite_inventory = inventory;

    // set hud element enum
    HudElementType hud = HUD_ELEMENT_NANITE_INVENTORY;
    inventory->hud = hud;

    // copy metadata to hud inventory element
    HudInventory::InventoryRender* render = HudInventory::get_inventory_hud_element(hud);
    assert(render != NULL);
    render->xdim = inventory->width();
    render->ydim = inventory->height();
}

static void craft_bench_inventory_received(Inventory* inventory)
{
    // attach to global pointer
    Items::nanite_inventory = inventory;

    // set hud element enum
    HudElementType hud = HUD_ELEMENT_NANITE_INVENTORY;
    inventory->hud = hud;

    // copy metadata to hud inventory element
    HudInventory::InventoryRender* render = HudInventory::get_inventory_hud_element(hud);
    assert(render != NULL);
    render->xdim = inventory->width();
    render->ydim = inventory->height();
}

// trigger on network create inventory
void received_inventory_handler(Inventory* inventory)
{
    // if player does not own this inventory, we dont care
    // TODO: change this, if/when we have shared/global inventories, such as craft blocks
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    if (a == NULL) return;
    if (a != ClientState::playerAgent_state.you) return;

    switch (inventory->type)
    {
        case OBJECT_AGENT_INVENTORY:
            agent_inventory_received(inventory);
            break;
        case OBJECT_AGENT_TOOLBELT:
            agent_toolbelt_received(inventory);
            break;
        case OBJECT_NANITE_INVENTORY:
            nanite_inventory_received(inventory);
            break;
        case OBJECT_CRAFTING_BENCH:
            craft_bench_inventory_received(inventory);
            break;
        default:
            printf("WARNING:: received_inventory_handler() -- unhandled inventory type %d\n", inventory->type);
            break;
    }
}

void inventory_input_event()
{
    // inspect InventoryInputEvent, route based on that

    //const int n_inventories = 4;
    //Inventory* inventories[n_inventories] = {
        //agent_inventory,
        //agent_toolbelt,
        //nanite_inventory,
        //craft_bench_inventory,
    //};

    //Inventory* inventory;
    //HudInventory::InventoryRender* render;
    //for (int i=0; i<n_inventories; i++)
    //{
        //inventory = inventories[i];
        //if (inventory == NULL) continue;
        //render = HudInventory::get_inventory_hud_element(inventory->hud);
        //if (render == NULL) continue;
        //inventory->select_slot(render->active_slot);
    //}

}

} // Items

#endif
