#pragma once

#if DC_CLIENT

namespace Item
{

//void add_container_item(ItemContainer* container, int slot, int id)
//{
    //bool can_add = container->can_add(type, slot);
    //if (!can_add) return;
    //add_item_to_container_CtoS msg;
    //msg.container_id = container->id;
    //msg.id = id;
    //msg.type = type;
    //msg.slot = slot;
    //msg.send();
//}

//void remove_container_item(ItemContainer* container, int slot)
//{
    //bool can_remove = container->can_remove(slot);
    //if (!can_remove) return;
    //remove_item_from_container_CtoS msg;
    //msg.container_id = container->id;
    //msg.slot = slot;
    //msg.send();
//}

//bool swap_within_container_item(ItemContainer* container, int slota, int slotb)
//{
    //bool swapped = container->can_swap(slota, slotb);
    //if (!swapped) return false;
    //swap_item_in_container_CtoS msg;
    //msg.container_id = container->id;
    //msg.slota = slota;
    //msg.slotb = slotb;
    //msg.send();
    //return swapped;
//}

//bool swap_between_container_item(ItemContainer* containera, ItemContainer* containerb, int slota, int slotb)
//{
    //bool swapped = container->can_swap(slota, slotb);
    //if (!swapped) return false;
    //swap_item_in_container_CtoS msg;
    //msg.container_id = container->id;
    //msg.slota = slota;
    //msg.slotb = slotb;
    //msg.send();
    //return swapped;
//}

//bool merge_container_stack(ItemContainer* container, int slota, int slotb, int count)
//{
    //bool merged = container->can_merge_stack(slota, slotb, count);
    //if (!merged) return false;
    //merge_stack_in_container_CtoS msg;
    //msg.container_id = container->id;
    //msg.slota = slota;
    //msg.slotb = slotb;
    //msg.count = count;
    //msg.send();
    //return merged;
//}


///* Input Event Handler */

//// Event data required:
//// button
//// inventory
//// slot

//// t_hud processes input event,
//// logs that data

//void remove_event(int id, int slot)
//{
    //ItemContainer* container = Item::get_container(id);
    //if (container == NULL) return;


    //// if hand is occupied
        //// swap
    //// else
        //// remove item from inventory
        //// put item in hand
    
    //remove_container_item(container, slot);
//}

//void add_event(int id, int slot)
//{
    //ItemContainer* container = Item::get_container(id);
    //if (container == NULL) return;

    //// if nothing in hand
        //// return
    //// else
        
    
//}

//void swap_within_event(int inventory_id, int slota, int slotb)
//{
    //ItemContainer* container = t_Item::get_container(id);
    //if (container == NULL) return;

    //// decide if a swap or a merge

    //InventorySlot* itema = inv->get_item(slota);
    //InventorySlot* itemb = inv->get_item(slotb);
    //assert(itema != NULL && itemb != NULL);

    //int stack_space = STACK_SIZE_MAX - itemb->count;
    //int stack_avail = itema->count;
    //int count = (stack_avail > stack_space) ? stack_space : stack_avail;

    //if (stack_avail && stack_space > 0                // there is room to merge
        //&& !itema->empty() && !itemb->empty()       // neither are empty
        //&& itema->item_type == itemb->item_type     // items match
    //)   // merge
        //inv->merge_stack_action(slota, slotb, count);
    //else   // swap
        //inv->swap_action(slota, slotb);
//}

//void swap_between_event(int inventory_ida, int slota, int inventory_idb, int slotb)
//{
    //Inventory* inva = Items::get_inventory(inventory_ida);
    //if (inva == NULL) return;
    //Inventory* invb = Items::get_inventory(inventory_idb);
    //if (invb == NULL) return;

    //// decide if a swap or a merge

    //InventorySlot* itema = inva->get_item(slota);
    //InventorySlot* itemb = invb->get_item(slotb);
    //assert(itema != NULL && itemb != NULL);

