#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/container/_state.hpp>

namespace ItemContainer
{

/* Transactions (no packets) */

// first order transactions
// only use internally here, for the 2nd order transactions
void remove_item_from_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(agent_hand_list[agent_id] != NULL_ITEM);
    if (agent_hand_list[agent_id] == NULL_ITEM) return;
    Item::Item* item = Item::get_item(agent_hand_list[agent_id]);
    GS_ASSERT(item != NULL);
    if (item != NULL)
        item->location = IL_NOWHERE;
    agent_hand_list[agent_id] = NULL_ITEM;
}

static void insert_item_in_hand(int agent_id, ItemID item_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(agent_hand_list[agent_id] == NULL_ITEM);
    if (agent_hand_list[agent_id] != NULL_ITEM) return;
    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL)
    {
        GS_ASSERT(item->location == IL_NOWHERE);
        item->location = IL_HAND;
        item->location_id = agent_id;
    }
    agent_hand_list[agent_id] = item_id;
}

// 2nd order transactions

void transfer_item_between_containers(ItemID item_id, int container_id_a, int slot_a, int container_id_b, int slot_b)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id_a != NULL_CONTAINER);
    GS_ASSERT(slot_a != NULL_SLOT);
    GS_ASSERT(container_id_b != NULL_CONTAINER);
    GS_ASSERT(slot_b != NULL_SLOT);
    
    ItemContainerInterface* container_a = get_container(container_id_a);
    GS_ASSERT(container_a != NULL);
    if (container_a == NULL) return;
    
    ItemContainerInterface* container_b = get_container(container_id_b);
    GS_ASSERT(container_b != NULL);
    if (container_b == NULL) return;

    GS_ASSERT(container_a->get_item(slot_a) == item_id);
    if (container_a->get_item(slot_a) != item_id) return;

    Agent_state* owner_a = NULL;
    if (container_a->owner != NO_AGENT)
    {
        owner_a = ServerState::agent_list->get(container_a->owner);
        GS_ASSERT(owner_a != NULL);
    }
    
    Agent_state* owner_b = NULL;
    if (container_b->owner != NO_AGENT)
    {
        owner_b = ServerState::agent_list->get(container_b->owner);
        GS_ASSERT(owner_b != NULL);
    }
    
    // remove item from container a
    container_a->remove_item(slot_a);
    // send container remove
    if (owner_a != NULL)
        send_container_remove(owner_a->client_id, container_id_a, slot_a);

    // add item to container b
    container_b->insert_item(slot_b, item_id);
    // send container insert
    if (owner_b != NULL)
        send_container_insert(owner_b->client_id, item_id, container_id_b, slot_b);

    // if container owners match, nothing
    // if container owners don't match,
    // unsubscribe owner a from item
    // (replace with containers' subscription list complement intersection 
    if (owner_a != owner_b && owner_a != NULL)
        Item::unsubscribe_agent_from_item(owner_a->id, item_id);
}

void transfer_item_from_container_to_hand(ItemID item_id, int container_id, int slot, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item == NULL_ITEM);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    
    GS_ASSERT(container->get_item(slot) == item_id);
    if (container->get_item(slot) != item_id) return;

    Agent_state* owner = NULL;
    if (container->owner != NO_AGENT)
    {
        owner = ServerState::agent_list->get(container->owner);
        GS_ASSERT(owner != NULL);
    }
    
    // remove item from container
    container->remove_item(slot);
    // send container remove
    if (owner != NULL)
        send_container_remove(owner->client_id, container_id, slot);

    // insert item in hand
    insert_item_in_hand(agent_id, item_id);

    // send hand insert
    Agent_state* hand_owner = ServerState::agent_list->get(agent_id);
    GS_ASSERT(hand_owner != NULL);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, item_id);

    // if container owners match, nothing
    // if container owners don't match,
    // unsubscribe container owner from item
    // (replace with containers' subscription list complement intersection 
    if (owner != hand_owner && owner != NULL)
        Item::unsubscribe_agent_from_item(owner->id, item_id);
}

void transfer_item_from_hand_to_container(ItemID item_id, int container_id, int slot, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item == item_id);
    if (hand_item != item_id) return;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    
    GS_ASSERT(container->get_item(slot) == NULL_ITEM);
    if (container->get_item(slot) != NULL_ITEM) return;

    Agent_state* hand_owner = ServerState::agent_list->get(agent_id);
    GS_ASSERT(hand_owner != NULL);

    // remove item from hand
    remove_item_from_hand(agent_id);
    // send hand remove
    if (hand_owner != NULL)
        send_hand_remove(hand_owner->client_id);

    Agent_state* owner = NULL;
    if (container->owner != NO_AGENT)
    {
        owner = ServerState::agent_list->get(container->owner);
        GS_ASSERT(owner != NULL);
    }

    // insert item in container
    container->insert_item(slot, item_id);
    // send container insert
    if (owner != NULL)
        send_container_insert(owner->client_id, item_id, container->id, slot);
    
    // if container owners match, nothing
    // if container owners don't match,
    // unsubscribe owner from item
    // (replace with containers' subscription list complement intersection 
    if (owner != hand_owner && hand_owner != NULL)
        Item::unsubscribe_agent_from_item(hand_owner->id, item_id);
}

