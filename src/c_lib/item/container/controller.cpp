#include "container.hpp"

#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>

#if DC_CLIENT
#include <item/container/container_ui.hpp>
#endif

#if DC_SERVER
#include <item/container/server.hpp>
#include <item/container/net/StoC.hpp>
#endif

namespace ItemContainer
{

/* Transaction Logic */

// transactions

#if DC_CLIENT

ContainerActionType full_hand_to_world(
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability
)
{
    GS_ASSERT(*hand_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(*hand_item_stack > 0);

    *hand_item_type = NULL_ITEM_TYPE;
    *hand_item_stack = 1;
    *hand_item_durability = NULL_DURABILITY;
    return FULL_HAND_TO_WORLD;
}

ContainerActionType partial_hand_to_world(
    int* hand_item_stack, int transfer_stack_size
)
{
    GS_ASSERT(*hand_item_stack > 1);
    *hand_item_stack -= transfer_stack_size;
    return PARTIAL_HAND_TO_WORLD;
}

ContainerActionType full_hand_to_empty_slot(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability
)
{   // put hand item in slot
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(*hand_item_stack > 0);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->insert_item(slot, *hand_item_type, *hand_item_stack, *hand_item_durability);
    *hand_item_type = NULL_ITEM_TYPE;
    *hand_item_stack = 1;
    *hand_item_durability = NULL_DURABILITY;
    return FULL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType full_hand_to_occupied_slot(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability,
    int slot_item_type, int slot_item_stack, int slot_item_durability
)
{ // add stacks
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(*hand_item_stack > 0);
    GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_stack > 0);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->insert_item(slot, slot_item_type, slot_item_stack + *hand_item_stack, slot_item_durability);
    *hand_item_type = NULL_ITEM_TYPE;
    *hand_item_stack = 1;
    *hand_item_durability = NULL_DURABILITY;

    return FULL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_hand_to_empty_slot(
    ItemContainerUIInterface* container, int slot,
    int hand_item_type, int* hand_item_stack, int hand_item_durability
)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(hand_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(*hand_item_stack > 1);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->insert_item(slot, hand_item_type, 1, hand_item_durability);
    // hand item type unchanged
    *hand_item_stack -= 1;
    GS_ASSERT(*hand_item_stack > 0);
    return PARTIAL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType partial_hand_to_occupied_slot(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_stack,
    int slot_item_type, int slot_item_stack, int slot_item_space, int slot_item_durability
)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_stack > 1);
    GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_stack > 0);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->insert_item(slot, slot_item_type, slot_item_stack + slot_item_space, slot_item_durability);
    //hand_item_type unchanged
    *hand_item_stack -= slot_item_space;
    GS_ASSERT(*hand_item_stack > 0);
    return PARTIAL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_slot_to_empty_hand(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability,
    int slot_item_type, int slot_item_stack, int slot_item_durability
)
{   // split stack, rounded down
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_type == NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_stack > 1);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    *hand_item_type = slot_item_type;
    *hand_item_stack = slot_item_stack / 2;
    *hand_item_durability = slot_item_durability;
    container->insert_item(slot, slot_item_type, slot_item_stack - *hand_item_stack, slot_item_durability);
    // slot item type unchanged
    return PARTIAL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_slot_to_empty_hand(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability,
    int slot_item_type, int slot_item_stack, int slot_item_durability
)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_type == NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_stack > 0);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->remove_item(slot);
    *hand_item_type = slot_item_type;
    *hand_item_stack = slot_item_stack;
    *hand_item_durability = slot_item_durability;

    return FULL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_hand_swap_with_slot(
    ItemContainerUIInterface* container, int slot,
    int* hand_item_type, int* hand_item_stack, int* hand_item_durability,
    int slot_item_type, int slot_item_stack, int slot_item_durability
)    
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(*hand_item_stack > 0);
    GS_ASSERT(slot_item_type != NULL_ITEM_TYPE);
    GS_ASSERT(slot_item_stack > 0);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    container->insert_item(slot, *hand_item_type, *hand_item_stack, *hand_item_durability);
    *hand_item_type = slot_item_type;
    *hand_item_stack = slot_item_stack;
    *hand_item_durability = slot_item_durability;

    return FULL_HAND_SWAP_WITH_SLOT;
}

#endif

#if DC_SERVER

ContainerActionType full_hand_to_world(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    // throw item
    transfer_hand_to_particle(agent_id);

    return FULL_HAND_TO_WORLD;
}

