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

/* ItemContainer methods */

void ItemContainer::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;
}

void ItemContainer::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        item->container_id = NULL_CONTAINER;
        item->container_slot = NULL_SLOT;
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

/* Nanite */

void ItemContainerNanite::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;

    #if DC_SERVER
    if (slot == 0) this->digestion_tick = 0;
    #endif
}

void ItemContainerNanite::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
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
        int coins_type = Item::get_item_type(coins_id);
        if (Item::get_stack_size(coins_id) >= Item::get_max_stack_size(coins_type)) return;
    }

    // tick digestion
    this->digestion_tick++;
    if (this->digestion_tick % NANITE_DIGESTION_RATE != 0) return;
    ItemID item_id = this->slot[0];
    if (item_id == NULL_ITEM) return;

    // decrement stack
    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    item->stack_size -= 1;
    
    Agent_state* a = STATE::agent_list->get(this->owner);

    // send new food state
    if (item->stack_size <= 0)
    {
        this->remove_item(0);
        if (a != NULL) send_container_remove(a->client_id, this->id, 0);
        Item::destroy_item(item_id);
    }
    else
    {
        if (a != NULL) Item::send_item_state(a->client_id, item->id);
    }

    // update coins
    if (coins_id == NULL_ITEM)
    {   // no coins were in coin slot, create new stack
        Item::Item* coin = Item::create_item((char*)"nanite_coin");
        GS_ASSERT(coin != NULL);
        if (a != NULL) Item::send_item_create(a->client_id, coin->id);
        this->insert_coins(coin->id);
        if (a != NULL) send_container_insert(a->client_id, coin->id, this->id, this->slot_max-1);
    }
    else
    {   // add to existing coin stack
        Item::Item* coins = Item::get_item_object(coins_id);
        coins->stack_size += 1;
        if (a != NULL) Item::send_item_state(a->client_id, coins_id);
    }
}
#endif

/* Crafting Bench */

void ItemContainerCraftingBench::insert_item(int slot, ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(this->is_valid_slot(slot));
    //GS_ASSERT(item_id != this->get_item(slot));
    this->slot[slot] = item_id;
    this->slot_count++;

    Item::Item* item = Item::get_item_object(item_id);
    GS_ASSERT(item != NULL);
    item->container_id = this->id;
    item->container_slot = slot;
}

void ItemContainerCraftingBench::remove_item(int slot)
{
    GS_ASSERT(this->is_valid_slot(slot));

    ItemID item_id = this->slot[slot];
    if (item_id != NULL_ITEM)
    {
        Item::Item* item = Item::get_item_object(this->slot[slot]);
        GS_ASSERT(item != NULL);
        item->container_id = NULL_CONTAINER;
        item->container_slot = NULL_SLOT;
    }

    this->slot[slot] = NULL_ITEM;
    this->slot_count--;
}

/* Initializer */

void init_container(ItemContainerInterface* container)
{
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    switch (container->type)
    {
        case AGENT_CONTAINER:
            container->attached_to_agent = true;
            container->init(AGENT_CONTAINER_X, AGENT_CONTAINER_Y);
            break;
        case AGENT_TOOLBELT:
            container->attached_to_agent = true;
            container->init(AGENT_TOOLBELT_X, AGENT_TOOLBELT_Y);
            break;
        case AGENT_NANITE:
            container->attached_to_agent = true;
            container->init(AGENT_NANITE_X, AGENT_NANITE_Y);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            container->attached_to_agent = false;
            container->init(CRAFTING_BENCH_UTILITY_X, CRAFTING_BENCH_UTILITY_Y);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
            container->attached_to_agent = false;
            container->init(CRAFTING_BENCH_REFINERY_X, CRAFTING_BENCH_REFINERY_Y);
            break;
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            container->attached_to_agent = false;
            container->init(CRYOFREEZER_SMALL_X, CRYOFREEZER_SMALL_Y);
            break;
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            container->attached_to_agent = false;
            container->init(STORAGE_BLOCK_SMALL_X, STORAGE_BLOCK_SMALL_Y);
            break;
        default:
            printf("%s - Unhandled container type %d\n", __FUNCTION__, container->type);
            GS_ASSERT(false);
            break;
    }
}

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