    //int stack_space = STACK_SIZE_MAX - itemb->count;
    //int stack_avail = itema->count;
    //int count = (stack_avail > stack_space) ? stack_space : stack_avail;
    //if (stack_avail && stack_space > 0                             // there is room to merge
        //&& !itema->empty() && !itemb->empty()       // neither are empty
        //&& itema->item_type == itemb->item_type     // items match
    //) {   // merge
        //if (!invb->can_add_stack(itema->item_type, slotb, count)) return;
        //if (!inva->can_remove_stack(slota, count)) return;

        //merge_stack_between_inventory_CtoS msg;
        //msg.inventorya = inventory_ida;
        //msg.slota = slota;
        //msg.inventoryb = inventory_idb;
        //msg.slotb = slotb;
        //msg.count = count;
        //msg.send();
    //}  
    //else
    //{
        //if (!inva->can_remove(slota)) return;
        //if (!invb->can_add(itema->item_type)) return;

        //swap_item_between_inventory_CtoS msg;
        //msg.inventorya = inventory_ida;
        //msg.slota = slota;
        //msg.inventoryb = inventory_idb;
        //msg.slotb = slotb;
        //msg.send();
    //}
//}

// 3 packets, for actions
// container_action_alpha_CtoS
// container_action_beta_CtoS
// failure_StoC

// 2 packets, for state in shared containers
// container_remove_item_StoC
// container_add_item_StoC

static uint16_t container_event_id = 0;

void send_container_alpha_action()
{
    container_event_id += 1;
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.send();
}

void send_container_beta_action()
{
    container_event_id += 1;
    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.send();
}

bool alpha_action_decision_tree(int id, int slot)
{
    if (slot == NULL_SLOT || id < 0) return false;
    ItemContainer* container = get_container(id);
    if (container == NULL) return false;

    ItemID slot_item = container->get_item(slot);
    bool something_happened = false;

    // if hand empty
        // pick up
    // else
        // if slot empty
            // move to slot
        // else
            // if slot type matches hand type
                // if hand stack < available stack
                    // move hand stack onto slot
                // else
                    // if avail stack == 0
                        // swap
                    // else
                        // move avail stack amt from hand stack
                        
    // hand is empty
    if (player_hand == NULL_ITEM)
    {
        // slot is occupied
        if (slot_item != NULL_ITEM)
        {   // SLOT -> HAND
            // remove slot item
            container->remove_item(slot);
            // put in hand
            player_hand = slot_item;
            something_happened = true;
        }
    }
    // hand holding item
    else
    {
        // slot is empty
        if (slot_item == NULL_ITEM)
        {   // HAND -> SLOT
            // put hand item in slot
            container->insert_item(slot, player_hand);
            // remove item from hand
            player_hand = NULL_ITEM;
            something_happened = true;
        }
        // slot is occupied
        else
        {
            // do stack stuff
        }
    }

    // send packet
    return something_happened;
}

bool beta_action_decision_tree(int id, int slot)
{
    if (slot == NULL_SLOT || id < 0) return false;
    ItemContainer* container = get_container(id);
    if (container == NULL) return false;

    ItemID slot_item = container->get_item(slot);

    bool something_happened = false;

    // if hand empty
    // do nothing [minecraft splits stacks]
    // else
        // if slot empty
            // place 1 unit in slot
            // if 0 units remain
                // remove from hand
        // if hand item stacks with slot item
            // place 1 unit from stack in slot
            // if 0 units remain
                // remove from hand

    if (player_hand == NULL_ITEM)
    {
        // do nothing
        // Minecraft would split a stack here
    }
    else
    {
        // slot is empty
        if (slot_item == NULL_ITEM)
        {
            // place 1 stack unit in slot
        }
    }

    return something_happened;
}

void mouse_left_click_handler(int id, int slot)
{
    bool something_happened = alpha_action_decision_tree(id, slot);
    if (something_happened) send_container_alpha_action();
}

void mouse_right_click_handler(int id, int slot)
{
    bool something_happened = beta_action_decision_tree(id, slot);
    if (something_happened) send_container_beta_action();
}

}

#endif
