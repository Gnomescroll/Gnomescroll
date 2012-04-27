#include "_interface.hpp"


#include <item/item_container.hpp>
#include <item/item.hpp>

#include <item/_state.hpp>

#if DC_SERVER
#include <item/net/StoC.hpp>
#endif

namespace Item
{

void init()
{
    item_container_list = new ItemContainerList;
    item_list           = new ItemList;

    #if DC_SERVER
    agent_inventory_list = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list  = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_nanite_list    = (int*)malloc(AGENT_MAX * sizeof(int));
    for (int i=0; i<AGENT_MAX; i++) agent_inventory_list[i] = NO_AGENT;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list [i] = NO_AGENT;
    for (int i=0; i<AGENT_MAX; i++) agent_nanite_list   [i] = NO_AGENT;
    #endif
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;
    if (item_list           != NULL) delete item_list;

    #if DC_SERVER
    if (agent_inventory_list != NULL) free(agent_inventory_list);
    if (agent_toolbelt_list  != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list    != NULL) free(agent_nanite_list);
    #endif
}


ItemContainer* get_container(int id)
{
    assert(item_container_list != NULL);
    return item_container_list->get(id);
}

Item* get_item(int id)
{
    return item_list->get(id);
}

}
 
// Client
#if DC_CLIENT
namespace Item
{

Item* create_item(int item_type, ItemID item_id)
{
    return item_list->create_type(item_type, item_id);
}

Item* create_item_particle(int item_type, ItemID item_id, float x, float y, float z, float vx, float vy, float vz)
{
    Item* item = create_item(item_type, item_id);
    if (item == NULL) return NULL;
    Particle::create_item_particle(item->id, x,y,z,vx,vy,vz);
    return item;
}

}
#endif 

// Server
#if DC_SERVER

namespace Item
{


Item* create_item(int item_type)
{
    return item_list->create_type(item_type);
}

Item* create_item_particle(int item_type, float x, float y, float z, float vx, float vy, float vz)
{
    Item* item = create_item(item_type);
    if (item == NULL) return NULL;
    Particle::create_item_particle(item->id, x,y,z,vx,vy,vz);
    return item;
}

void check_item_pickups()
{
    using Particle::ItemParticle;
    using Particle::item_particle_list;
    using Particle::item_particle_picked_up_StoC;
    
    for (int i=0; i<item_particle_list->n_max; i++)
    {
        if (item_particle_list->a[i] == NULL) continue;
        ItemParticle* item_particle = item_particle_list->a[i];
        Item* item = get_item(item_particle->item_id);
        assert(item != NULL);
    
        const static float pick_up_distance = 1.0f;
        Agent_state* agent = nearest_agent_in_range(item_particle->verlet.position, pick_up_distance);
        if (agent == NULL) continue;

        item_particle_picked_up_StoC pickup_msg;
        pickup_msg.id = item_particle->id;
        pickup_msg.agent_id = agent->id;
        pickup_msg.broadcast();

        item_particle_list->destroy(item_particle->id);

        int inventory_id = agent_inventory_list[agent->id];
        ItemContainer* ic = item_container_list->get(inventory_id);
        if (ic == NULL) return;

        int slot = auto_add_item_to_container(ic, item->item_id);   //insert item on server
        if (slot == NULL_SLOT) return;

        class item_create_StoC create_msg;
        create_msg.item_id = item->item_id;
        create_msg.item_type = item->type;
        create_msg.inventory_id = inventory_id;
        create_msg.inventory_slot = slot;
        create_msg.sendToClient(agent->client_id);
    }
}

}

#endif 
