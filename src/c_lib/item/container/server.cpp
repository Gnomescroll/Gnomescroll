/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "server.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <agent/_interface.hpp>
#include <item/container/_state.hpp>

namespace ItemContainer
{

/* Transactions (no packets) */

// first order transactions
// only use internally here, for the 2nd order transactions
void remove_item_from_hand(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    ItemContainerID hand_id = get_agent_hand(agent_id);
    IF_ASSERT(hand_id == NULL_CONTAINER) return;

    ItemContainerHand* hand = (ItemContainerHand*)get_container(hand_id);
    IF_ASSERT(hand == NULL) return;

    Item::Item* item = Item::get_item(hand->get_item());
    GS_ASSERT(item != NULL);
    if (item != NULL)
    {
        GS_ASSERT(item->location == IL_HAND);
        GS_ASSERT(item->location_id == agent_id);
        item->location = IL_NOWHERE;
    }
    hand->remove_item();
}

static void insert_item_in_hand(AgentID agent_id, ItemID item_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(item_id == NULL_ITEM) return;

    ItemContainerID hand_id = get_agent_hand(agent_id);
    if (hand_id == NULL_CONTAINER) return;
    ItemContainerHand* hand = (ItemContainerHand*)get_container(hand_id);
    IF_ASSERT(hand == NULL) return;

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item != NULL)
    {
        GS_ASSERT(item->location == IL_NOWHERE);
        item->location = IL_HAND;
        item->location_id = agent_id;
    }
    hand->insert_item(item_id);
}

// 2nd order transactions

