#include "item_container.hpp"

#include <item/net/StoC.hpp>
#include <item/item.hpp>
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

/* ItemContainer methods */

void ItemContainer::insert_item(int slot, ItemID item_id)
{
    assert(item_id != NULL_ITEM);
    assert(this->is_valid_slot(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item* item = get_item_object(item_id);
    assert(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;
}

void ItemContainer::remove_item(int slot)
{
    assert(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item* item = get_item_object(this->slot[slot]);
        assert(item != NULL);
        item->container_id = NULL_CONTAINER;
        item->container_slot = NULL_SLOT;
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

/* Nanite */

void ItemContainerNanite::insert_item(int slot, ItemID item_id)
{
    assert(item_id != NULL_ITEM);
    assert(this->is_valid_slot(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item* item = get_item_object(item_id);
    assert(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;

    #if DC_SERVER
    if (slot == 0) this->digestion_tick = 0;
    #endif
}

void ItemContainerNanite::remove_item(int slot)
{
    assert(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item* item = get_item_object(this->slot[slot]);
        assert(item != NULL);
        item->container_id = NULL_CONTAINER;
        item->container_slot = NULL_SLOT;
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

#if DC_SERVER
void ItemContainerNanite::digest()
{
    // dont eat if coins are full
    ItemID coins_id = this->get_coins();
    if (coins_id != NULL_ITEM)
    {
        int coins_type = get_item_type(coins_id);
        if (get_stack_size(coins_id) >= get_max_stack_size(coins_type)) return;
    }

    // tick digestion
    this->digestion_tick++;
    if (this->digestion_tick % NANITE_DIGESTION_RATE != 0) return;
    ItemID item_id = this->slot[0];
    if (item_id == NULL_ITEM) return;

    // decrement stack
    Item* item = get_item_object(item_id);
    assert(item != NULL);
    item->stack_size -= 1;
    
    Agent_state* a = STATE::agent_list->get(this->owner);

    // send new food state
    if (item->stack_size <= 0)
    {
        this->remove_item(0);
        if (a != NULL) send_container_remove(a->client_id, this->id, 0);
        destroy_item(item_id);
    }
    else
    {
        if (a != NULL) send_item_state(a->client_id, item->id);
    }

    // update coins
    if (coins_id == NULL_ITEM)
    {   // no coins were in coin slot, create new stack
        Item* coin = create_item((char*)"nanite_coin");
        assert(coin != NULL);
        if (a != NULL) send_item_create(a->client_id, coin->id);
        this->insert_item(this->slot_max-1, coin->id);
        if (a != NULL) send_container_insert(a->client_id, coin->id, this->id, this->slot_max-1);
    }
    else
    {   // add to existing coin stack
        Item* coins = get_item_object(coins_id);
        coins->stack_size += 1;
        if (a != NULL) send_item_state(a->client_id, coins_id);
    }
}
#endif

/* Crafting Bench */

void ItemContainerCraftingBench::insert_item(int slot, ItemID item_id)
{
    assert(item_id != NULL_ITEM);
    assert(this->is_valid_slot(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item* item = get_item_object(item_id);
    assert(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;
}

void ItemContainerCraftingBench::remove_item(int slot)
{
    assert(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item* item = get_item_object(this->slot[slot]);
        assert(item != NULL);
        item->container_id = NULL_CONTAINER;
        item->container_slot = NULL_SLOT;
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

/* Initializer */

void init_container(ItemContainerInterface* container)
{
    assert(container != NULL);
    switch (container->type)
    {
        case AGENT_CONTAINER:
            container->init(AGENT_CONTAINER_X, AGENT_CONTAINER_Y);
            break;
        case AGENT_TOOLBELT:
            container->init(AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case AGENT_NANITE:
            container->init(AGENT_NANITE_X, AGENT_NANITE_Y);
            break;
        case CRAFTING_BENCH:
            container->init(CRAFTING_BENCH_X, CRAFTING_BENCH_Y);
            break;
        default:
            printf("init_container() - Unhandled container type %d\n", container->type);
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

    ItemContainerType container_type = get_container_type(id);

    #if DC_CLIENT
    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
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
            hand_item_durability = NULL_DURABILITY;
            #endif
            #if DC_SERVER
            ItemParticle::throw_item(agent_id, hand_item);
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
        ItemContainerUIInterface* container = get_container_ui(id);
        #endif
        #if DC_SERVER
        ItemContainerInterface* container = get_container(id);
        #endif
        assert(container != NULL);

        #if DC_CLIENT
        int slot_item_type = container->get_slot_type(slot);
        bool slot_empty = (slot_item_type == NULL_ITEM_TYPE);
        int slot_item_stack = container->get_slot_stack(slot);
        int slot_item_space = get_max_stack_size(slot_item_type) - slot_item_stack;
        int slot_item_durability = container->get_slot_durability(slot);
        #endif

        #if DC_SERVER
        ItemID slot_item = container->get_item(slot);
        bool slot_empty = (slot_item == NULL_ITEM);
        int slot_item_type = get_item_type(slot_item);
        //int slot_item_stack = get_stack_size(slot_item);
        int slot_item_space = get_stack_space(slot_item);
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

        // NANITE
        // if slot == 0
            // if hand empty
                // if slot occupied
                    // pick up food
            // else
                // if slot empty
                    // place food
                // NO SWAPS
        // slot == max-1
            // if hand empty
                // attempt to pickup poop
            // else
                // attempt to place poop
                    // --only accepts poop
                    // --behaviour is the same as otherwise
        /// else
            // --here we are shopping
            // if hand empty
                // attempt to pickup item
                    // no ui predction?
                    // server needs to do shopping
            // else
                // do nothing


        if (container_type == AGENT_NANITE)
        {
            if (slot == 0)
            {   // food slot
                if (hand_empty)
                {
                    if (!slot_empty)
                    {   // pick up food
                        #if DC_CLIENT
                        container->remove_item(slot);
                        hand_item_type = slot_item_type;
                        hand_item_stack = slot_item_stack;
                        hand_item_durability = slot_item_durability;
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
                else
                {
                    if (slot_empty)
                    {   // place food
                        #if DC_CLIENT
                        bool can_insert = container->can_insert_item(slot, hand_item_type);
                        #endif
                        #if DC_SERVER
                        bool can_insert = container->can_insert_item(slot, hand_item);
                        #endif
                        if (can_insert)
                        {
                            #if DC_CLIENT
                            container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                            hand_item_type = NULL_ITEM_TYPE;
                            hand_item_stack = 1;
                            hand_item_durability = NULL_DURABILITY;
                            #endif
                            #if DC_SERVER
                            container->insert_item(slot, hand_item);
                            send_container_insert(client_id, hand_item, container->id, slot);
                            hand_item = NULL_ITEM;
                            send_hand_remove(client_id);
                            #endif
                            action = FULL_HAND_TO_EMPTY_SLOT;
                        }
                    }
                    // dont swap
                }
            }
            else if (slot == container->slot_max-1)
            {   // coin slot
                if (hand_empty)
                {
                    if (!slot_empty)
                    {   // pickup coins
                        #if DC_CLIENT
                        container->remove_item(slot);
                        hand_item_type = slot_item_type;
                        hand_item_stack = slot_item_stack;
                        hand_item_durability = slot_item_durability;
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
                else
                {
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
                            #if DC_CLIENT
                            container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                            hand_item_type = NULL_ITEM_TYPE;
                            hand_item_stack = 1;
                            hand_item_durability = NULL_DURABILITY;
                            #endif
                            #if DC_SERVER
                            container->insert_item(slot, hand_item);
                            send_container_insert(client_id, hand_item, container->id, slot);
                            hand_item = NULL_ITEM;
                            send_hand_remove(client_id);
                            #endif
                            action = FULL_HAND_TO_EMPTY_SLOT;
                        }
                    }
                    else
                    {
                        if (hand_item_type == slot_item_type)
                        {
                            // hand stack will fit entirely in slot
                            if (hand_item_stack <= slot_item_space)
                            {   // FULL STACK MERGE
                                // add stacks
                                #if DC_CLIENT
                                container->insert_item(slot, slot_item_type, slot_item_stack + hand_item_stack, slot_item_durability);
                                hand_item_type = NULL_ITEM_TYPE;
                                hand_item_stack = 1;
                                hand_item_durability = NULL_DURABILITY;
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
                                    container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                                    hand_item_type = slot_item_type;
                                    hand_item_stack = slot_item_stack;
                                    hand_item_durability = slot_item_durability;
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
                                    container->insert_item(slot, slot_item_type, slot_item_stack + slot_item_space, slot_item_durability);
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
                    }
                }
            }
            else
            {   // shopping
                if (hand_empty)
                {   // send purchase packet
                    action = PURCHASE_ITEM_FROM_NANITE;
                }
            }
                
        }
        else
        {   // all other container
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
                    hand_item_durability = slot_item_durability;
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
                    container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                    hand_item_type = NULL_ITEM_TYPE;
                    hand_item_stack = 1;
                    hand_item_durability = NULL_DURABILITY;
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
                            container->insert_item(slot, slot_item_type, slot_item_stack + hand_item_stack, slot_item_durability);
                            hand_item_type = NULL_ITEM_TYPE;
                            hand_item_stack = 1;
                            hand_item_durability = NULL_DURABILITY;
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
                                container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                                hand_item_type = slot_item_type;
                                hand_item_stack = slot_item_stack;
                                hand_item_durability = slot_item_durability;
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
                                container->insert_item(slot, slot_item_type, slot_item_stack + slot_item_space, slot_item_durability);
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
                        container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                        hand_item_type = slot_item_type;
                        hand_item_stack = slot_item_stack;
                        hand_item_durability = slot_item_durability;
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
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
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

    ItemContainerType container_type = get_container_type(id);

    if (slot == NULL_SLOT || id < 0) return action;
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
    int slot_item_space = get_max_stack_size(slot_item_type) - slot_item_stack;
    int slot_item_durability = container->get_slot_durability(slot);

    bool hand_empty = (player_hand_type_ui == NULL_ITEM_TYPE);
    int hand_item_type = player_hand_type_ui;
    int hand_item_stack = player_hand_stack_ui;
    int hand_item_durability = player_hand_durability_ui;
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


    // if container_type is AGENT_NANITE
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

    // else
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
        if (container_type == AGENT_NANITE)
        {
            if (!slot_empty)
            {
                if (slot == 0 || slot == container->slot_max-1)
                {
                    if (slot_item_stack > 1)
                    // stack can split
                    {
                        // split stack, rounded down
                        #if DC_CLIENT
                        hand_item_type = slot_item_type;
                        hand_item_stack = slot_item_stack / 2;
                        hand_item_durability = slot_item_durability;
                        container->insert_item(slot, slot_item_type, slot_item_stack - hand_item_stack, slot_item_durability);
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
                // do nothing for shopping
            }
        }
        else
        {
            if (!slot_empty)
            // slot is occupied
            {
                if (slot_item_stack > 1)
                // stack can split
                {
                    // split stack, rounded down
                    #if DC_CLIENT
                    hand_item_type = slot_item_type;
                    hand_item_stack = slot_item_stack / 2;
                    hand_item_durability = slot_item_durability;
                    container->insert_item(slot, slot_item_type, slot_item_stack - hand_item_stack, slot_item_durability);
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
    }
    else
    // hand is holding something
    {
        assert(hand_item_stack >= 1);

        if (container_type == AGENT_NANITE)
        {
            if (slot_empty)
            {
                if (slot == 0 || slot == container->slot_max-1)
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
                            container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                            hand_item_type = NULL_ITEM_TYPE;
                            hand_item_stack = 1;
                            hand_item_durability = NULL_DURABILITY;
                            #endif
                            #if DC_SERVER
                            container->insert_item(slot, hand_item);
                            send_container_insert(client_id, hand_item, container->id, slot);
                            send_hand_remove(client_id);
                            destroy_item(hand_item);
                            hand_item = NULL_ITEM;
                            #endif
                            action = FULL_HAND_TO_EMPTY_SLOT;
                        }
                        else
                        // must split stack
                        {
                            #if DC_CLIENT
                            container->insert_item(slot, hand_item_type, 1, hand_item_durability);
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
                                container->insert_item(slot, slot_item_type, slot_item_stack + 1, slot_item_durability);
                                hand_item_type = NULL_ITEM_TYPE;
                                hand_item_stack = 1;
                                hand_item_durability = NULL_DURABILITY;
                                #endif
                                #if DC_SERVER
                                merge_item_stack(hand_item, slot_item);
                                // update dest
                                broadcast_item_state(slot_item);
                                // destroy src
                                send_hand_remove(client_id);
                                destroy_item(hand_item);
                                hand_item = NULL_ITEM;
                                #endif
                                action = FULL_HAND_TO_OCCUPIED_SLOT;
                            }
                            else
                            // hand has >1 stack
                            {
                                #if DC_CLIENT
                                container->insert_item(slot, slot_item_type, slot_item_stack + 1, slot_item_durability);
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

        }
        else
        {
            if (slot_empty)
            // slot is empty
            {
                // place 1 stack unit in slot
                if (hand_item_stack == 1)
                // only 1 in stack, do simple insert
                {
                    #if DC_CLIENT
                    container->insert_item(slot, hand_item_type, hand_item_stack, hand_item_durability);
                    hand_item_type = NULL_ITEM_TYPE;
                    hand_item_stack = 1;
                    hand_item_durability = NULL_DURABILITY;
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
                    container->insert_item(slot, hand_item_type, 1, hand_item_durability);
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
                            container->insert_item(slot, slot_item_type, slot_item_stack + 1, slot_item_durability);
                            hand_item_type = NULL_ITEM_TYPE;
                            hand_item_stack = 1;
                            hand_item_durability = NULL_DURABILITY;
                            #endif
                            #if DC_SERVER
                            merge_item_stack(hand_item, slot_item);
                            // update dest
                            broadcast_item_state(slot_item);
                            // destroy src
                            send_hand_remove(client_id);
                            destroy_item(hand_item);    // sends packet
                            hand_item = NULL_ITEM;
                            #endif
                            action = FULL_HAND_TO_OCCUPIED_SLOT;
                        }
                        else
                        // hand has >1 stack
                        {
                            #if DC_CLIENT
                            container->insert_item(slot, slot_item_type, slot_item_stack + 1, slot_item_durability);
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
    }

    #if DC_CLIENT
    player_hand_type_ui = hand_item_type;
    player_hand_stack_ui = hand_item_stack;
    player_hand_durability_ui = hand_item_durability;
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
void send_container_assign(class ItemContainerInterface* container, int client_id)
{
    class assign_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

void send_container_create(class ItemContainerInterface* container, int client_id)
{
    class create_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

void send_container_delete(class ItemContainerInterface* container, int client_id)
{
    class delete_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

#endif



}
