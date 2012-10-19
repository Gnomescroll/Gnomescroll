#include "item.hpp"

#if DC_SERVER
#include <item/server.hpp>
#include <item/container/_state.hpp>
#include <agent/_interface.hpp>
#endif

namespace Item
{

void Item::init(int item_type)
{
    this->type = item_type;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    this->durability = attr->max_durability;
    this->gas_decay = attr->gas_lifetime;
}


#if DC_SERVER

void Item::init_for_loading()
{   // use only by serializer
    // we will set defaults for state properties that are not important enough to serialize
    ItemAttribute* attr = get_item_attributes(this->type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    this->gas_decay = attr->gas_lifetime;
}

void ItemList::decay_gas()
{
    gas_tick++;
    if (gas_tick % GAS_TICK_INTERVAL != 0) return;
    
    // iterate item list
    // decay any gases
    // dont decay if container is cryofreezer

    int container_id;
    Item* item;
    ItemAttribute* attr;
    ItemContainer::ItemContainerInterface* container;
    for (unsigned int i=0; i<this->max; i++)
    {
        // get item
        if (this->objects[i].id == this->null_id) continue;
        item = &this->objects[i];
        attr = get_item_attributes(item->type);
        GS_ASSERT(attr != NULL);
        if (attr == NULL) continue;
        if (!attr->gas) continue;

        // particle items -- gases decay much faster
        // could do this with variable ttl in item particle,
        // but keeping this loop in one place
        if (item->location == IL_PARTICLE)
        {   // free item
            item->gas_decay -= GAS_TICK_INTERVAL;
            if (item->gas_decay <= 0)
            {
                int final_stack = consume_stack_item(item->id);
                if (final_stack > 0) item->gas_decay = attr->gas_lifetime;
            }
        }
        else if (item->location == IL_CONTAINER)
        {   // get container
            container_id = item->location_id;
            GS_ASSERT(container_id != NULL_CONTAINER);  // if it wasnt a particle it should be in a container
            if (container_id == NULL_CONTAINER) continue;

            container = ItemContainer::get_container(container_id);
            GS_ASSERT(container != NULL);
            if (container == NULL) continue;
            // ignore cryofreezer items
            if (container->type == CONTAINER_TYPE_CRYOFREEZER_SMALL)
            {
                item->gas_decay = attr->gas_lifetime;    // reset decay
                continue;
            }
            // dont decrement if sitting in fuel slot
            if (is_smelter(container->type)
            && ((ItemContainer::ItemContainerSmelter*)container)->is_fuel_slot(item->container_slot))
                continue;

            // decay item
            item->gas_decay -= GAS_TICK_INTERVAL;
            if (item->gas_decay <= -GAS_TICK_INTERVAL)
            {
                int stack_size = item->stack_size;
                int final_stack = consume_stack_item(item->id);
                if (final_stack > 0)
                {
                    item->gas_decay = attr->gas_lifetime;
                    if (stack_size != final_stack)
                    {
                        AgentID agent_id = container->owner;
                        Agent* agent = Agents::get_agent(agent_id);
                        if (agent != NULL)
                            send_item_state(item->id);
                    }
                }
            }
        }
        else if (item->location == IL_HAND)
        {   // hand
            item->gas_decay -= GAS_TICK_INTERVAL;
            if (item->gas_decay <= -GAS_TICK_INTERVAL)
            {
                int stack_size = item->stack_size;
                int final_stack = consume_stack_item(item->id);
                if (final_stack > 0)
                {
                    item->gas_decay = attr->gas_lifetime;
                    if (stack_size != final_stack)
                    {
                        Agent* agent = Agents::get_agent((AgentID)item->location_id);
                        if (agent != NULL)
                            send_item_state(item->id);
                    }
                }
            }
        }
        else GS_ASSERT(false);
    }
}

bool is_valid_location_data(ItemLocationType location, int location_id, int container_slot, const int assert_limit)
{
    bool valid = true;
    #define VERIFY_ITEM_LOCATION(COND) \
        GS_ASSERT_LIMIT((COND), assert_limit); \
        if (!(COND)) valid = false;

    VERIFY_ITEM_LOCATION(location != IL_NOWHERE);

    if (location == IL_PARTICLE)
    {
        VERIFY_ITEM_LOCATION(location != IL_PARTICLE || location_id != NULL_PARTICLE);
    }
    else
    if (location == IL_HAND)
    {
        VERIFY_ITEM_LOCATION(IS_VALID_AGENT_ID(location_id));
        VERIFY_ITEM_LOCATION(container_slot == 0);
    }
    else
    if (location == IL_CONTAINER)
    {
        VERIFY_ITEM_LOCATION(IS_VALID_CONTAINER_ID(location_id));
        VERIFY_ITEM_LOCATION(location_id != NULL_CONTAINER);
        VERIFY_ITEM_LOCATION(container_slot != NULL_SLOT);
        ItemContainerType container_type = ItemContainer::get_container_type(location_id);
        VERIFY_ITEM_LOCATION(container_type != CONTAINER_TYPE_NONE);
        // we can't check container slot max from attr for container blocks, because the configuration is not specialized enough
        // we can check for player containers though
        if (ItemContainer::container_type_is_attached_to_agent(container_type))
        {
            VERIFY_ITEM_LOCATION(container_slot >= 0 && (unsigned int)container_slot < ItemContainer::get_container_max_slots(container_type));
        }
    }

    #undef VERIFY_ITEM_LOCATION

    return valid;
}

void ItemList::verify_items()
{
    // use this macro for conditions which should mark the item as invalid item state
    // dont use it if the item's state does not align with meta info, like subscribers
    #define VERIFY_ITEM(COND, LIMIT, ITEM) \
        GS_ASSERT_LIMIT((COND), (LIMIT)); \
        if (!(COND)) (ITEM)->valid = false;
    
    const int LIMIT = 1;
    for (unsigned int k=0; k<this->max; k++)
    {
        if (this->objects[k].id == this->null_id) continue;
        Item* i = &this->objects[k];

        VERIFY_ITEM(is_valid_location_data(i->location, i->location_id, i->container_slot, LIMIT), LIMIT, i);

        VERIFY_ITEM(i->type != NULL_ITEM_TYPE, LIMIT, i);
    
        GS_ASSERT_LIMIT(i->subscribers.n >= 0, LIMIT);  // this should warn, but not mark the item as invalid
        VERIFY_ITEM((i->stack_size > 0 && i->stack_size <= MAX_STACK_SIZE) || i->stack_size == NULL_STACK_SIZE, LIMIT, i);
        VERIFY_ITEM((i->durability > 0 && i->durability <= MAX_DURABILITY) || i->durability == NULL_DURABILITY, LIMIT, i);

        if (i->location == IL_HAND)
        {
            GS_ASSERT_LIMIT(i->subscribers.n == 1, LIMIT);
            GS_ASSERT_LIMIT(i->subscribers.n <= 0 || i->location_id == i->subscribers.subscribers[0], LIMIT); // WARNING -- assumes client_id==agent_id
            GS_ASSERT_LIMIT(ItemContainer::get_agent_hand_item((AgentID)i->location_id) == i->id, LIMIT);
            VERIFY_ITEM(i->location_id >= 0 && i->location_id < MAX_AGENTS, LIMIT, i);
        }
        else
        if (i->location == IL_CONTAINER)
        {
            ItemContainerType type = ItemContainer::get_container_type(i->location_id);
            int owner = ItemContainer::get_container_owner(i->location_id);
            if (ItemContainer::container_type_is_attached_to_agent(type))
            {
                GS_ASSERT_LIMIT(i->subscribers.n == 1, LIMIT);
                GS_ASSERT_LIMIT(i->subscribers.n <= 0 || owner == i->subscribers.subscribers[0], LIMIT);
            }
            else if (owner != NULL_AGENT)
            {
                GS_ASSERT_LIMIT(i->subscribers.n == 1, LIMIT);
                GS_ASSERT_LIMIT(i->subscribers.n <= 0 || owner == i->subscribers.subscribers[0], LIMIT);
            }
        }
    }

    #undef VERIFY_ITEM
}
#endif

}   // Item