ContainerActionType partial_hand_to_world(int agent_id, int transfer_stack_size)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item != NULL_ITEM);

    // split 1 unit of hand item
    ItemID new_item = Item::split_item_stack(hand_item, 1);   // WARNING: CREATES ITEM

    // update items
    Item::send_item_state(hand_item);
    //Item::broadcast_item_create(new_item); 

    // throw item
    ItemParticle::throw_agent_item(agent_id, new_item);

    return PARTIAL_HAND_TO_WORLD;
}

ContainerActionType full_hand_to_empty_slot(int agent_id, ItemContainerInterface* container, int slot)
{   // put hand item in slot
    GS_ASSERT(container != NULL);
    if (container == NULL) return CONTAINER_ACTION_NONE;

    ASSERT_VALID_AGENT_ID(agent_id);
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item != NULL_ITEM);

    transfer_item_from_hand_to_container(hand_item, container->id, slot, agent_id);

    return FULL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType full_hand_to_occupied_slot(int agent_id, int slot, ItemID slot_item)
{ // add stacks
    GS_ASSERT(slot_item != NULL_ITEM);

    ASSERT_VALID_AGENT_ID(agent_id);
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item != NULL_ITEM);
    GS_ASSERT(hand_item != slot_item);

    // merge the hand with the slot
    Item::merge_item_stack(hand_item, slot_item); // Item::merge_item_stack(src, dest)
    // update the slot
    Item::send_item_state(slot_item);

    Item::destroy_item(hand_item);

    return FULL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_hand_to_empty_slot(ItemContainerInterface* container, int slot, ItemID hand_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(hand_item != NULL_ITEM);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    // split 1 unit of hand item
    ItemID new_item = Item::split_item_stack(hand_item, 1);   // WARNING: CREATES ITEM
    // put in slot

    transfer_free_item_to_container(new_item, container->id, slot);
    
    // update items
    Item::send_item_state(hand_item);

    // hand item id is unchanged
    return PARTIAL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType partial_hand_to_occupied_slot(int slot, ItemID hand_item, ItemID slot_item, int slot_item_space)
{
    GS_ASSERT(hand_item != slot_item);
    GS_ASSERT(hand_item != NULL_ITEM);
    GS_ASSERT(slot_item != NULL_ITEM);
    GS_ASSERT(slot_item_space > 0);

    Item::merge_item_stack(hand_item, slot_item, slot_item_space);
    // update items
    Item::send_item_state(slot_item);
    Item::send_item_state(hand_item);
    // hand item unchanged
    return PARTIAL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_slot_to_empty_hand(int agent_id, int slot, ItemID slot_item)
{   // split stack, rounded down
    GS_ASSERT(slot_item != NULL_ITEM);

    // split slot item
    ItemID new_item = Item::split_item_stack_in_half(slot_item);
    // put in hand
    transfer_free_item_to_hand(new_item, agent_id);

    // send new state
    Item::send_item_state(slot_item);

    return PARTIAL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_slot_to_empty_hand(int agent_id, ItemContainerInterface* container, int slot, ItemID slot_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(slot_item != NULL_ITEM);
    GS_ASSERT(slot != NULL_SLOT);
    
    if (container == NULL) return CONTAINER_ACTION_NONE;
    if (slot == NULL_SLOT) return CONTAINER_ACTION_NONE;

    transfer_item_from_container_to_hand(slot_item, container->id, slot, agent_id);
    
    return FULL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_hand_swap_with_slot(int client_id, int agent_id, ItemContainerInterface* container, int slot, ItemID slot_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(slot_item != NULL_ITEM);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    ASSERT_VALID_AGENT_ID(agent_id);
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item != NULL_ITEM);
    GS_ASSERT(hand_item != slot_item);

    // remove item from slot (we need to do this to trigger item's reference to container_id and slot
    container->remove_item(slot);
    send_container_remove(client_id, container->id, slot);

    // remove item from hand
    remove_item_from_hand(agent_id);
    send_hand_remove(client_id);
    
    transfer_free_item_to_container(hand_item, container->id, slot);
    transfer_free_item_to_hand(slot_item, agent_id);

    return FULL_HAND_SWAP_WITH_SLOT;
}

#endif

#if DC_CLIENT
ContainerActionType alpha_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType alpha_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    #endif

    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    int hand_item_type = Item::get_item_type(hand_item);
    int hand_item_stack = Item::get_stack_size(hand_item);
    #endif

    // client was inside container, but not a slot
    // do nothing
    if (slot < 0 || slot == NULL_SLOT) return action;

    // get container
    GS_ASSERT(container_id != NULL_CONTAINER);
    #if DC_CLIENT
    ItemContainerUIInterface* container = get_container_ui(container_id);
    #endif
    #if DC_SERVER
    ItemContainerInterface* container = get_container(container_id);
    #endif
    GS_ASSERT(container != NULL);
    if (container == NULL) return action;

    #if DC_CLIENT
    int slot_item_type = container->get_slot_type(slot);
    bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
    int slot_item_stack = container->get_slot_stack(slot);
    int slot_item_space = Item::get_max_stack_size(slot_item_type) - slot_item_stack;
    int slot_item_durability = container->get_slot_durability(slot);
    #endif

    #if DC_SERVER
    ItemID slot_item = container->get_item(slot);
    bool slot_empty = (slot_item == NULL_ITEM);
    int slot_item_type = Item::get_item_type(slot_item);
    //int slot_item_stack = Item::get_stack_size(slot_item);
    int slot_item_space = Item::get_stack_space(slot_item);
    #endif

    // NORMAL
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

    #if DC_CLIENT
    bool can_insert = container->can_insert_item(slot, hand_item_type);
    #endif
    #if DC_SERVER
    bool can_insert = container->can_insert_item(slot, hand_item);
    #endif

    if (!hand_empty && !can_insert) return action;

    if (hand_empty)
    // hand is empty
    {
        if (!slot_empty)
        // slot is occupied
        {   // SLOT -> HAND
            // remove slot item
            #if DC_CLIENT
            action = full_slot_to_empty_hand(
                container, slot,
                &hand_item_type, &hand_item_stack, &hand_item_durability,
                slot_item_type, slot_item_stack, slot_item_durability
           );
            #endif
            #if DC_SERVER
            action = full_slot_to_empty_hand(agent_id, container, slot, slot_item);
            #endif
        }
    }
    // hand holding item
    else
    {
        if (slot_empty)
        // slot is empty
        {   // HAND -> SLOT
            #if DC_CLIENT
            action = full_hand_to_empty_slot(
                container, slot,
                &hand_item_type, &hand_item_stack, &hand_item_durability
           );
            #endif
            #if DC_SERVER
            action = full_hand_to_empty_slot(agent_id, container, slot);
            #endif
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
                    #if DC_CLIENT
                    action = full_hand_to_occupied_slot(
                        container, slot,
                        &hand_item_type, &hand_item_stack, &hand_item_durability,
                        slot_item_type, slot_item_stack, slot_item_durability
                   );
                    #endif
                    #if DC_SERVER
                    action = full_hand_to_occupied_slot(agent_id, slot, slot_item);
                    #endif
                }
                else
                // stacks will not completely merge
                {
                    if (slot_item_space == 0)
                    // the stack is full
                    {  // SWAP
                        #if DC_CLIENT
                        action = full_hand_swap_with_slot(
                            container, slot,
                            &hand_item_type, &hand_item_stack, &hand_item_durability,
                            slot_item_type, slot_item_stack, slot_item_durability
                       );
                        #endif
                        #if DC_SERVER
                        action = full_hand_swap_with_slot(client_id, agent_id, container, slot, slot_item);
                        #endif
                    }
                    else
                    // some of the hand stack will fit in the slot
                    {   // PARTIAL STACK MERGE
                        #if DC_CLIENT
                        action = partial_hand_to_occupied_slot(
                            container, slot,
                            &hand_item_stack,
                            slot_item_type, slot_item_stack, slot_item_space, slot_item_durability
                       );
                        #endif
                        #if DC_SERVER
                        action = partial_hand_to_occupied_slot(slot, hand_item, slot_item, slot_item_space);
                        #endif
                    }
                }
            }
            else
            // types are different
            {   // SWAP
                #if DC_CLIENT
                action = full_hand_swap_with_slot(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability,
                    slot_item_type, slot_item_stack, slot_item_durability
               );
                #endif
                #if DC_SERVER
                action = full_hand_swap_with_slot(client_id, agent_id, container, slot, slot_item);
                #endif
            }
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType synthesizer_alpha_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType synthesizer_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    if (slot == NULL_SLOT || id == NULL_CONTAINER) return action;

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    #endif

    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    //int hand_item_type = Item::get_item_type(hand_item);
    int hand_item_stack = Item::get_stack_size(hand_item);
    #endif

    // get container
    #if DC_CLIENT
    ItemContainerUIInterface* container = get_container_ui(id);
    #endif
    #if DC_SERVER
    ItemContainerInterface* container = get_container(id);
    #endif
    GS_ASSERT(container != NULL);
    if (container == NULL) return action;
    GS_ASSERT(container->type = AGENT_SYNTHESIZER);

    // client was inside container, but not a slot
    // do nothing
    GS_ASSERT(container->is_valid_slot(slot));
    if (!container->is_valid_slot(slot)) return action;
    
    #if DC_CLIENT
    int slot_item_type = container->get_slot_type(slot);
    bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
    int slot_item_stack = container->get_slot_stack(slot);
    int slot_item_space = Item::get_max_stack_size(slot_item_type) - slot_item_stack;
    int slot_item_durability = container->get_slot_durability(slot);
    #endif

    #if DC_SERVER
    ItemID slot_item = container->get_item(slot);
    bool slot_empty = (slot_item == NULL_ITEM);
    //int slot_item_type = Item::get_item_type(slot_item);
    //int slot_item_stack = Item::get_stack_size(slot_item);
    int slot_item_space = Item::get_stack_space(slot_item);
    #endif

    #if DC_CLIENT
    bool can_insert = container->can_insert_item(slot, hand_item_type);
    #endif
    #if DC_SERVER
    bool can_insert = container->can_insert_item(slot, hand_item);
    #endif

	if (hand_empty)
	{
		if (!slot_empty)
		{   // pick up coin
			#if DC_CLIENT
			action = full_slot_to_empty_hand(
				container, slot,
				&hand_item_type, &hand_item_stack, &hand_item_durability,
				slot_item_type, slot_item_stack, slot_item_durability
		    );
			#endif
			#if DC_SERVER
			action = full_slot_to_empty_hand(agent_id, container, slot, slot_item);
			#endif
		}
	}
	else
	{
		if (slot_empty)
		{   // place coin
			if (can_insert)
			{
				#if DC_CLIENT
				action = full_hand_to_empty_slot(
					container, slot,
					&hand_item_type, &hand_item_stack, &hand_item_durability
			    );
				#endif
				#if DC_SERVER
				action = full_hand_to_empty_slot(agent_id, container, slot);
				#endif
			}
		}
		else
		{   // see if we can merge some
			if (can_insert)
			{
				// hand stack will fit entirely in slot
				if (hand_item_stack <= slot_item_space)
				{   // FULL STACK MERGE
					#if DC_CLIENT
					action = full_hand_to_occupied_slot(
						container, slot,
						&hand_item_type, &hand_item_stack, &hand_item_durability,
						slot_item_type, slot_item_stack, slot_item_durability
				    );
					#endif
					#if DC_SERVER
					action = full_hand_to_occupied_slot(agent_id, slot, slot_item);
					#endif
				}
				else
				// stacks will not completely merge
				{
					if (slot_item_space == 0)
					// the stack is full
					{
                        #if DC_CLIENT
                        action = full_hand_swap_with_slot(
                            container, slot,
                            &hand_item_type, &hand_item_stack, &hand_item_durability,
                            slot_item_type, slot_item_stack, slot_item_durability
                        );
                        #endif
                        #if DC_SERVER
                        action = full_hand_swap_with_slot(client_id, agent_id, container, slot, slot_item);
                        #endif
					}
					else
					// some of the hand stack will fit in the slot
					{   // PARTIAL STACK MERGE
						#if DC_CLIENT
						action = partial_hand_to_occupied_slot(
							container, slot,
							&hand_item_stack,
							slot_item_type, slot_item_stack, slot_item_space, slot_item_durability
					    );
						#endif
						#if DC_SERVER
						action = partial_hand_to_occupied_slot(slot, hand_item, slot_item, slot_item_space);
						#endif
					}
				}                    
			}
		}
	}

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
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

    if (slot == NULL_SLOT || id == NULL_CONTAINER) return action;
    #if DC_CLIENT
    ItemContainerUIInterface* container = get_container_ui(id);
    #endif
    #if DC_SERVER
    ItemContainerInterface* container = get_container(id);
    #endif
    if (container == NULL) return action;

    #if DC_CLIENT
    int slot_item_type = container->get_slot_type(slot);
    bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
    int slot_item_stack = container->get_slot_stack(slot);
    int slot_item_space = Item::get_max_stack_size(slot_item_type) - slot_item_stack;
    int slot_item_durability = container->get_slot_durability(slot);

    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    #endif

    #if DC_SERVER
    ItemID slot_item = container->get_item(slot);
    bool slot_empty = (slot_item == NULL_ITEM);
    int slot_item_type = Item::get_item_type(slot_item);
    int slot_item_stack = Item::get_stack_size(slot_item);
    int slot_item_space = Item::get_stack_space(slot_item);

    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    int hand_item_type = Item::get_item_type(hand_item);
    int hand_item_stack = Item::get_stack_size(hand_item);
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
            if (slot_item_stack > 1)
            // stack can split
            {
                #if DC_CLIENT
                action = partial_slot_to_empty_hand(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability,
                    slot_item_type, slot_item_stack, slot_item_durability
               );
                #endif
                #if DC_SERVER
                action = partial_slot_to_empty_hand(agent_id, slot, slot_item);
                #endif
            }
        }
    }
    else
    // hand is holding something
    {
        GS_ASSERT(hand_item_stack >= 1);

        if (slot_empty)
        // slot is empty
        {
            // place 1 stack unit in slot
            if (hand_item_stack == 1)
            // only 1 in stack, do simple insert
            {
                #if DC_CLIENT
                action = full_hand_to_empty_slot(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability
               );
                #endif
                #if DC_SERVER
                action = full_hand_to_empty_slot(agent_id, container, slot);
                #endif
            }
            else
            // must split stack
            {
                #if DC_CLIENT
                action = partial_hand_to_empty_slot(
                    container, slot,
                    hand_item_type, &hand_item_stack, hand_item_durability
               );
                #endif
                #if DC_SERVER
                action = partial_hand_to_empty_slot(container, slot, hand_item);
                #endif
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
                        action = full_hand_to_occupied_slot(
                            container, slot,
                            &hand_item_type, &hand_item_stack, &hand_item_durability,
                            slot_item_type, slot_item_stack, slot_item_durability
                       );
                        #endif
                        #if DC_SERVER
                        action = full_hand_to_occupied_slot(agent_id, slot, slot_item);
                        #endif
                    }
                    else
                    // hand has >1 stack
                    {
                        #if DC_CLIENT
                        action = partial_hand_to_occupied_slot(
                            container, slot,
                            &hand_item_stack,
                            slot_item_type, slot_item_stack, 1, slot_item_durability
                       );
                        #endif
                        #if DC_SERVER
                        action = partial_hand_to_occupied_slot(slot, hand_item, slot_item, 1);
                        #endif
                    }
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType synthesizer_beta_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType synthesizer_beta_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    if (slot == NULL_SLOT || id == NULL_CONTAINER) return action;
    #if DC_CLIENT
    ItemContainerUIInterface* container = get_container_ui(id);
    #endif
    #if DC_SERVER
    ItemContainerInterface* container = get_container(id);
    #endif
    if (container == NULL) return action;
    GS_ASSERT(container->type = AGENT_SYNTHESIZER);
    GS_ASSERT(container->is_valid_slot(slot));
    if (!container->is_valid_slot(slot)) return action;

    #if DC_CLIENT
    int slot_item_type = container->get_slot_type(slot);
    bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
    int slot_item_stack = container->get_slot_stack(slot);
    int slot_item_space = Item::get_max_stack_size(slot_item_type) - slot_item_stack;
    int slot_item_durability = container->get_slot_durability(slot);

    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    #endif

    #if DC_SERVER
    ItemID slot_item = container->get_item(slot);
    bool slot_empty = (slot_item == NULL_ITEM);
    int slot_item_type = Item::get_item_type(slot_item);
    int slot_item_stack = Item::get_stack_size(slot_item);
    int slot_item_space = Item::get_stack_space(slot_item);

    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    int hand_item_type = Item::get_item_type(hand_item);
    int hand_item_stack = Item::get_stack_size(hand_item);
    #endif

    GS_ASSERT(hand_item_stack >= 1);

    // if hand empty
        // if food slot
            // if slot stack > 1
                // split stack rounded up
        // else if coin slot
            // if slot stack > 1
                // split stack, rounded up
        // else (is shopping)
            // do nothing
    // else (hand occupied)
        // if food slot
            // if types match
                // if food slot stack not maxed
                    // remove 1 from hand
                    // add 1 to stack
        // else if coin slot
            // same, except coins only
        // else (is shopping)
            // do nothing

    if (hand_empty)
    // hand is empty
    {
        if (!slot_empty)
        {
            if (slot_item_stack > 1)
            // stack can split
            {
                #if DC_CLIENT
                action = partial_slot_to_empty_hand(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability,
                    slot_item_type, slot_item_stack, slot_item_durability
               );
                #endif
                #if DC_SERVER
                action = partial_slot_to_empty_hand(agent_id, slot, slot_item);
                #endif
            }
        }
    }
    else
    // hand is holding something
    {
        GS_ASSERT(hand_item_stack >= 1);

        if (slot_empty)
        {
            #if DC_CLIENT
            bool can_insert = container->can_insert_item(slot, hand_item_type);
            #endif
            #if DC_SERVER
            bool can_insert = container->can_insert_item(slot, hand_item);
            #endif
            if (can_insert)
            {
                // place 1 stack unit in slot
                if (hand_item_stack == 1)
                // only 1 in stack, do simple insert
                {
                    #if DC_CLIENT
                    action = full_hand_to_empty_slot(
                        container, slot,
                        &hand_item_type, &hand_item_stack, &hand_item_durability
                   );
                    #endif
                    #if DC_SERVER
                    action = full_hand_to_empty_slot(agent_id, container, slot);
                    #endif
                }
                else
                // must split stack
                {
                    #if DC_CLIENT
                    action = partial_hand_to_empty_slot(
                        container, slot,
                        hand_item_type, &hand_item_stack, hand_item_durability
                   );
                    #endif
                    #if DC_SERVER
                    action = partial_hand_to_empty_slot(container, slot, hand_item);
                    #endif
                }
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
                    #if DC_CLIENT
                    bool can_insert = container->can_insert_item(slot, hand_item_type);
                    #endif
                    #if DC_SERVER
                    bool can_insert = container->can_insert_item(slot, hand_item);
                    #endif
                    if (can_insert)
                    {
                        if (hand_item_stack == 1)
                        // hand only had one
                        {
                            #if DC_CLIENT
                            action = full_hand_to_occupied_slot(
                                container, slot,
                                &hand_item_type, &hand_item_stack, &hand_item_durability,
                                slot_item_type, slot_item_stack, slot_item_durability
                           );
                            #endif
                            #if DC_SERVER
                            action = full_hand_to_occupied_slot(agent_id, slot, slot_item);
                            #endif
                        }
                        else
                        // hand has >1 stack
                        {
                            #if DC_CLIENT
                            action = partial_hand_to_occupied_slot(
                                container, slot,
                                &hand_item_stack,
                                slot_item_type, slot_item_stack, 1, slot_item_durability
                           );
                            #endif
                            #if DC_SERVER
                            action = partial_hand_to_occupied_slot(slot, hand_item, slot_item, 1);
                            #endif
                        }
                    }
                }
            }
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType synthesizer_shopping_alpha_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType synthesizer_shopping_alpha_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    if (slot == NULL_SLOT || container_id == NULL_CONTAINER) return CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    ItemContainerSynthesizerUI* container = (ItemContainerSynthesizerUI*)get_container_ui(container_id);
    #endif
    #if DC_SERVER
    ItemContainerSynthesizer* container = (ItemContainerSynthesizer*)get_container(container_id);
    #endif
    if (container == NULL) return CONTAINER_ACTION_NONE;
    GS_ASSERT(container->type = AGENT_SYNTHESIZER);

    GS_ASSERT(slot >= 0 && slot < container->shopping_xdim*container->shopping_ydim);
    if (slot < 0 || slot >= container->shopping_xdim*container->shopping_ydim)
		return CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    int hand_item_type = player_hand_type_ui;
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    int stack_space = Item::get_max_stack_size(hand_item_type) - player_hand_stack_ui;
    #endif
    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    int hand_item_type = Item::get_item_type(hand_item);
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    int stack_space = Item::get_stack_space(hand_item);
    #endif

    if (hand_empty)
    {   // send purchase packet
        return PURCHASE_ITEM_FROM_SYNTHESIZER;
    }
    else if (stack_space > 0)
    {	// attempt to stack
		// get store item
		int xslot = slot % container->shopping_xdim;
		int yslot = slot / container->shopping_xdim;
		int cost;
		int item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
		GS_ASSERT(cost >= 0);
		if (item_type == NULL_ITEM_TYPE) return CONTAINER_ACTION_NONE;
		if (item_type != hand_item_type) return CONTAINER_ACTION_NONE;
		// we can stack
		return PURCHASE_ITEM_FROM_SYNTHESIZER;
	}

    return CONTAINER_ACTION_NONE;
}

#if DC_CLIENT
ContainerActionType synthesizer_shopping_beta_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType synthesizer_shopping_beta_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    return CONTAINER_ACTION_NONE;
}

#if DC_CLIENT
ContainerActionType craft_input_alpha_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType craft_input_alpha_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    // treat input slots exactly like normal container
    #if DC_CLIENT
    return alpha_action_decision_tree(container_id, slot);
    #endif
    #if DC_SERVER
    return alpha_action_decision_tree(agent_id, client_id, container_id, slot);
    #endif
}

#if DC_CLIENT
ContainerActionType craft_input_beta_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType craft_input_beta_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    // treat input slots exactly like normal container
    #if DC_CLIENT
    return beta_action_decision_tree(container_id, slot);
    #endif
    #if DC_SERVER
    return beta_action_decision_tree(agent_id, client_id, container_id, slot);
    #endif
}

