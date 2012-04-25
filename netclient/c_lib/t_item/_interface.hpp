#pragma once



namespace t_item
{


extern class Free_item_list* free_item_list;
extern class ItemContainerList* item_container_list;
extern class ItemList* item_list;

void state_init();
void state_teardown();

void tick();

void check_item_pickups();

void create_free_item(int item_type, 
    float x, float y, float z,
 float vx, float vy, float vz);


}

/*
CLIENT
*/
#if DC_CLIENT

namespace t_item
{
    extern int player_inventory_id;
    extern int player_toolbelt_id;
    extern int player_nanite_id;

    extern class ItemContainer* player_inventory;
    extern class ItemContainer* player_toolbelt;
    extern class ItemContainer* player_nanite;


void draw();
//move item within inventory
//void move_item(int inventory_id, int slot1, int slot2);

//move item
void move_item(int inventory_id1, int inventory_id2, int slot1, int slot2);

}

#endif 



/*
SERVER
*/
#if DC_SERVER

namespace t_item
{

const int NO_AGENT = 0xffff;
extern int AgentInventoryList[256];
extern int AgentToolbeltList[256];
extern int AgentNaniteList[256];

void create_agent_inventory(int agent_id, int client_id);
void delete_agent_inventory(int agent_id);

void create_agent_toolbar(int agent_id, int client_id);
void delete_agent_toolbar(int agent_id);

void create_agent_nanite(int agent_id, int client_id);
void delete_agent_nanite(int agent_id);

void check_item_pickups();

void create_free_item(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz);

void create_item(int inventory_id);
}

#endif 
