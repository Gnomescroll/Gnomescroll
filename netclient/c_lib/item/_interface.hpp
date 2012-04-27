#pragma once



namespace item
{

void init();
void teardown();

}

/*
CLIENT
*/
#if DC_CLIENT

namespace item
{



}

#endif 



/*
SERVER
*/

#ifdef DC_SERVER

namespace item
{

void create_agent_inventory(int agent_id, int client_id);
void delete_agent_inventory(int agent_id);

void create_agent_toolbar(int agent_id, int client_id);
void delete_agent_toolbar(int agent_id);

void create_agent_nanite(int agent_id, int client_id);
void delete_agent_nanite(int agent_id);

void check_item_pickups();

void create_item(int inventory_id);
}
#endif


