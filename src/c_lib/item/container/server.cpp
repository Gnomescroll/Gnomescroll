#include "server.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <item/container/_state.hpp>

namespace ItemContainer
{

/* Transactions (no packets) */

// first order transactions

void remove_item_from_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_hand_list[agent_id] != NULL_ITEM);
    if (agent_hand_list[agent_id] == NULL_ITEM) return;
    Item::Item* item = Item::get_item(agent_hand_list[agent_id]);
    GS_ASSERT(item != NULL);
    if (item != NULL)
        item->location = IL_NOWHERE;
    agent_hand_list[agent_id] = NULL_ITEM;
}

void insert_item_in_hand(int agent_id, ItemID item_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
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
    if (owner_a != owner_b)
        Item::unsubscribe_agent_from_item(owner_a->id, item_id);
}

void transfer_item_from_container_to_hand(ItemID item_id, int container_id, int slot, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    ASSERT_VALID_AGENT_ID(agent_id);
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
    if (owner != hand_owner)
        Item::unsubscribe_agent_from_item(owner->id, item_id);
}

void transfer_item_from_hand_to_container(ItemID item_id, int container_id, int slot, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    ASSERT_VALID_AGENT_ID(agent_id);
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
    if (owner != hand_owner)
        Item::unsubscribe_agent_from_item(hand_owner->id, item_id);
}

// new unassigned item to container
void transfer_free_item_to_container(ItemID item_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);

    if (item_id == NULL_ITEM) return;
    if (container_id == NULL_CONTAINER) return;
    if (slot == NULL_SLOT) return;

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;

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
}

// new unassigned item to hand
void transfer_free_item_to_hand(ItemID item_id, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    ASSERT_VALID_AGENT_ID(agent_id);

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

void transfer_particle_to_container(ItemID item_id, int particle_id, int container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    GS_ASSERT(particle_id != NULL_PARTICLE);

    if (item_id == NULL_ITEM) return;
    if (container_id == NULL_CONTAINER) return;
    if (slot == NULL_SLOT) return;
    if (particle_id == NULL_PARTICLE) return;

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;

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
}

void transfer_particle_to_hand(ItemID item_id, int particle_id, int agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    ASSERT_VALID_AGENT_ID(agent_id);
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

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    
    close_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_container_open(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);

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

// transactions
bool agent_open_container(int agent_id, int container_id)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return false;
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;

    if (!agent_can_access_container(agent_id, container_id)) return false;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;

    GS_ASSERT(!container->attached_to_agent);   // we shouldnt use this function for attached containers
    if (container->attached_to_agent) return false;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return false;

    // release currently opened container
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        ItemContainerInterface* opened = get_container(opened_containers[agent_id]);
        GS_ASSERT(opened != NULL);
        if (opened == NULL) return false;
        opened_containers[agent_id] = NULL_CONTAINER;
        bool did_unlock = opened->unlock(a->id);
        GS_ASSERT(did_unlock);
        broadcast_container_unlock(container_id, agent_id);
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
    return true;
}

void agent_close_container(int agent_id, int container_id)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    GS_ASSERT(!container->attached_to_agent);
    if (container->attached_to_agent) return;

    Agent_state* a = ServerState::agent_list->get(agent_id);

    // throw anything in hand
    ItemID hand_item = get_agent_hand(agent_id);
    if (hand_item != NULL_ITEM)
    {
        remove_item_from_hand(agent_id);
        if (a != NULL) send_hand_remove(a->client_id);
        ItemParticle::throw_agent_item(agent_id, hand_item);
    }
    GS_ASSERT(get_agent_hand(agent_id) == NULL_ITEM);
    
    opened_containers[agent_id] = NULL_CONTAINER;

    bool did_unlock = container->unlock(agent_id);
    GS_ASSERT(did_unlock);
    broadcast_container_unlock(container->id, agent_id);

    unsubscribe_agent_from_container_contents(agent_id, container_id);
}

void unsubscribe_agent_from_container_contents(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER);
    
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