void transfer_item_between_containers(ItemID item_id, ItemContainerID container_id_a, int slot_a, ItemContainerID container_id_b, int slot_b)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id_a != NULL_CONTAINER);
    GS_ASSERT(slot_a != NULL_SLOT);
    GS_ASSERT(container_id_b != NULL_CONTAINER);
    GS_ASSERT(slot_b != NULL_SLOT);

    ItemContainerInterface* container_a = get_container(container_id_a);
    IF_ASSERT(container_a == NULL) return;

    ItemContainerInterface* container_b = get_container(container_id_b);
    IF_ASSERT(container_b == NULL) return;
    IF_ASSERT(container_a->get_item(slot_a) != item_id) return;

    Agents::Agent* owner_a = NULL;
    if (container_a->owner != NULL_AGENT)
    {
        owner_a = Agents::get_agent(container_a->owner);
        GS_ASSERT(owner_a != NULL);
    }

    Agents::Agent* owner_b = NULL;
    if (container_b->owner != NULL_AGENT)
    {
        owner_b = Agents::get_agent(container_b->owner);
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

void transfer_item_from_container_to_hand(ItemID item_id, ItemContainerID container_id, int slot, AgentID agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    IF_ASSERT(!isValid(agent_id)) return;

    ItemID hand_item = get_agent_hand_item(agent_id);
    GS_ASSERT(hand_item == NULL_ITEM);

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;
    IF_ASSERT(container->get_item(slot) != item_id) return;

    Agents::Agent* owner = NULL;
    if (container->owner != NULL_AGENT)
    {
        owner = Agents::get_agent(container->owner);
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
    Agents::Agent* hand_owner = Agents::get_agent(agent_id);
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

void transfer_item_from_hand_to_container(ItemID item_id, ItemContainerID container_id, int slot, AgentID agent_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(slot != NULL_SLOT);
    IF_ASSERT(!isValid(agent_id)) return;

    ItemID hand_item = get_agent_hand_item(agent_id);
    IF_ASSERT(hand_item != item_id) return;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;

    IF_ASSERT(container->get_item(slot) != NULL_ITEM) return;

    Agents::Agent* hand_owner = Agents::get_agent(agent_id);
    GS_ASSERT(hand_owner != NULL);

    // remove item from hand
    remove_item_from_hand(agent_id);
    // send hand remove
    if (hand_owner != NULL)
        send_hand_remove(hand_owner->client_id);

    Agents::Agent* owner = NULL;
    if (container->owner != NULL_AGENT)
    {
        owner = Agents::get_agent(container->owner);
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

bool swap_item_between_hand_and_container(AgentID agent_id, ItemContainerID container_id, int slot)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return false;
    IF_ASSERT(slot == NULL_SLOT) return false;
    IF_ASSERT(!isValid(agent_id)) return false;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return false;

    Agents::Agent* container_owner = NULL;
    if (container->owner != NULL_AGENT)
    {
        container_owner = Agents::get_agent(container->owner);
        GS_ASSERT(container_owner != NULL);
    }

    Agents::Agent* hand_owner = Agents::get_agent(agent_id);
    GS_ASSERT(hand_owner != NULL);

    ItemID container_item_id = container->get_item(slot);
    GS_ASSERT(container_item_id != NULL_ITEM);
    if (container_item_id == NULL_ITEM) return false;
    Item::Item* container_item = Item::get_item(container_item_id);
    GS_ASSERT(container_item != NULL);
    if (container_item == NULL) return false;

    ItemID hand_item_id = get_agent_hand_item(agent_id);
    GS_ASSERT(hand_item_id != NULL_ITEM);
    if (hand_item_id == NULL_ITEM) return false;
    Item::Item* hand_item = Item::get_item(hand_item_id);
    GS_ASSERT(hand_item != NULL);
    if (hand_item == NULL) return false;

    GS_ASSERT(container_item_id != hand_item_id);
    if (container_item_id == hand_item_id) return false;

    container->remove_item(slot);
    if (container_owner != NULL)
        send_container_remove(container_owner->client_id, container->id, slot);

    remove_item_from_hand(agent_id);
    if (hand_owner != NULL)
        send_hand_remove(hand_owner->client_id);

    container->insert_item(slot, hand_item->id);
    if (container_owner != NULL)
        send_container_insert(container_owner->client_id, hand_item->id, container->id, slot);

    insert_item_in_hand(agent_id, container_item->id);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, container_item->id);

    if (hand_owner != container_owner)
    {
        if (hand_owner != NULL)
        {
            Item::unsubscribe_agent_from_item(agent_id, hand_item->id);
            Item::subscribe_agent_to_item(agent_id, container_item->id);
        }
        if (container_owner != NULL)
        {
            Item::unsubscribe_agent_from_item(agent_id, container_item->id);
            Item::subscribe_agent_to_item(agent_id, hand_item->id);
        }
    }

    return true;
}

// new unassigned item to container
bool transfer_free_item_to_container(ItemID item_id, ItemContainerID container_id, int slot)
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
    if (!container->is_valid_slot(slot)) return false;
    GS_ASSERT(container->get_item(slot) == NULL_ITEM);
    if (container->get_item(slot) != NULL_ITEM) return false;

    if (container->owner != NULL_AGENT)
        Item::subscribe_agent_to_item(container->owner, item->id);

    // insert item in container
    container->insert_item(slot, item->id);

    // send container insert
    Agents::Agent* owner = NULL;
    if (container->owner != NULL_AGENT)
    {
        owner = Agents::get_agent(container->owner);
        GS_ASSERT(owner != NULL);
    }
    if (owner != NULL)
        send_container_insert(owner->client_id, item->id, container->id, slot);

    return true;
}

// new unassigned item to hand
bool transfer_free_item_to_hand(ItemID item_id, AgentID agent_id)
{
    IF_ASSERT(item_id == NULL_ITEM) return false;
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(get_agent_hand_item(agent_id) != NULL_ITEM) return false;

    Item::subscribe_agent_to_item(agent_id, item_id);

    // put in hand
    insert_item_in_hand(agent_id, item_id);
    Agents::Agent* hand_owner = Agents::get_agent(agent_id);
    GS_ASSERT(hand_owner != NULL);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, item_id);

    return true;
}

bool transfer_particle_to_container(ItemID item_id, ItemParticleID particle_id, ItemContainerID container_id, int slot)
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

    if (container->owner != NULL_AGENT)
        Item::subscribe_agent_to_item(container->owner, item->id);

    // insert item in container
    container->insert_item(slot, item->id);

    // send container insert
    Agents::Agent* owner = NULL;
    if (container->owner != NULL_AGENT)
    {
        owner = Agents::get_agent(container->owner);
        GS_ASSERT(owner != NULL);
    }
    if (owner != NULL)
        send_container_insert(owner->client_id, item->id, container->id, slot);

    return true;
}

