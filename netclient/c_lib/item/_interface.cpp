#include "_interface.hpp"


#include <item/item_container.hpp>
#include <item/item.hpp>

#include <item/_state.hpp>



#if DC_SERVER
#include <item/net/StoC.hpp>
#endif

namespace item
{

void init()
{
    item_container_list = new ItemContainerList;
    item_list = new ItemList;

    #if DC_SERVER
    agent_inventory_list = (int*)malloc(AGENTS_MAX, sizeof(int));
    agent_toolbelt_list = (int*)malloc(AGENTS_MAX, sizeof(int));
    agent_nanite_list = (int*)malloc(AGENTS_MAX, sizeof(int));
    for (int i=0; i<256; i++) agent_inventory_list[i] = NO_AGENT;
    for (int i=0; i<256; i++) agent_toolbelt_list[i] = NO_AGENT;
    for (int i=0; i<256; i++) agent_nanite_list[i] = NO_AGENT;
    #endif
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;
    if (item_list != NULL) delete item_list;

    if (agent_inventory_list != NULL) free(agent_inventory_list);
    if (agent_toolbelt_list != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list != NULL) free(agent_nanite_list);
    
}

}
 
/*
CLIENT
*/
#if DC_CLIENT

namespace item
{


//move item
void move_item(int inventory_id1, int inventory_id2, int slot1, int slot2)
{
/*
    Fill in
*/
}

}
#endif 

/*
SERVER
*/
#if DC_SERVER

namespace item
{

/*
    Inventory
*/



void check_item_pickups()
{
#if 0
    for (int i=0; i<free_item_list->n_max; i++)
    {
        if (free_item_list->a[i] == NULL) continue;
        * item_particle = item_particle_list->a[i];

        const static float pick_up_distance = 0.5;
        Agent_state* agent = nearest_agent_in_range(item_particle->verlet.position, pick_up_distance);

        if (agent == NULL) continue;

        printf("agent %i picked up item %i \n", agent->id, item_particle->id);

        item_particle_picked_up_StoC p1;
        p1.id = item_particle->id;
        p1.agent_id = agent->id;
        p1.broadcast();

        item::item_particle_list->destroy(item_particle->id);


        /*
            Put item in agent inventory
        */
        assert(agent_inventory_list[agent->id] != NO_AGENT);

        int inventory_id = agent_inventory_list[agent->id];
        ItemContainer* ic = item_container_list->get(inventory_id);
        
        if (ic == 0)
        {
            printf("item::check_item_pickups, item container null \n");
            return;
        }

        //int slot = ic->get_empty_slot();
        if ( ic->is_full() )
        {
            printf("item::check_item_pickups, Agent inventory full: item deleted, fix \n");
            return;
        }

        int item_type = rand()%12;

        int slot = ic->create_item(item_type);   //insert item on server

        class item_create_StoC p2;
        p2.item_id = item_type;
        p2.item_type = item_type;
        p2.inventory_id = inventory_id;
        p2.inventory_slot = slot;

        p2.sendToClient(agent->id); //warning, assumes agent and player id are same
        //p2.broadcast();
    }
#endif
}

}

#endif 
