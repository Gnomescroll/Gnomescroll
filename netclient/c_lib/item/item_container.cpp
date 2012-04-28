#include "item_container.hpp"

#include <item/net/StoC.hpp>
#include <item/_interface.hpp>
#include <item/_state.hpp>

namespace Item
{

void init_container(ItemContainer* container, ItemContainerType type)
{
    switch (type)
    {
        case AGENT_INVENTORY:
            container->init(AGENT_INVENTORY, AGENT_INVENTORY_X, AGENT_INVENTORY_Y);
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
bool alpha_action_decision_tree(int client_id, int id, int slot)
#endif
{
    if (slot == NULL_SLOT || id < 0) return false;
    ItemContainer* container = get_container(id);
    if (container == NULL) return false;

    #if DC_CLIENT
    ItemID hand_item = player_hand;
    #endif
    #if DC_SERVER
    assert(agent_hand_list != NULL);
    ItemID hand_item = agent_hand_list[client_id];
    #endif

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
    if (hand_item == NULL_ITEM)
    {
        // slot is occupied
        if (slot_item != NULL_ITEM)
        {   // SLOT -> HAND
            // remove slot item
            container->remove_item(slot);
            // put in hand
            hand_item = slot_item;
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
            container->insert_item(slot, hand_item);
            // remove item from hand
            hand_item = NULL_ITEM;
            something_happened = true;
        }
        // slot is occupied
        else
        {
            // do stack stuff
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
bool beta_action_decision_tree(int client_id, int id, int slot)
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
            // place 1 unit in slot
            // if 0 units remain
                // remove from hand
        // if hand item stacks with slot item
            // place 1 unit from stack in slot
            // if 0 units remain
                // remove from hand

    if (hand_item == NULL_ITEM)
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
