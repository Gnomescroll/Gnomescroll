#include "item.hpp"

#if DC_SERVER
#include <item/server.hpp>
#include <item/container/_state.hpp>
#endif

namespace Item
{

void Item::init(int item_type)
{
    this->type = item_type;
    ItemAttribute* attr = get_item_attributes(item_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    this->energy = attr->max_energy;
    this->durability = attr->max_durability;
    this->gas_decay = attr->gas_lifetime;
}

#if DC_SERVER
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
                        int agent_id = container->owner;
                        Agent_state* agent = ServerState::agent_list->get(agent_id);
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
                        int agent_id = item->location_id;
                        Agent_state* agent = ServerState::agent_list->get(agent_id);
                        if (agent != NULL)
                            send_item_state(item->id);
                    }
                }
            }
        }
        else GS_ASSERT(false);
    }
}

void ItemList::verify_items()
{
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;

        if (this->a[i]->location == IL_NOWHERE)
            printf("Item %d is lost\n", this->a[i]->id);

        if (this->a[i]->location == IL_CONTAINER && this->a[i]->location_id == NULL_CONTAINER)
            printf("Item %d is located in container but has NULL_CONTAINER location_id\n", this->a[i]->id);

        if (this->a[i]->location == IL_CONTAINER && this->a[i]->location_id != NULL_CONTAINER && this->a[i]->container_slot == NULL_SLOT)
            printf("Item %d is in container %d but has NULL_SLOT\n", this->a[i]->id, this->a[i]->location_id);

        if (this->a[i]->location == IL_PARTICLE && this->a[i]->location_id == NULL_PARTICLE)
            printf("Item %d is located in particle but has NULL_PARTICLE location_id\n", this->a[i]->id);
            
        if (this->a[i]->location == IL_HAND && (this->a[i]->location_id < 0 || this->a[i]->location_id >= AGENT_MAX))
            printf("Item %d is located in agent hand but has invalid agent location_id %d\n", this->a[i]->id, this->a[i]->location_id);
            
        if (this->a[i]->location == IL_HAND && this->a[i]->location_id >= 0 && this->a[i]->location_id < AGENT_MAX && ItemContainer::agent_hand_list[this->a[i]->location_id] == NULL_ITEM)
            printf("Item %d is located in agent hand but agent %d hand is empty\n", this->a[i]->id, this->a[i]->location_id);
            
        if (this->a[i]->location == IL_HAND && this->a[i]->location_id >= 0 && this->a[i]->location_id < AGENT_MAX && ItemContainer::agent_hand_list[this->a[i]->location_id] != this->a[i]->id)
            printf("Item %d is located in agent hand but agent %d hand is a different item, %d\n", this->a[i]->id, this->a[i]->location_id, ItemContainer::agent_hand_list[this->a[i]->location_id]);

        if (this->a[i]->stack_size <= 0)
            printf("Item %d has no stack size\n", this->a[i]->id);
    }
}
#endif

}   // Item
