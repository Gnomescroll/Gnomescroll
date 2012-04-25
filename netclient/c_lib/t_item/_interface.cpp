#include "_interface.hpp"

//#include <c_lib/t_item/list.hpp>

#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/item_container.hpp>
#include <c_lib/t_item/item.hpp>

#include <c_lib/t_item/net/StoC.hpp>

#if DC_CLIENT
#include <c_lib/input/handlers.hpp>
#endif

namespace t_item
{

Free_item_list* free_item_list = NULL;
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;

int AgentInventoryList[256];
int AgentToolbeltList[256];
int AgentNaniteList[256];

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

    for(int i=0; i<256; i++) AgentInventoryList[i] = NO_AGENT;
    for(int i=0; i<256; i++) AgentToolbeltList[i] = NO_AGENT;
    for(int i=0; i<256; i++) AgentNaniteList[i] = NO_AGENT;
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
#if DC_SERVER
    check_item_pickups();
#endif
}

#if DC_CLIENT
void draw()
{
    free_item_list->draw();
}


#endif

}
 
/*
CLIENT
*/
#if DC_CLIENT

namespace t_item
{
    int player_inventory_id = -1;   //store id of player inventory
    int player_inventory_id = -1;   //store id of player inventory
    int player_inventory_id = -1;   //store id of player inventory

    class ItemContainer* player_inventory = NULL;
    class ItemContainer* player_toolbar = NULL;
    class ItemContainer* player_nanite = NULL;
}

//move item within inventory
//void move_item(int inventory_id, int slot1, int slot2) {}

//move item
void move_item(int inventory_id1, int inventory_id2, int slot1, int slot2)
{
/*
    Fill in
*/
}

#endif 

/*
SERVER
*/
#if DC_SERVER

namespace t_item
{

//const int NO_AGENT = 0xffff;

/*
    Inventory
*/
void create_agent_inventory(int agent_id, int client_id)
{
    ItemContainer* ic = item_container_list->create();
    ic->init_agent_inventory();

    assert(AgentInventoryList[agent_id] == NO_AGENT);
    assert((agent_id < 255) && (agent_id > 0));

    AgentInventoryList[agent_id] = ic->id;
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    a->inventory_id = ic->id;

    class assign_agent_inventory_StoC p;
    p.inventory_id = ic->id;
    p.agent_id = agent_id;
    p.sendToClient(client_id);

    printf("Sending inventory id to client %i \n", client_id);
}

void delete_agent_inventory(int agent_id)
{
    int inventory_id = AgentInventoryList[agent_id];
    assert(AgentInventoryList[agent_id] != NO_AGENT);
    assert(item_container_list->get(inventory_id) != NULL);

    item_container_list->destroy(inventory_id);
    AgentInventoryList[agent_id] = NO_AGENT;
}

/*
    Toolbar
*/
void create_agent_toolbelt(int agent_id, int client_id)
{
    ItemContainer* ic = item_container_list->create();
    ic->init_agent_toolbelt();

    assert(AgentToolbeltList[agent_id] == NO_AGENT);
    assert((agent_id < 255) && (agent_id > 0));

    AgentToolbeltList[agent_id] = ic->id;
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    a->toolbar_id = ic->id;

    class assign_agent_toolbelt_StoC p;
    p.inventory_id = ic->id;
    p.agent_id = agent_id;
    p.sendToClient(client_id);

    printf("Sending toolbar id to client %i \n", client_id);
}

void delete_agent_toolbelt(int agent_id)
{
    int inventory_id = AgentToolbeltList[agent_id];
    assert(AgentToolbeltList[agent_id] != NO_AGENT);
    assert(item_container_list->get(inventory_id) != NULL);

    item_container_list->destroy(inventory_id);
    AgentToolbeltList[agent_id] = NO_AGENT;
}


/*
    Nanite
*/
void create_agent_nanite(int agent_id, int client_id)
{
    printf("inplement: t_item::create_agent_nanite \n");
}

void delete_agent_nanite(int agent_id)
{
    printf("inplement: t_item::delete_agent_nanite \n");
}


void check_item_pickups()
{

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
        assert(AgentInventoryList[agent->id] != NO_AGENT);

        int inventory_id = AgentInventoryList[agent->id];
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