void transfer_particle_to_hand(ItemID item_id, ItemParticleID particle_id, AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(item_id == NULL_ITEM) return;
    IF_ASSERT(particle_id == NULL_PARTICLE) return;

    ItemID hand_item = get_agent_hand_item(agent_id);
    GS_ASSERT(hand_item == NULL_ITEM);

    // destroy particle
    ItemParticle::ItemParticle* particle = ItemParticle::get(particle_id);
    GS_ASSERT(particle != NULL);
    if (particle != NULL) GS_ASSERT(particle->item_id == item_id);
    ItemParticle::destroy(particle_id);

    Item::subscribe_agent_to_item(agent_id, item_id);

    // put in hand
    insert_item_in_hand(agent_id, item_id);
    Agents::Agent* hand_owner = Agents::get_agent(agent_id);
    GS_ASSERT(hand_owner != NULL);
    if (hand_owner != NULL)
        send_hand_insert(hand_owner->client_id, item_id);
}

// use this, instead of remove_item_from_hand, because it handles subscriptions
// use this only when no other hand transaction is sufficient
// this does not handle item particle creation
// you will need to call throw_item or whatever you want yourself
void transfer_hand_to_particle(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    ItemID hand_item = get_agent_hand_item(agent_id);
    IF_ASSERT(hand_item == NULL_ITEM) return;

    remove_item_from_hand(agent_id);

    Agents::Agent* agent = Agents::get_agent(agent_id);
    GS_ASSERT(agent != NULL);
    if (agent != NULL) send_hand_remove(agent->client_id);

    Item::unsubscribe_agent_from_item(agent_id, hand_item);

    ItemParticle::throw_agent_item(agent_id, hand_item);
}

/* Network */

//  tell client to assign container to an agent
void send_container_assign(ClientID client_id, ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    class assign_item_container_StoC msg;
    msg.container_id = container->id;
    msg.container_type = container->type;
    msg.agent_id = client_id;
    msg.sendToClient(client_id);
}

static bool pack_container_create(ItemContainerID container_id, create_item_container_StoC* msg)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    msg->container_id = container->id;
    msg->container_type = container->type;
    msg->chunk = container->chunk;
    return true;
}

