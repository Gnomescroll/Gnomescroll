#include "item_container.hpp"

#include <item/net/StoC.hpp>
#include <item/_interface.hpp>
#include <item/_state.hpp>

#if DC_SERVER
#include <item/server.hpp>
#endif

namespace Item
{

void init_container(ItemContainer* container, ItemContainerType type)
{
    switch (type)
    {
        case AGENT_CONTAINER:
            container->init(AGENT_CONTAINER, AGENT_CONTAINER_X, AGENT_CONTAINER_Y);
            break;
        case AGENT_TOOLBELT:
            container->init(AGENT_TOOLBELT, AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case AGENT_NANITE:
            container->init(AGENT_NANITE, AGENT_NANITE_X, AGENT_NANITE_Y);
            break;
        default:
            printf("init_container() - Unhandled container type %d\n", type);
            assert(false);
            break;
    }
}

/* Transaction Logic */

#if DC_CLIENT
bool alpha_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
bool alpha_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    bool something_happened = false;

    #if DC_CLIENT
    ItemID hand_item = player_hand;
    #endif
    #if DC_SERVER
    assert(agent_hand_list != NULL);
    ItemID hand_item = agent_hand_list[client_id];
    #endif

    // click outside container
    if (id == NULL_CONTAINER)
    {
        if (hand_item != NULL_ITEM)
        {   // remove
            #if DC_SERVER
            throw_item(agent_id, hand_item);
            #endif
            hand_item = NULL_ITEM;
            something_happened = true;
        }
    }
    else
    // click inside container
    {
        // client was inside container, but not a slot
        // do nothing
        if (slot < 0 || slot == NULL_SLOT) return false;

        // get container
        ItemContainer* container = get_container(id);
        assert(container != NULL);

        ItemID slot_item = container->get_item(slot);

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

        if (hand_item == NULL_ITEM)
        // hand is empty
        {
            if (slot_item != NULL_ITEM)
            // slot is occupied
            {   // SLOT -> HAND
                // remove slot item
                container->remove_item(slot);
                #if DC_SERVER
                send_container_remove(client_id, container->id, slot);
                #endif
                // put in hand
                hand_item = slot_item;
                something_happened = true;
            }
        }
        // hand holding item
        else
        {
            if (slot_item == NULL_ITEM)
            // slot is empty
            {   // HAND -> SLOT
                // put hand item in slot
                container->insert_item(slot, hand_item);
                #if DC_SERVER
                send_container_insert(client_id, hand_item, container->id, slot);
                #endif
                // remove item from hand
                hand_item = NULL_ITEM;
                something_happened = true;
            }
            else
            // slot is occupied
            {
                if (get_item_type(slot_item) == get_item_type(hand_item))
                // types are the same
                {
                    int hand_stack_size = get_stack_size(hand_item);
                    int slot_stack_space = get_stack_space(slot_item);
                    // hand stack will fit entirely in slot
                    if (hand_stack_size <= slot_stack_space)
                    {   // FULL STACK MERGE
                        // add stacks
                        merge_item_stack(hand_item, slot_item); // merge_item_stack(src, dest)
                        #if DC_SERVER
                        // update dest
                        broadcast_item_state(slot_item);
                        // destroy src
                        destroy_item(hand_item);
                        #endif
                        hand_item = NULL_ITEM;
                        something_happened = true;
                    }
                    else
                    // stacks will not completely merge
                    {
                        if (slot_stack_space == 0)
                        // the stack is full
                        {  // SWAP
                            container->insert_item(slot, hand_item);
                            #if DC_SERVER
                            send_container_insert(client_id, hand_item, container->id, slot);
                            #endif
                            hand_item = slot_item;
                            something_happened = true;
                        }
                        else
                        // some of the hand stack will fit in the slot
                        {   // PARTIAL STACK MERGE
                            merge_item_stack(hand_item, slot_item, slot_stack_space);
                            #if DC_SERVER
                            // update items
                            broadcast_item_state(slot_item);
                            broadcast_item_state(hand_item);
                            #endif
                            // hand item unchanged
                            something_happened = true;
                        }
                    }
                }
                else
                // types are different
                {   // SWAP
                    container->insert_item(slot, hand_item);
                    #if DC_SERVER
                    send_container_insert(client_id, hand_item, container->id, slot);
                    #endif
                    hand_item = slot_item;
                    something_happened = true;
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand = hand_item;
    #endif
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    // send packet
    return something_happened;
}

#if DC_CLIENT
bool beta_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
bool beta_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    if (slot == NULL_SLOT || id < 0) return false;
    ItemContainer* container = get_container(id);
    if (container == NULL) return false;

    #if DC_CLIENT
    ItemID hand_item = player_hand;
    #endif
    #if DC_SERVER
    ItemID hand_item = agent_hand_list[client_id];
    #endif

    ItemID slot_item = container->get_item(slot);

    bool something_happened = false;

    // if hand empty
    // do nothing [minecraft splits stacks]
    // else
        // if slot empty
            // if stack is 1
                // place item in slot
            // else
                // place 1 unit in slot
        // if hand item stacks with slot item
            // place 1 unit from stack in slot
            // if 0 units remain
                // remove from hand

    if (hand_item == NULL_ITEM)
    // hand is empty
    {
        // do nothing
        // Minecraft would split a stack here

        // CREATES NEW ITEM
    }
    else
    // hand is holding something
    {
        int hand_stack_size = get_stack_size(hand_item);
        if (slot_item == NULL_ITEM)
        // slot is empty
        {
            // place 1 stack unit in slot
            if (hand_stack_size == 1)
            // only 1 in stack, do simple insert
            {
                container->insert_item(slot, hand_item);
                #if DC_SERVER
                send_container_insert(client_id, hand_item, container->id, slot);
                #endif
                hand_item = NULL_ITEM;
                something_happened = true;
            }
            else
            // must split stack
            {
                #if DC_SERVER
                ItemID new_item = split_item_stack(hand_item, 1);   // THIS POSES PROBLEM FOR CLIENT PREDICTION --
                container->insert_item(slot, new_item);             // CREATES NEW ITEM
                broadcast_item_state(hand_item);
                broadcast_item_create(new_item);
                send_container_insert(client_id, new_item, container->id, slot);
                #endif
                // hand item is unchanged
                something_happened = true;
            }
        }
        else
        // slot is occupied
        {
            if (get_item_type(slot_item) == get_item_type(hand_item))
            // types are the same
            {
                int slot_stack_space = get_stack_space(slot_item);
                if (slot_stack_space > 0)
                // item can stack 1
                {
                    something_happened = true;
                    if (hand_stack_size == 1)
                    // hand only had one
                    {
                        #if DC_SERVER
                        merge_item_stack(hand_item, slot_item);
                        // update dest
                        broadcast_item_state(slot_item);
                        // destroy src
                        destroy_item(hand_item);
                        #endif
                        hand_item = NULL_ITEM;
                        something_happened = true;
                    }
                    else
                    // hand has >1 stack
                    {
                        #if DC_SERVER
                        merge_item_stack(hand_item, slot_item, hand_stack_size);
                        // update items
                        broadcast_item_state(slot_item);
                        broadcast_item_state(hand_item);
                        #endif
                        // hand item unchanged
                        something_happened = true;
                    }
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand = hand_item;
    #endif
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    return something_happened;
}

/* Network */
#if DC_SERVER

#include <item/net/StoC.hpp>

//  tell client to assign container to an agent
void send_container_assign(class ItemContainer* container, int client_id)
{
    class assign_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

void send_container_create(class ItemContainer* container, int client_id)
{
    class create_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

void send_container_delete(class ItemContainer* container, int client_id)
{
    class delete_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

#endif



}