// new unassigned item to container
bool transfer_free_item_to_container(ItemID item_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    if (item_id == NULL_ITEM) return false;
    if (container_id == NULL_CONTAINER) return false;
    if (slot == NULL_SLOT) return false;

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return false;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;

    GS_ASSERT(container->is_valid_slot(slot));
    GS_ASSERT(container->get_item(slot) == NULL_ITEM);

    if (container->owner != NO_AGENT)
        Item::subscribe_agent_to_item(container->owner, item->id);

    // insert item in container
    container->insert_item(slot, item->id);

    // send container insert
    Agent_state* owner = NULL;
    if (container->owner != NO_AGENT)
    {
        owner = ServerState::agent_list->get(container->owner);
        GS_ASSERT(owner != NULL);
    }
    if (owner != NULL)
        send_container_insert(owner->client_id, item->id, container->id, slot);

    return true;
}

// new unassigned item to hand
void transfer_free_item_to_hand(ItemID item_id, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    if (item_id == NULL_ITEM) return;

    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item == NULL_ITEM);

    Item::subscribe_agent_to_item(agent_id, item_id);

    // put in hand
    insert_item_in_hand(agent_id, item_id);
    Agent_state* hand_owner = ServerState::agent_list->get(agent_id);
    GS_ASSERT(hand_owner != NULL);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, item_id);
}

bool transfer_particle_to_container(ItemID item_id, int particle_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    GS_ASSERT(particle_id != NULL_PARTICLE);

    if (item_id == NULL_ITEM) return false;
    if (container_id == NULL_CONTAINER) return false;
    if (slot == NULL_SLOT) return false;
    if (particle_id == NULL_PARTICLE) return false;

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return false;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;

    GS_ASSERT(container->is_valid_slot(slot));
    GS_ASSERT(container->get_item(slot) == NULL_ITEM);

    // destroy particle
    ItemParticle::ItemParticle* particle = ItemParticle::get(particle_id);
    GS_ASSERT(particle != NULL);
    if (particle != NULL) GS_ASSERT(particle->item_id == item_id);
    ItemParticle::destroy(particle_id);

    if (container->owner != NO_AGENT)
        Item::subscribe_agent_to_item(container->owner, item->id);

    // insert item in container
    container->insert_item(slot, item->id);

    // send container insert
    Agent_state* owner = NULL;
    if (container->owner != NO_AGENT)
    {
        owner = ServerState::agent_list->get(container->owner);
        GS_ASSERT(owner != NULL);
    }
    if (owner != NULL)
        send_container_insert(owner->client_id, item->id, container->id, slot);
        
    return true;
}

void transfer_particle_to_hand(ItemID item_id, int particle_id, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(particle_id != NULL_PARTICLE);

    if (item_id == NULL_ITEM) return;
    if (particle_id == NULL_PARTICLE) return;

    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item == NULL_ITEM);

    // destroy particle
    ItemParticle::ItemParticle* particle = ItemParticle::get(particle_id);
    GS_ASSERT(particle != NULL);
    if (particle != NULL) GS_ASSERT(particle->item_id == item_id);
    ItemParticle::destroy(particle_id);

    Item::subscribe_agent_to_item(agent_id, item_id);

    // put in hand
    insert_item_in_hand(agent_id, item_id);
    Agent_state* hand_owner = ServerState::agent_list->get(agent_id);
    GS_ASSERT(hand_owner != NULL);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, item_id);
}

// use this, instead of remove_item_from_hand, because it handles subscriptions
// use this only when no other hand transaction is sufficient
// this does not handle item particle creation
// you will need to call throw_item or whatever you want yourself
void transfer_hand_to_particle(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    ItemID hand_item = get_agent_hand(agent_id);
    GS_ASSERT(hand_item != NULL_ITEM);
    if (hand_item == NULL_ITEM) return;

    remove_item_from_hand(agent_id);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    GS_ASSERT(agent != NULL);
    if (agent != NULL) send_hand_remove(agent->client_id);
    
    Item::unsubscribe_agent_from_item(agent_id, hand_item);

    ItemParticle::throw_agent_item(agent_id, hand_item);
}

/* Network */

