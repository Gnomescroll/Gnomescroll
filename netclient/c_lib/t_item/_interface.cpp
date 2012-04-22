#include "_interface.hpp"

//#include <c_lib/t_item/list.hpp>

#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/item_container.hpp>
#include <c_lib/t_item/item.hpp>

#include <c_lib/t_item/net/StoC.hpp>

#include <c_lib/input/handlers.hpp>

namespace t_item
{

Free_item_list* free_item_list = NULL;
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;

int INVENTORY_AGENTList[256];

void state_init()
{
    if (free_item_list != NULL)
    {
        printf("WARNING: attempt to call t_item::state_init() more than once\n");
        return;
    }
    free_item_list = new Free_item_list;
    item_container_list = new ItemContainerList;
    item_list = new ItemList;

    for(int i=0; i<256; i++) INVENTORY_AGENTList[i] = NO_AGENT;
}

void state_teardown()
{
    if (free_item_list != NULL) delete free_item_list;
    if (item_container_list != NULL) delete item_container_list;
    if (item_list != NULL) delete item_list;
}
 
void tick()
{
    free_item_list->tick();
#ifdef DC_SERVER
    check_item_pickups();
#endif
}

void draw()
{
    free_item_list->draw();

    //if (input_state.inventory)
        //draw_inventory();
}

}
 
/*
CLIENT
*/
#ifdef DC_CLIENT

namespace t_item
{
    int player_inventory_id = 0xffff;   //store id of player inventory
    class ItemContainer* player_inventory = NULL;
}

#endif 

/*
SERVER
*/
#ifdef DC_SERVER

namespace t_item
{

//const int NO_AGENT = 0xffff;

void create_agent_inventory(int agent_id, int client_id)
{
    ItemContainer* ic = item_container_list->create();
    ic->init_agent_inventory();

    assert(INVENTORY_AGENTList[agent_id] == NO_AGENT);
    assert((agent_id < 255) && (agent_id > 0));

    INVENTORY_AGENTList[agent_id] = ic->id;
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    a->inventory_id = ic->id;

    assign_agent_inventory_StoC p;
    p.inventory_id = ic->id;
    p.agent_id = agent_id;
    p.sendToClient(client_id);

    printf("Sending inventory to client %i \n", client_id);
}

void delete_agent_inventory(int agent_id)
{
    assert(INVENTORY_AGENTList[agent_id] != NO_AGENT);
    item_container_list->destroy(agent_id);
    INVENTORY_AGENTList[agent_id] == NO_AGENT;
}

void check_item_pickups()
{
#ifdef DC_SERVER
    for (int i=0; i<free_item_list->n_max; i++)
    {
        if (free_item_list->a[i] == NULL) continue;
        Free_item* free_item = free_item_list->a[i];

        const static float pick_up_distance = 0.5;
        Agent_state* agent = nearest_agent_in_range(free_item->verlet.position, pick_up_distance);

        if(agent == NULL) continue;

        printf("agent %i picked up item %i \n", agent->id, free_item->id);

        free_item_picked_up_StoC p1;
        p1.id = free_item->id;
        p1.agent_id = agent->id;
        p1.broadcast();

        t_item::free_item_list->destroy(free_item->id);


        /*
            Put item in agent inventory
        */
        assert(INVENTORY_AGENTList[agent->id] != NO_AGENT);

        int inventory_id = INVENTORY_AGENTList[agent->id];
        ItemContainer* ic = item_container_list->get(inventory_id);
        
        if(ic == 0)
        {
            printf("t_item::check_item_pickups, item container null \n");
            return;
        }

        //int slot = ic->get_empty_slot();
        if( ic->is_full() )
        {
            printf("t_item::check_item_pickups, Agent inventory full: item deleted, fix \n");
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

void create_free_item(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz)
{
    Free_item* f = free_item_list->create();
    if(f == NULL) return;
    f->init(x,y,z,vx,vy,vz);

    class free_item_create_StoC p;

    p.type = 0;
    p.id = f->id;
    p.x = x;
    p.y = y;
    p.z = z;
    p.mx = vx;
    p.my = vy;
    p.mz = vz;

    p.broadcast();
}

}

#endif 
