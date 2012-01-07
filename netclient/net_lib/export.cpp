#include "export.hpp"

#include <net_lib/global.hpp>

//typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);
PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL = NULL;

void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
{  
    PY_MESSAGE_CALLBACK_GLOBAL = pt;
}

PY_CLIENT_EVENT_CALLBACK PY_CLIENT_EVENT_CALLBACK_GLOBAL = NULL;

void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt) 
{
    PY_CLIENT_EVENT_CALLBACK_GLOBAL = pt;
}


void client_connect_event(int client_id)
{
    if(PY_CLIENT_EVENT_CALLBACK_GLOBAL == NULL)
    {
        printf("client_connect_event Error: PY_CLIENT_EVENT_CALLBACK is null \n");
        return;
    }
    PY_CLIENT_EVENT_CALLBACK_GLOBAL(client_id, 0);
}

void client_disconnect_event(int client_id)
{
    if(PY_CLIENT_EVENT_CALLBACK_GLOBAL == NULL)
    {
        printf("client_disconnect_event Error: PY_CLIENT_EVENT_CALLBACK is null \n");
        return;
    }
    PY_CLIENT_EVENT_CALLBACK_GLOBAL(client_id, 1);
}

void send_python_net_message(char* message, int length, int client_id)
{
    #ifdef DC_CLIENT
        static int t_count = 0;
        //printf("py_out: packet %i \n", t_count);
        //NetClient::Server.write_python_packet(message, length);
        t_count++;
    #endif

    #ifdef DC_SERVER
        if(NetServer::pool[client_id] == NULL)
        {
            printf("send_python_net_message: client_id % is null\n", client_id);
            return;
        }
        NetServer::pool[client_id]->write_python_packet(message, length);
    #endif    

}

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
        return 0;
    #endif
}