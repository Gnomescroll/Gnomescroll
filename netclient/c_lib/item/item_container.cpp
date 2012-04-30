#include "item_container.hpp"

#include <item/net/StoC.hpp>
#include <item/_interface.hpp>
#include <item/_state.hpp>

#if DC_CLIENT
#include <item/client_item_container.hpp>
#endif

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
ContainerActionType alpha_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType alpha_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    #endif

    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    int hand_item_type = get_item_type(hand_item);
    int hand_item_stack = get_stack_size(hand_item);
    #endif

    // click outside container
    if (id == NULL_CONTAINER)
    {
        if (!hand_empty)
        {   // remove
            #if DC_CLIENT
            hand_item_type = NULL_ITEM_TYPE;
            hand_item_stack = 1;
            #endif
            #if DC_SERVER
            throw_item(agent_id, hand_item);
            hand_item = NULL_ITEM;
            send_hand_remove(client_id);
            #endif
            action = FULL_HAND_TO_WORLD;
        }
    }
    else
    // click inside container
    {
        // client was inside container, but not a slot
        // do nothing
        if (slot < 0 || slot == NULL_SLOT) return action;

        // get container
        #if DC_CLIENT
        ItemContainerUI* container = get_container_ui(id);
        #endif
        #if DC_SERVER
        ItemContainer* container = get_container(id);
        #endif
        assert(container != NULL);

        #if DC_CLIENT
        int slot_item_type = container->get_slot_type(slot);
        bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
        int slot_item_stack = container->get_slot_stack(slot);
        int slot_item_space = get_stack_max(slot_item_type) - slot_item_stack;
        #endif

        #if DC_SERVER
        ItemID slot_item = container->get_item(slot);
        bool slot_empty = (slot_item == NULL_ITEM);
        int slot_item_type = get_item_type(slot_item);
        //int slot_item_stack = get_stack_size(slot_item);
        int slot_item_space = get_stack_space(slot_item);
        #endif

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

        if (hand_empty)
        // hand is empty
        {
            if (!slot_empty)
            // slot is occupied
            {   // SLOT -> HAND
                // remove slot item
                #if DC_CLIENT
                container->remove_item(slot);
                hand_item_type = slot_item_type;
                hand_item_stack = slot_item_stack;
                #endif
                #if DC_SERVER
                container->remove_item(slot);
                send_container_remove(client_id, container->id, slot);
                hand_item = slot_item;
                send_hand_insert(client_id, hand_item);
                #endif
                action = FULL_SLOT_TO_EMPTY_HAND;
            }
        }
        // hand holding item
        else
        {
            if (slot_empty)
            // slot is empty
            {   // HAND -> SLOT
                // put hand item in slot
                #if DC_CLIENT
                container->insert_item(slot, hand_item_type, hand_item_stack);
                hand_item_type = NULL_ITEM_TYPE;
                hand_item_stack = 1;
                #endif
                #if DC_SERVER
                container->insert_item(slot, hand_item);
                send_container_insert(client_id, hand_item, container->id, slot);
                hand_item = NULL_ITEM;
                send_hand_remove(client_id);
                #endif
                action = FULL_HAND_TO_EMPTY_SLOT;
            }
            else
            // slot is occupied
            {
                if (slot_item_type == hand_item_type)
                // types are the same
                {
                    // hand stack will fit entirely in slot
                    if (hand_item_stack <= slot_item_space)
                    {   // FULL STACK MERGE
                        // add stacks
                        #if DC_CLIENT
                        container->insert_item(slot, slot_item_type, slot_item_stack + hand_item_stack);
                        hand_item_type = NULL_ITEM_TYPE;
                        hand_item_stack = 1;
                        #endif
                        #if DC_SERVER
                        merge_item_stack(hand_item, slot_item); // merge_item_stack(src, dest)
                        broadcast_item_state(slot_item);
                        destroy_item(hand_item);
                        hand_item = NULL_ITEM;
                        send_hand_remove(client_id);
                        #endif
                        action = FULL_HAND_TO_OCCUPIED_SLOT;
                    }
                    else
                    // stacks will not completely merge
                    {
                        if (slot_item_space == 0)
                        // the stack is full
                        {  // SWAP
                            #if DC_CLIENT
                            container->insert_item(slot, hand_item_type, hand_item_stack);
                            hand_item_type = slot_item_type;
                            hand_item_stack = slot_item_stack;
                            #endif
                            #if DC_SERVER
                            container->insert_item(slot, hand_item);
                            send_container_insert(client_id, hand_item, container->id, slot);
                            hand_item = slot_item;
                            send_hand_insert(client_id, hand_item);
                            #endif
                            action = FULL_HAND_SWAP_WITH_SLOT;
                        }
                        else
                        // some of the hand stack will fit in the slot
                        {   // PARTIAL STACK MERGE
                            #if DC_CLIENT
                            container->insert_item(slot, slot_item_type, slot_item_stack + slot_item_space);
                            //hand_item_type unchanged
                            hand_item_stack -= slot_item_space;
                            assert(hand_item_stack > 0);
                            #endif
                            #if DC_SERVER
                            merge_item_stack(hand_item, slot_item, slot_item_space);
                            // update items
                            broadcast_item_state(slot_item);
                            broadcast_item_state(hand_item);
                            // hand item unchanged
                            #endif
                            action = PARTIAL_HAND_TO_OCCUPIED_SLOT;
                        }
                    }
                }
                else
                // types are different
                {   // SWAP
                    #if DC_CLIENT
                    container->insert_item(slot, hand_item_type, hand_item_stack);
                    hand_item_type = slot_item_type;
                    hand_item_stack = slot_item_stack;
                    #endif
                    #if DC_SERVER
                    container->insert_item(slot, hand_item);
                    send_container_insert(client_id, hand_item, container->id, slot);
                    hand_item = slot_item;
                    send_hand_insert(client_id, hand_item);
                    #endif
                    action = FULL_HAND_SWAP_WITH_SLOT;
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    #endif
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType beta_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType beta_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    if (slot == NULL_SLOT || id < 0) return action;
    #if DC_CLIENT
    ItemContainerUI* container = get_container_ui(id);
    #endif
    #if DC_SERVER
    ItemContainer* container = get_container(id);
    #endif
    if (container == NULL) return action;

    #if DC_CLIENT
    int slot_item_type = container->get_slot_type(slot);
    bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
    int slot_item_stack = container->get_slot_stack(slot);
    int slot_item_space = get_stack_max(slot_item_type) - slot_item_stack;

    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    #endif

    #if DC_SERVER
    ItemID slot_item = container->get_item(slot);
    bool slot_empty = (slot_item == NULL_ITEM);
    int slot_item_type = get_item_type(slot_item);
    int slot_item_stack = get_stack_size(slot_item);
    int slot_item_space = get_stack_space(slot_item);

    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    int hand_item_type = get_item_type(hand_item);
    int hand_item_stack = get_stack_size(hand_item);
    #endif

    // if hand empty
        // if slot not empty
            // if slot stack > 1 (can split)
                // split stack, rounded up
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

    if (hand_empty)
    // hand is empty
    {
        if (!slot_empty)
        // slot is occupied
        {
            if  (slot_item_stack > 1)
            // stack can split
            {
                // split stack, rounded down
                #if DC_CLIENT
                hand_item_type = slot_item_type;
                hand_item_stack = slot_item_stack / 2;
                container->insert_item(slot, slot_item_type, slot_item_stack - hand_item_stack);
                // slot item type unchanged
                #endif
                #if DC_SERVER
                ItemID new_item = split_item_stack_in_half(slot_item);
                hand_item = new_item;
                // slot id is unchanged
                broadcast_item_state(slot_item);
                broadcast_item_create(new_item);
                send_hand_insert(client_id, new_item);
                #endif
                action = PARTIAL_SLOT_TO_EMPTY_HAND;
            }
        }
    }
    else
    // hand is holding something
    {
        assert(hand_item_stack >= 1);
        if (slot_empty)
        // slot is empty
        {
            // place 1 stack unit in slot
            if (hand_item_stack == 1)
            // only 1 in stack, do simple insert
            {
                #if DC_CLIENT
                container->insert_item(slot, hand_item_type, hand_item_stack);
                hand_item_type = NULL_ITEM_TYPE;
                hand_item_stack = 1;
                #endif
                #if DC_SERVER
                container->insert_item(slot, hand_item);
                send_container_insert(client_id, hand_item, container->id, slot);
                hand_item = NULL_ITEM;
                send_hand_remove(client_id);
                #endif
                action = FULL_HAND_TO_EMPTY_SLOT;
            }
            else
            // must split stack
            {
                #if DC_CLIENT
                container->insert_item(slot, hand_item_type, 1);
                // hand item type unchanged
                hand_item_stack -= 1;
                assert(hand_item_stack > 0);
                #endif
                #if DC_SERVER
                ItemID new_item = split_item_stack(hand_item, 1);   // WARNING: CREATES ITEM
                container->insert_item(slot, new_item);
                broadcast_item_state(hand_item);
                broadcast_item_create(new_item);
                send_container_insert(client_id, new_item, container->id, slot);
                // hand item id is unchanged
                #endif
                action = PARTIAL_HAND_TO_EMPTY_SLOT;
            }
        }
        else
        // slot is occupied
        {
            if (slot_item_type == hand_item_type)
            // types are the same
            {
                if (slot_item_space > 0)
                // item can stack 1
                {
                    if (hand_item_stack == 1)
                    // hand only had one
                    {
                        #if DC_CLIENT
                        container->insert_item(slot, slot_item_type, slot_item_stack + 1);
                        hand_item_type = NULL_ITEM_TYPE;
                        hand_item_stack = 1;
                        #endif
                        #if DC_SERVER
                        merge_item_stack(hand_item, slot_item);
                        // update dest
                        broadcast_item_state(slot_item);
                        // destroy src
                        destroy_item(hand_item);
                        hand_item = NULL_ITEM;
                        send_hand_remove(client_id);
                        #endif
                        action = FULL_HAND_TO_OCCUPIED_SLOT;
                    }
                    else
                    // hand has >1 stack
                    {
                        #if DC_CLIENT
                        container->insert_item(slot, slot_item_type, slot_item_stack + 1);
                        // hand item type unchanged
                        hand_item_stack -= 1;
                        assert(hand_item_stack > 0);
                        #endif
                        #if DC_SERVER
                        merge_item_stack(hand_item, slot_item, 1);
                        // update items
                        broadcast_item_state(slot_item);
                        broadcast_item_state(hand_item);
                        #endif
                        // hand item unchanged
                        action = PARTIAL_HAND_TO_OCCUPIED_SLOT;
                    }
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    #endif
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    return action;
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