void send_container_create(ClientID client_id, ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_create(ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    create_item_container_StoC msg;
    if (!pack_container_create(container_id, &msg)) return;
    msg.broadcast();
}

void send_container_delete(ClientID client_id, ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(client_id);
}

void broadcast_container_delete(ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    delete_item_container_StoC msg;
    msg.container_id = container_id;
    msg.broadcast();
}

static bool pack_container_lock(ItemContainerID container_id, lock_container_StoC* msg)
{
    GS_ASSERT(msg != NULL);
    if (msg == NULL) return false;

    IF_ASSERT(container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    IF_ASSERT(!isValid(container->owner)) return false;
    if (container->owner == NULL_AGENT) return false;

    msg->container_id = container->id;
    msg->agent_id = container->owner;
    return true;
}

void send_container_lock(ClientID client_id, ItemContainerID container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.sendToClient(client_id);
}

void broadcast_container_lock(ItemContainerID container_id)
{
    lock_container_StoC msg;
    if (!pack_container_lock(container_id, &msg)) return;
    msg.broadcast();
}

void broadcast_container_unlock(ItemContainerID container_id, int unlocking_agent_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    GS_ASSERT(container->owner == NULL_AGENT);
    if (container->owner != NULL_AGENT) return;

    unlock_container_StoC msg;
    msg.container_id = container->id;
    msg.agent_id = unlocking_agent_id;
    msg.broadcast();
}

void send_container_state(ClientID client_id, ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    if (container->owner != NULL_AGENT) send_container_lock(client_id, container_id);
}

void send_container_item_create(ClientID client_id, ItemID item_id, ItemContainerID container_id, int slot)
{
    GS_ASSERT(item_id != NULL_ITEM);
    GS_ASSERT(slot != NULL_SLOT);
    GS_ASSERT(container_id != NULL_CONTAINER);
    Item::send_item_create(client_id, item_id);
    send_container_insert(client_id, item_id, container_id, slot);
}

void send_container_close(AgentID agent_id, ItemContainerID container_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(container_id == NULL_CONTAINER) return;

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    close_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_container_open(AgentID agent_id, ItemContainerID container_id)
{
    IF_ASSERT(!isValid(agent_id)) return;

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    open_container_StoC msg;
    msg.container_id = container_id;
    msg.sendToClient(a->client_id);
}

void send_open_container_failed(ClientID client_id, ItemContainerID container_id, int event_id)
{
    open_container_failed_StoC msg;
    msg.container_id = container_id;
    msg.event_id = event_id;
    msg.sendToClient(client_id);
}

void send_smelter_fuel(ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;
    if (container->owner == NULL_AGENT) return;
    GS_ASSERT(Item::is_smelter(container->type));
    if (!Item::is_smelter(container->type)) return;
    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;
    GS_ASSERT(container_id == smelter->id);

    Agents::Agent* owner = Agents::get_agent(smelter->owner);
    IF_ASSERT(owner == NULL) return;

    smelter_fuel_StoC msg;
    msg.container_id = smelter->id;
    msg.fuel = smelter->fuel;
    msg.fuel_type = smelter->fuel_type;
    msg.sendToClient(owner->client_id);
}

void send_smelter_progress(ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;
    if (container->owner == NULL_AGENT) return;
    GS_ASSERT(Item::is_smelter(container->type));
    if (!Item::is_smelter(container->type)) return;
    ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;

    Agents::Agent* owner = Agents::get_agent(smelter->owner);
    IF_ASSERT(owner == NULL) return;

    smelter_progress_StoC msg;
    msg.progress = smelter->progress;
    msg.container_id = smelter->id;
    msg.sendToClient(owner->client_id);
}

// transactions
bool agent_open_container(AgentID agent_id, ItemContainerID container_id)
{
    IF_ASSERT(opened_containers == NULL) return false;
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(container_id == NULL_CONTAINER) return false;

    if (!agent_can_access_container(agent_id, container_id)) return false;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return false;

    // we shouldnt use this function for attached containers
    IF_ASSERT(container->attached_to_agent) return false;

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return false;

    // release currently opened container
    IF_ASSERT(opened_containers[agent_id] != NULL_CONTAINER)
    {
        ItemContainerInterface* opened = get_container(opened_containers[agent_id]);
        GS_ASSERT(opened != NULL);
        if (opened != NULL)
        {
            opened_containers[agent_id] = NULL_CONTAINER;
            bool did_unlock = opened->unlock(agent_id);
            GS_ASSERT(did_unlock);
            //broadcast_container_unlock(opened->id, agent_id);
            //unsubscribe_agent_from_container_contents(agent_id, opened->id);
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

static void agent_close_container(AgentID agent_id, ItemContainerID container_id, bool send_close)
{
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers == NULL) return;
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    GS_ASSERT(container_id == opened_containers[agent_id]);

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;
    IF_ASSERT(container->attached_to_agent) return;

    // throw anything in hand
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);

    if (send_close)
        send_container_close(agent_id, container_id);
    opened_containers[agent_id] = NULL_CONTAINER;

    bool did_unlock = container->unlock(agent_id);
    IF_ASSERT(!did_unlock) return;
    broadcast_container_unlock(container->id, agent_id);

    unsubscribe_agent_from_container_contents(agent_id, container_id);
}

void agent_close_container(AgentID agent_id, ItemContainerID container_id)
{
    agent_close_container(agent_id, container_id, true);    // send close as normal
}

void agent_close_container_silent(AgentID agent_id, ItemContainerID container_id)
{   // dont send container_close packet
    // this should be used if the client initiated the closing
    // there is a race condition that occurs otherwise
    // normally the client should handle this, but the handling in client is there
    // with did_close_container_block, but the race condition is still present and
    // i dont have an alternative solution
    agent_close_container(agent_id, container_id, false);
}

void unsubscribe_agent_from_container_contents(AgentID agent_id, ItemContainerID container_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    IF_ASSERT(container_id == NULL_CONTAINER) return;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;

    for (int i=0; i<container->slot_max; i++)
        if (container->slot[i] != NULL_ITEM)
            Item::unsubscribe_agent_from_item(agent_id, container->slot[i]);
}

}   // ItemContainer
