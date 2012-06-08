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
    // decay any gases
    // dont decay if container is cryofreezer

    int container_id;
    Item* item;
    ItemAttribute* attr;
    ItemContainer::ItemContainerInterface* container;
    for (int i=0; i<this->n_max; i++)
    {
        // get item
        if (this->a[i] == NULL) continue;
        item = this->a[i];
        attr = get_item_attributes(item->type);
        GS_ASSERT(attr != NULL);
        if (attr == NULL) continue;
        if (!attr->gas) continue;

        // particle items -- gases decay much faster
        // could do this with variable ttl in item particle,
        // but keeping this loop in one place
        if (item->particle_id != NULL_PARTICLE)
        {   // free item
            item->gas_decay -= GAS_TICK_INTERVAL;
            if (item->gas_decay <= 0)
            {
                printf("Decaying gas particle\n");
                consume_stack_item(item->id);
            }
            continue;
        }

        // get container
        container_id = item->container_id;
        GS_ASSERT(container_id != NULL_CONTAINER);  // if it wasnt a particle it should be in a container
        if (container_id == NULL_CONTAINER) continue;

        if (container_id != AGENT_HAND)
        {
            container = ItemContainer::get_container(container_id);
            GS_ASSERT(container != NULL);
            // ignore cryofreezer items
            if (container != NULL && container->type == CONTAINER_TYPE_CRYOFREEZER_SMALL) continue;
        }
        else container = NULL;

        // decay item
        item->gas_decay -= GAS_TICK_INTERVAL;
        if (item->gas_decay <= 0)
        {
            printf("Decaying gas item\n");
            int stack_size = item->stack_size;
            int final_stack = consume_stack_item(item->id);
            if (final_stack > 0 && stack_size != final_stack)
            {
                int agent_id;
                if (container != NULL) agent_id = container->owner;
                else agent_id = item->container_slot;
                Agent_state* agent = ServerState::agent_list->get(agent_id);
                if (agent != NULL)
                    send_item_state(agent->client_id, item->id);
            }
            
        }
    }
    #endif
}

void ItemList::verify_items()
{
    #if DC_SERVER
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->container_id == NULL_CONTAINER && this->a[i]->particle_id == NULL_PARTICLE)
            printf("Item %d is lost\n", this->a[i]->id);
        if (this->a[i]->container_id != NULL_CONTAINER && this->a[i]->container_slot == NULL_SLOT)
            printf("Item %d is in container %d but has NULL_SLOT\n", this->a[i]->id, this->a[i]->container_id);
        if (this->a[i]->stack_size <= 0)
            printf("Item %d has no stack size\n", this->a[i]->id);
    }
    #endif
}


}   // Item
