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
                destroy_item(item->id);
            }
            continue;
        }

        // get container
        container_id = item->container_id;
        GS_ASSERT(container_id != NULL_CONTAINER);  // if it wasnt a particle it should be in a container
        if (container_id == NULL_CONTAINER) continue;
        
        container = ItemContainer::get_container(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) continue;

        // ignore cryofreezer items
        if (container->type != CONTAINER_TYPE_CRYOFREEZER_SMALL) continue;

        // decay item
        item->gas_decay -= GAS_TICK_INTERVAL;
        if (item->gas_decay <= 0)
        {
            printf("Decaying gas particle\n");
            destroy_item(item->id);
        }
    }
    #endif
}


}   // Item
