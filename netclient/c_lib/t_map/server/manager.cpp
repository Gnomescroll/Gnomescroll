#include "manager.hpp"

#include <net_lib/global.hpp>

#include <c_lib/t_map/server/manager_class.hpp>
#include <c_lib/t_map/t_map.hpp>

namespace t_map
{

static class Map_manager* map_manager_list[NetServer::HARD_MAX_CONNECTIONS];

void t_map_manager_setup(int client_id)
{
    if(map_manager_list[client_id] != NULL ) printf("FATAL ERROR: t_map_manager_setup \n");
    map_manager_list[client_id] = new Map_manager();
}

void t_map_manager_teardown(int client_id)
{
    if(map_manager_list[client_id] != NULL)
    {
        delete map_manager_list[client_id];
    }
    map_manager_list[client_id] = NULL;
}


void t_map_manager_update(int client_id, int x, int y)
{
    if(x<0) x = 0;
    if(x >= MAP_WIDTH) x = MAP_WIDTH - 1;

    if(y<0) y = 0;
    if(y >= MAP_HEIGHT) x = MAP_HEIGHT - 1;
   
    x = x / 16;
    y = y / 16;

    if(client_id < 0 || client_id >= NetServer::HARD_MAX_CONNECTIONS)
    {
        printf("ERROR: t_map_manager_update, invalid client id \n");
        return;
    }

    if( map_manager_list[client_id] == NULL )
    {
        printf("ERROR: t_map_manager_update, client %i map manager pointer is null! \n", client_id);
        return;
    }

    map_manager_list[client_id]->update(x,y);

}

}