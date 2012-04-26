#include "manager.hpp"

#include <net_lib/global.hpp>

#include <t_map/t_map.hpp>

#include <t_map/server/map_manager_class.hpp>
#include <t_map/server/map_chunk_history.hpp>

namespace t_map
{

static class Map_manager* map_manager_list[NetServer::HARD_MAX_CONNECTIONS];

class Terrain_map_history* map_history = NULL;

void t_map_manager_setup(int client_id)
{
    if(COMPRESSION_BUFFER == NULL) COMPRESSION_BUFFER = (char*) malloc(COMPRESSION_BUFFER_SIZE);
    /*
        When valgrind finds this... needs to be freed at end
    */

    if(map_manager_list[client_id] != NULL ) printf("FATAL ERROR: t_map_manager_setup \n");
    map_manager_list[client_id] = new Map_manager(client_id);
}

void t_map_manager_teardown(int client_id)
{
    if(map_manager_list[client_id] != NULL)
    {
        delete map_manager_list[client_id];
    }
    map_manager_list[client_id] = NULL;
}

/*
    Call this to set player position for map loading
*/
void t_map_manager_update_client_position(int client_id, float _x, float _y)
{

    //printf("updated client %i to %f %f \n", client_id, _x, _y);
    
    int x = _x;
    int y = _y;

    if(x<0) x = 0;
    if(x >= MAP_WIDTH) x = MAP_WIDTH - 1;

    if(y<0) y = 0;
    if(y >= MAP_HEIGHT) x = MAP_HEIGHT - 1;


    if(client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS)
    {
        printf("ERROR: t_map_manager_update, invalid client id \n");
        return;
    }

    if( map_manager_list[client_id] == NULL )
    {
        //printf("ERROR: t_map_manager_update, client %i map manager pointer is null! \n", client_id);
        return;
    }

    map_manager_list[client_id]->set_position(x,y);

}

void t_map_manager_update()
{
    //iterate through client ids
    for(int i=0; i < NetServer::HARD_MAX_CONNECTIONS; i++)
    {
        if(map_manager_list[i] == NULL) continue;
        map_manager_list[i]->update();
    }
}

    /*
        Use when agent has teleported
    */
void t_map_manager_force_update(int client_id)
{
    if(map_manager_list[client_id] == NULL) return;
    map_manager_list[client_id]->update();
    map_manager_list[client_id]->sort_que();
}

void t_map_send_map_chunks()
{
    for(int i=0; i < NetServer::HARD_MAX_CONNECTIONS; i++)
    {
        if(map_manager_list[i] == NULL) continue;
        map_manager_list[i]->dispatch_que();
    }
}

void t_map_sort_map_chunk_ques()
{
    for(int i=0; i < NetServer::HARD_MAX_CONNECTIONS; i++)
    {
        if(map_manager_list[i] == NULL) continue;
        map_manager_list[i]->sort_que();
    }
}

}