ContainerActionType full_hand_to_world(int client_id, int agent_id, ItemID* hand_item)
{
    GS_ASSERT(*hand_item != NULL_ITEM);

    // throw item
    ItemParticle::throw_agent_item(agent_id, *hand_item);
    // remove item from hand
    *hand_item = NULL_ITEM;
    send_hand_remove(client_id);
    return FULL_HAND_TO_WORLD;
}

ContainerActionType full_hand_to_empty_slot(int client_id, ItemContainerInterface* container, int slot, ItemID* hand_item)
{   // put hand item in slot
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item != NULL_ITEM);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    // put hand in slot
    container->insert_item(slot, *hand_item);
    send_container_insert(client_id, *hand_item, container->id, slot);
    // remove item from hand
    *hand_item = NULL_ITEM;
    send_hand_remove(client_id);
    return FULL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType full_hand_to_occupied_slot(int client_id, int slot, ItemID* hand_item, ItemID slot_item)
{ // add stacks
    GS_ASSERT(*hand_item != slot_item);
    GS_ASSERT(*hand_item != NULL_ITEM);
    GS_ASSERT(slot_item != NULL_ITEM);

    // merge the hand with the slot
    Item::merge_item_stack(*hand_item, slot_item); // Item::merge_item_stack(src, dest)
    // update the slot
    Item::broadcast_item_state(slot_item);
    // destroy the hand
    Item::destroy_item(*hand_item);
    // clear hand
    *hand_item = NULL_ITEM;
    // remove item from hand
    send_hand_remove(client_id);
    return FULL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_hand_to_empty_slot(int client_id, ItemContainerInterface* container, int slot, ItemID hand_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(hand_item != NULL_ITEM);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    // split 1 unit of hand item
    ItemID new_item = Item::split_item_stack(hand_item, 1);   // WARNING: CREATES ITEM
    // put in slot
    container->insert_item(slot, new_item);
    // update items
    Item::broadcast_item_state(hand_item);
    Item::broadcast_item_create(new_item);
    // insert split 1 stack into container
    send_container_insert(client_id, new_item, container->id, slot);
    // hand item id is unchanged
    return PARTIAL_HAND_TO_EMPTY_SLOT;
}

ContainerActionType partial_hand_to_occupied_slot(int client_id, int slot, ItemID hand_item, ItemID slot_item, int slot_item_space)
{
    GS_ASSERT(hand_item != slot_item);
    GS_ASSERT(hand_item != NULL_ITEM);
    GS_ASSERT(slot_item != NULL_ITEM);
    GS_ASSERT(slot_item_space > 0);

    Item::merge_item_stack(hand_item, slot_item, slot_item_space);
    // update items
    Item::broadcast_item_state(slot_item);
    Item::broadcast_item_state(hand_item);
    // hand item unchanged
    return PARTIAL_HAND_TO_OCCUPIED_SLOT;
}

ContainerActionType partial_slot_to_empty_hand(int client_id, int slot, ItemID* hand_item, ItemID slot_item)
{   // split stack, rounded down
    GS_ASSERT(*hand_item == NULL_ITEM);
    GS_ASSERT(slot_item != NULL_ITEM);

    // split slot item
    ItemID new_item = Item::split_item_stack_in_half(slot_item);
    // put new stack in hand
    *hand_item = new_item;
    // slot id is unchanged

    // send new state
    Item::broadcast_item_state(slot_item);
    Item::broadcast_item_create(new_item);
    send_hand_insert(client_id, new_item);
    return PARTIAL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_slot_to_empty_hand(int client_id, ItemContainerInterface* container, int slot, ItemID* hand_item, ItemID slot_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item == NULL_ITEM);
    GS_ASSERT(slot_item != NULL_ITEM);
    
    if (container == NULL) return CONTAINER_ACTION_NONE;
    
    // remove slot
    container->remove_item(slot);
    send_container_remove(client_id, container->id, slot);
    // put slot in hand
    *hand_item = slot_item;
    send_hand_insert(client_id, *hand_item);

    return FULL_SLOT_TO_EMPTY_HAND;
}

ContainerActionType full_hand_swap_with_slot(int client_id, ItemContainerInterface* container, int slot, ItemID* hand_item, ItemID slot_item)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(*hand_item != slot_item);
    GS_ASSERT(slot_item != NULL_ITEM);
    GS_ASSERT(*hand_item != NULL_ITEM);

    if (container == NULL) return CONTAINER_ACTION_NONE;

    // remove item from slot (we need to do this to trigger item's reference to container_id and slot
    container->remove_item(slot);
    send_container_remove(client_id, container->id, slot);
    // put hand in slot
    container->insert_item(slot, *hand_item);
    send_container_insert(client_id, *hand_item, container->id, slot);
    // put slot in hand
    *hand_item = slot_item;
    send_hand_insert(client_id, slot_item);

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
            action = full_slot_to_empty_hand(client_id, container, slot, &hand_item, slot_item);
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
            action = full_hand_to_empty_slot(client_id, container, slot, &hand_item);
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
                    action = full_hand_to_occupied_slot(client_id, slot, &hand_item, slot_item);
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
                        action = full_hand_swap_with_slot(client_id, container, slot, &hand_item, slot_item);
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
                        action = partial_hand_to_occupied_slot(client_id, slot, hand_item, slot_item, slot_item_space);
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
                action = full_hand_swap_with_slot(client_id, container, slot, &hand_item, slot_item);
                #endif
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
ContainerActionType nanite_alpha_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType nanite_alpha_action_decision_tree(int agent_id, int client_id, int id, int slot)
#endif
{
    ContainerActionType action = CONTAINER_ACTION_NONE;

    // client was inside container, but not a slot
    // do nothing
    GS_ASSERT(slot == 0 || slot == 1);
    if (slot < 0 || slot == NULL_SLOT || slot > 1) return action;

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

    // get container
    #if DC_CLIENT
    ItemContainerUIInterface* container = get_container_ui(id);
    #endif
    #if DC_SERVER
    ItemContainerInterface* container = get_container(id);
    #endif
    GS_ASSERT(container != NULL);
    if (container == NULL) return action;
    GS_ASSERT(container->type = AGENT_NANITE);
    
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

    if (slot == 0)
    {   // food slot
        if (hand_empty)
        {
            if (!slot_empty)
            {   // pick up food
                #if DC_CLIENT
                action = full_slot_to_empty_hand(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability,
                    slot_item_type, slot_item_stack, slot_item_durability
                );
                #endif
                #if DC_SERVER
                action = full_slot_to_empty_hand(client_id, container, slot, &hand_item, slot_item);
                #endif
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
                    action = full_hand_to_empty_slot(
                        container, slot,
                        &hand_item_type, &hand_item_stack, &hand_item_durability
                    );
                    #endif
                    #if DC_SERVER
                    action = full_hand_to_empty_slot(client_id, container, slot, &hand_item);
                    #endif
                }
            }
            else
            {   // see if we can merge some
                #if DC_CLIENT
                bool can_insert = container->can_insert_item(slot, hand_item_type);
                #endif
                #if DC_SERVER
                bool can_insert = container->can_insert_item(slot, hand_item);
                #endif
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
                        action = full_hand_to_occupied_slot(client_id, slot, &hand_item, slot_item);
                        #endif
                    }
                    else
                    // stacks will not completely merge
                    {
                        if (slot_item_space == 0)
                        // the stack is full
                        {
                            // do nothing.
                            // default container would swap here
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
                            action = partial_hand_to_occupied_slot(client_id, slot, hand_item, slot_item, slot_item_space);
                            #endif
                        }
                    }                    
                }
            }
        }
    }
    else if (slot == 1)
    {   // coin slot
        if (hand_empty)
        {
            if (!slot_empty)
            {   // pickup coins
                #if DC_CLIENT
                action = full_slot_to_empty_hand(
                    container, slot,
                    &hand_item_type, &hand_item_stack, &hand_item_durability,
                    slot_item_type, slot_item_stack, slot_item_durability
                );
                #endif
                #if DC_SERVER
                action = full_slot_to_empty_hand(client_id, container, slot, &hand_item, slot_item);
                #endif
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
                    action = full_hand_to_empty_slot(
                        container, slot,
                        &hand_item_type, &hand_item_stack, &hand_item_durability
                    );
                    #endif
                    #if DC_SERVER
                    action = full_hand_to_empty_slot(client_id, container, slot, &hand_item);
                    #endif
                }
            }
            else
            {
                if (hand_item_type == slot_item_type)
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
                        action = full_hand_to_occupied_slot(client_id, slot, &hand_item, slot_item);
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
                            action = full_hand_swap_with_slot(client_id, container, slot, &hand_item, slot_item);
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
                            action = partial_hand_to_occupied_slot(client_id, slot, hand_item, slot_item, slot_item_space);
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
                action = partial_slot_to_empty_hand(client_id, slot, &hand_item, slot_item);
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
                action = full_hand_to_empty_slot(client_id, container, slot, &hand_item);
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
                action = partial_hand_to_empty_slot(client_id, container, slot, hand_item);
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
                        action = full_hand_to_occupied_slot(client_id, slot, &hand_item, slot_item);
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
                        action = partial_hand_to_occupied_slot(client_id, slot, hand_item, slot_item, 1);
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
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType nanite_beta_action_decision_tree(int id, int slot)
#endif
#if DC_SERVER
ContainerActionType nanite_beta_action_decision_tree(int agent_id, int client_id, int id, int slot)
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
    GS_ASSERT(container->type = AGENT_NANITE);

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
                action = partial_slot_to_empty_hand(client_id, slot, &hand_item, slot_item);
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
                    action = full_hand_to_empty_slot(client_id, container, slot, &hand_item);
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
                    action = partial_hand_to_empty_slot(client_id, container, slot, hand_item);
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
                            action = full_hand_to_occupied_slot(client_id, slot, &hand_item, slot_item);
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
                            action = partial_hand_to_occupied_slot(client_id, slot, hand_item, slot_item, 1);
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
    #if DC_SERVER
    agent_hand_list[client_id] = hand_item;
    #endif

    return action;
}

#if DC_CLIENT
ContainerActionType nanite_shopping_alpha_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType nanite_shopping_alpha_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
#endif
{
    if (slot == NULL_SLOT || container_id == NULL_CONTAINER) return CONTAINER_ACTION_NONE;

    #if DC_CLIENT
    int hand_item_type = player_hand_type_ui;
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    //int stack_space = Item::get_max_stack_size(hand_item_type) - player_hand_stack_ui;
    #endif
    #if DC_SERVER
    ItemID hand_item = get_agent_hand(agent_id);
    int hand_item_type = Item::get_item_type(hand_item);
    bool hand_empty = (hand_item_type == NULL_ITEM_TYPE);
    //int stack_space = Item::get_stack_space(hand_item);
    #endif

    // TODO -- Allow multiple purchasing if it stacks
    if (hand_empty)
    {   // send purchase packet
        return PURCHASE_ITEM_FROM_NANITE;
    }

    return CONTAINER_ACTION_NONE;
}

#if DC_CLIENT
ContainerActionType nanite_shopping_beta_action_decision_tree(int container_id, int slot)
#endif
#if DC_SERVER
ContainerActionType nanite_shopping_beta_action_decision_tree(int agent_id, int client_id, int container_id, int slot)
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
        action = full_hand_to_world(client_id, agent_id, &hand_item);
        #endif
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
ContainerActionType no_container_beta_action_decision_tree()
#endif
#if DC_SERVER
ContainerActionType no_container_beta_action_decision_tree(int agent_id, int client_id)
#endif
{
    return CONTAINER_ACTION_NONE;
}

}
