#include "handlers.hpp"

/* Handlers for inventory events that require cutting across modules */

#if DC_CLIENT

#include <c_lib/t_hud/_interface.hpp>
#include <c_lib/t_hud/event.hpp>

#include <c_lib/inventory/interface.hpp>
#include <c_lib/inventory/inventory.hpp>

namespace Items
{

/* Network events */

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

    // copy metadata to hud inventory element
    t_hud::network_inventory_assignment(inventory->type, inventory->id);
}

static void agent_toolbelt_received(Inventory* inventory)
{
    // get owner, for attaching
    Agent_state* a = STATE::agent_list->get(inventory->owner);
    assert(a != NULL); // this function should not be called if the owner did not exist

    // attach to agent status
    if (a->status.toolbelt != NULL) printf("WARNING: reassigned agent inventory\n");
    a->status.toolbelt = inventory;

    // attach to global pointer
    Items::agent_toolbelt = inventory;

    // copy metadata to hud inventory element
    t_hud::network_inventory_assignment(inventory->type, inventory->id);
}

static void nanite_inventory_received(Inventory* inventory)
{
    // attach to global pointer
    Items::nanite_inventory = inventory;

    // copy metadata to hud inventory element
    t_hud::network_inventory_assignment(inventory->type, inventory->id);
}

static void craft_bench_inventory_received(Inventory* inventory)
{
    // attach to global pointer
    Items::nanite_inventory = inventory;

    // copy metadata to hud inventory element
    t_hud::network_inventory_assignment(inventory->type, inventory->id);
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

/* Input Events */

void remove_event(int inventory_id, int slot)
{
    // get inventory
    Inventory* inv = Items::get_inventory(inventory_id);
    if (inv == NULL) return;

    inv->remove_action(slot);
}

void swap_within_event(int inventory_id, int slota, int slotb)
{
    Inventory* inv = Items::get_inventory(inventory_id);
    if (inv == NULL) return;

    // decide if a swap or a merge

    InventorySlot* itema = inv->get_item(slota);
    InventorySlot* itemb = inv->get_item(slotb);
    assert(itema != NULL && itemb != NULL);

    const int STACK_MAX = 5;
    int stack_space = STACK_MAX - itemb->count;
    if (stack_space > 0                             // there is room to merge
        && !itema->empty() && !itemb->empty()       // neither are empty
        && itema->item_type == itemb->item_type     // items match
    ) {   // merge
        int stack_avail = itema->count;
        int count = (stack_avail > stack_space) ? stack_space : stack_avail;
        inv->merge_stack_action(slota, slotb, count);
    }  
    else   // swap
        inv->swap_action(slota, slotb);
}

void swap_between_event(int inventory_ida, int slota, int inventory_idb, int slotb)
{
    Inventory* inva = Items::get_inventory(inventory_ida);
    if (inva == NULL) return;
    Inventory* invb = Items::get_inventory(inventory_idb);
    if (invb == NULL) return;

    // decide if a swap or a merge

    InventorySlot* itema = inva->get_item(slota);
    InventorySlot* itemb = invb->get_item(slotb);
    assert(itema != NULL && itemb != NULL);

    const int STACK_MAX = 5;
    int stack_space = STACK_MAX - itemb->count;
    if (stack_space > 0                             // there is room to merge
        && !itema->empty() && !itemb->empty()       // neither are empty
        && itema->item_type == itemb->item_type     // items match
    ) {   // merge
        int stack_avail = itema->count;
        int count = (stack_avail > stack_space) ? stack_space : stack_avail;

        if (!invb->can_add_stack(itema->item_type, slotb, count)) return;
        if (!inva->can_remove_stack(slota, count)) return;

        merge_stack_between_inventory_CtoS msg;
        msg.inventorya = inventory_ida;
        msg.slota = slota;
        msg.inventoryb = inventory_idb;
        msg.slotb = slotb;
        msg.count = count;
        msg.send();
    }  
    else
    {
        if (!inva->can_remove(slota)) return;
        if (!invb->can_add(itema->item_type)) return;

        swap_item_between_inventory_CtoS msg;
        msg.inventorya = inventory_ida;
        msg.slota = slota;
        msg.inventoryb = inventory_idb;
        msg.slotb = slotb;
        msg.send();
    }
}

void process_inventory_events()
{
    using t_hud::inventory_input_event;
    
    switch (inventory_input_event.type)
    {
        case t_hud::INVENTORY_INPUT_EVENT_NONE:
            break;

        case t_hud::INVENTORY_INPUT_EVENT_REMOVE:
            remove_event(inventory_input_event.inventory, inventory_input_event.slot);
            break;

        case t_hud::INVENTORY_INPUT_EVENT_SWAP_WITHIN:
            swap_within_event(
                inventory_input_event.inventory, inventory_input_event.slot,
                inventory_input_event.slot_b);
            break;

        case t_hud::INVENTORY_INPUT_EVENT_SWAP_BETWEEN:
            swap_between_event(
                inventory_input_event.inventory, inventory_input_event.slot,
                inventory_input_event.inventory_b, inventory_input_event.slot_b);
            break;

        default:
            printf("unhandled inventory input event type\n");
            assert(false);
            break;
    }
}

} // Items

#endif
