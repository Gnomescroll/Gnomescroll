#include "export.hpp"

#include <net_lib/global.hpp>
#include <net_lib/host.hpp>

int _get_client_id() 
{
    #ifdef DC_CLIENT
        return NetClient::Server.client_id;
    #else
        return 0;
    #endif
}

int _check_connection_status()
{
    #ifdef DC_CLIENT
        return NetClient::Server.connected;
    #else
        return 0;
    #endif
}

int _check_connection_status(int client_id) 
{
    #ifdef DC_SERVER
        if(NetServer::pool[client_id] == NULL)
        {
            printf("_check_connection_status: client_id % is null\n", client_id);
            return 0;
        }
        return NetServer::pool[client_id]->connected;
    #else
        return _check_connection_status();
    #endif
}