#if DC_CLIENT
ContainerActionType craft_output_alpha_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType craft_output_alpha_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    if (slot == NULL_SLOT || container_id == NULL_CONTAINER) return CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    int hand_item_type = player_hand_type_ui;
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    int stack_space = Item::get_max_stack_size(hand_item_type) - player_hand_stack_ui;
    #endif
    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    int hand_item_type = Item::get_item_type(hand_item);
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    int stack_space = Item::get_stack_space(hand_item);
    #endif
 
    GS_ASSERT(stack_space >= 0);

    if (hand_empty) return CRAFT_ITEM_FROM_BENCH;
    if (stack_space > 0)
    {
        bool available;
        int craft_item_type = Item::get_selected_craft_recipe_type(container_id, slot, &available);
        if (available && hand_item_type == craft_item_type) return CRAFT_ITEM_FROM_BENCH;
    }
    return CONTAINER_ACTION_NONE;
}

#if DC_CLIENT
ContainerActionType craft_output_beta_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType craft_output_beta_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    return CONTAINER_ACTION_NONE;
}

#if DC_CLIENT
ContainerActionType no_container_alpha_action_decision_tree()
#endif
#if DC_SERVER
ContainerActionType no_container_alpha_action_decision_tree(int agent_id, int client_id)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    #endif

    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    bool hand_empty = (hand_item == NULL_ITEM);
    #endif

    if (!hand_empty)
    {   // remove
        #if DC_CLIENT
        action = full_hand_to_world(&hand_item_type, &hand_item_stack, &hand_item_durability);
        #endif
        #if DC_SERVER
        action = full_hand_to_world(agent_id);
        #endif
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType no_container_beta_action_decision_tree()
#endif
#if DC_SERVER
ContainerActionType no_container_beta_action_decision_tree(int agent_id, int client_id)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
    GS_ASSERT(hand_item_stack >= 1);
    #endif

    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    int hand_item_stack = Item::get_stack_size(hand_item);
    bool hand_empty = (hand_item == NULL_ITEM);
    #endif

    if (!hand_empty)
    {   // throw
        if (hand_item_stack == 1)
        {   // only 1 remains, so throw whole thing
            #if DC_CLIENT
            action = full_hand_to_world(&hand_item_type, &hand_item_stack, &hand_item_durability);
            #endif
            #if DC_SERVER
            action = full_hand_to_world(agent_id);
            #endif
        }
        else
        {   // throw just 1
            #if DC_CLIENT
            action = partial_hand_to_world(&hand_item_stack, 1);
            #endif
            #if DC_SERVER
            action = partial_hand_to_world(agent_id, 1);
            #endif
        }
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType smelter_alpha_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType smelter_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    #if DC_CLIENT
    return alpha_action_decision_tree(id, slot);
    #endif
    #if DC_SERVER
    return alpha_action_decision_tree(agent_id, client_id, id, slot);
    #endif
}

#if DC_CLIENT
ContainerActionType smelter_beta_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType smelter_beta_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    #if DC_CLIENT
    return beta_action_decision_tree(id, slot);
    #endif
    #if DC_SERVER
    return beta_action_decision_tree(agent_id, client_id, id, slot);
    #endif
}

}   // ItemContainer
