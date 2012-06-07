#include "item.hpp"

#if DC_SERVER
#include <item/server.hpp>
#endif

namespace Item
{

void Item::init(int item_type)
{
    this->type = item_type;
    this->group = get_item_group_for_type(item_type);
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    this->energy = attr->max_energy;
    this->durability = attr->max_durability;
}

void ItemList::tick()
{
    #if DC_SERVER
    gas_tick++;
    if (gas_tick % GAS_TICK_INTERVAL != 0) return;
    
    // iterate item list
    // decay item particle gases @30seconds
    // decay container item gases @60 seconds
    // dont decay if container is cryofreezer

    int container_id;
    Item* item;
    ItemAttribute* attr;
    ItemContainer::ItemContainerInterface* container;
    Agent_state* agent;
    for (int i=0; i<this->n_max; i++)
    {
        // get item
        if (this->a[i] == NULL) continue;
        item = this->a[i];
        attr = get_item_attributes(item->type);
        GS_ASSERT(attr != NULL);
        if (attr == NULL) continue;
        if (!attr->gas) continue;

        // get container
        container_id = item->container_id;
        if (container_id == NULL_CONTAINER)
        {   // free item
            item->gas_decay -= GAS_TICK_INTERVAL;
            if (item->gas_decay <= 0) destroy_item(item->id);
            continue;
        }
        
        container = ItemContainer::get_container(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) continue;

        // ignore cryofreezer items
        if (container->type != CONTAINER_TYPE_CRYOFREEZER_SMALL) continue;

        // decay item
        item->gas_decay -= GAS_TICK_INTERVAL;
        if (item->gas_decay <= 0)
        {
            destroy_item_silently(item->id);
            container->remove_item(item->container_slot);
            if (container->owner != NO_AGENT)
            {
                agent = ServerState::agent_list->get(container->owner);
                if (agent != NULL)
                {
                    ItemContainer::send_container_remove(agent->client_id, container->id, item->container_slot);
                    send_item_destroy(agent->client_id, item->id);
                }
            }
        }
    }
    #endif
}


}   // Item
