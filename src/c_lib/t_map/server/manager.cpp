#include "manager.hpp"

#include <net_lib/global.hpp>

#include <t_map/t_map.hpp>

#include <t_map/server/manager_class.hpp>
#include <t_map/server/subscription_list.hpp>

namespace t_map
{

static class Map_manager* map_manager_list[HARD_MAX_CONNECTIONS];

class Terrain_map_subscription* map_history = NULL;

void t_map_manager_setup(ClientID client_id)
{
    if (COMPRESSION_BUFFER == NULL) COMPRESSION_BUFFER = (char*) malloc(COMPRESSION_BUFFER_SIZE);
    GS_ASSERT(map_manager_list[client_id] == NULL);
    map_manager_list[client_id] = new Map_manager(client_id);
}

void t_map_manager_teardown(ClientID client_id)
{
    if(map_manager_list[client_id] != NULL)
        delete map_manager_list[client_id];
    map_manager_list[client_id] = NULL;
}

/*
    Call this to set player position for map loading
*/
void t_map_manager_update_client_position(ClientID client_id, float x, float y)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;

    GS_ASSERT(map_manager_list[client_id] != NULL);
    if (map_manager_list[client_id] == NULL) return;


    GS_ASSERT(x >= 0 && x <= 512 && y >= 0 && y <= 512);
    x = translate_point(x); //box the position
    y = translate_point(y);

    map_manager_list[client_id]->set_position(x,y);
    map_manager_list[client_id]->update();
}

void t_map_manager_update()
{
    //iterate through client ids
    for(int i=0; i < HARD_MAX_CONNECTIONS; i++)
        if(map_manager_list[i] != NULL)
            map_manager_list[i]->update();
}

    /*
        Use when agent has teleported
    */
/*
void t_map_manager_force_update(ClientID client_id)
{
    if(map_manager_list[client_id] == NULL) return;
    map_manager_list[client_id]->update();
    map_manager_list[client_id]->sort_que();
}
*/

void t_map_send_map_chunks()
{
    for(int i=0; i < HARD_MAX_CONNECTIONS; i++)
        if(map_manager_list[i] != NULL)
            map_manager_list[i]->dispatch_que();
}

/*
void t_map_sort_map_chunk_ques()
{
    for(int i=0; i < HARD_MAX_CONNECTIONS; i++)
    {
        if(map_manager_list[i] == NULL) continue;
        map_manager_list[i]->sort_que();
    }
}
*/
}   // t_map