//  tell client to assign container to an agent
void send_container_assign(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    class assign_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

static bool pack_container_create(int container_id, create_item_container_StoC* msg)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    msg->container_id = container->id;
    msg->container_type = container->type;
    msg->chunk = container->chunk;
    return true;
}

void send_container_create(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_create(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.broadcast();
}

void send_container_delete(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(client_id);
}

void broadcast_container_delete(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.broadcast();
}

static bool pack_container_lock(int container_id, lock_container_StoC* msg)
{
    GS_ASSERT(msg != NULL);
    if (msg == NULL) return false;

    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    ASSERT_VALID_AGENT_ID(container->owner);
    IF_INVALID_AGENT_ID(container->owner) return false;
    if (container->owner == NO_AGENT) return false;
    
    msg->container_id = container->id;
    msg->agent_id = container->owner;
    return true;
}

void send_container_lock(int client_id, int container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_lock(int container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.broadcast();
}

void broadcast_container_unlock(int container_id, int unlocking_agent_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    GS_ASSERT(container->owner == NO_AGENT);
    if (container->owner != NO_AGENT) return;

    unlock_container_StoC msg;
    msg.container_id = container->id;
    msg.agent_id = unlocking_agent_id;
    msg.broadcast();
}

void send_container_state(int client_id, int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    if (container->owner != NO_AGENT) send_container_lock(client_id, container_id);
}

void send_container_item_create(int client_id, ItemID item_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(slot != NULL_SLOT);
    GS_ASSERT(container_id != NULL_CONTAINER);
    Item::send_item_create(client_id, item_id);
    send_container_insert(client_id, item_id, container_id, slot);
}

void send_container_close(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;
    
    close_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_container_open(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    
    open_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_open_container_failed(int client_id, int container_id, int event_id)
{
    open_container_failed_StoC msg;
    msg.container_id = container_id;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

void send_smelter_fuel(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    if (container->owner == NO_AGENT) return;
    GS_ASSERT(Item::is_smelter(container->type));
    if (!Item::is_smelter(container->type)) return;
    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;

    smelter_fuel_StoC msg;
    msg.container_id = smelter->id;
    msg.fuel = smelter->fuel;
    msg.fuel_type = smelter->fuel_type;
    msg.sendToClient(smelter->owner);
}

void send_smelter_progress(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    if (container->owner == NO_AGENT) return;
    GS_ASSERT(Item::is_smelter(container->type));
    if (!Item::is_smelter(container->type)) return;
    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;

    smelter_progress_StoC msg;
    msg.progress = smelter->progress;
    msg.container_id = smelter->id;
    msg.sendToClient(smelter->owner);
}

// transactions
bool agent_open_container(int agent_id, int container_id)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;

    if (!agent_can_access_container(agent_id, container_id)) return false;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;

    GS_ASSERT(!container->attached_to_agent);   // we shouldnt use this function for attached containers
    if (container->attached_to_agent) return false;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return false;

    // release currently opened container
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        GS_ASSERT(false);
        ItemContainerInterface* opened = get_container(opened_containers[agent_id]);
        GS_ASSERT(opened != NULL);
        if (opened != NULL)
        {
            opened_containers[agent_id] = NULL_CONTAINER;
            bool did_unlock = opened->unlock(a->id);
            GS_ASSERT(did_unlock);
            broadcast_container_unlock(container_id, agent_id);
        }
    }

    // place player lock on container if we want
    bool did_lock = container->lock(a->id);
    GS_ASSERT(did_lock);
    GS_ASSERT(opened_containers[agent_id] == NULL_CONTAINER);
    opened_containers[agent_id] = container->id;
    // send new lock to players
    broadcast_container_lock(container->id);
    
    // send container contents to player
    send_container_contents(a->id, a->client_id, container_id);

    if (Item::is_smelter(container->type))
    {   // send fuel, progress state
        send_smelter_fuel(container->id);
        send_smelter_progress(container->id);
    }
    
    return true;
}

void agent_close_container(int agent_id, int container_id, bool suppress_close_msg)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    GS_ASSERT(container_id == opened_containers[agent_id]);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;

    GS_ASSERT(!container->attached_to_agent);
    if (container->attached_to_agent) return;

    // throw anything in hand
    ItemID hand_item = get_agent_hand(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);
    
    if (!suppress_close_msg)
        send_container_close(agent_id, container_id);
    opened_containers[agent_id] = NULL_CONTAINER;

    bool did_unlock = container->unlock(agent_id);
    GS_ASSERT(did_unlock);
    if (!did_unlock) return;
    broadcast_container_unlock(container->id, agent_id);

    unsubscribe_agent_from_container_contents(agent_id, container_id);
}

void unsubscribe_agent_from_container_contents(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;

    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        Item::unsubscribe_agent_from_item(agent_id, container->slot[i]);
    }
}

}   // ItemContainer
