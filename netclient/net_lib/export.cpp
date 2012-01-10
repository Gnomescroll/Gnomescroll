#include "export.hpp"

#include <net_lib/global.hpp>
#include <net_lib/host.hpp>

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
        t_count++;
        printf("py_out: packet %i, size= %i \n", t_count, length);
        
        if(length > 256)
        {
            printf("Error: Client to Server python packet would exceed 512 bytes\n");
            return;
        }

        Net_message* nm = Net_message::acquire( length + 2 );
        int n1 = 0;
        //PACK_uint16_t(length, nm->buff, &n1);    //length
        pack_u16(&length, nm->buff, &n1, true);
/*
        n1 = 0;
        int len2;
        pack_u16(&len2, nm->buff, &n1, true);
        printf("len2= %i \n", len2);
*/
        memcpy(nm->buff+2, message, length);

        NetClient::Server.push_python_message(nm);
    #endif

    #ifdef DC_SERVER

        if(NetServer::pool[client_id] == NULL)
        {
            printf("send_python_net_message: client_id % is null\n", client_id);
            return;
        }

        int n1 =0;
        if(length > 256)
        {
            printf("send_python_net_message: sending large %i byte python packet to client %i \n", length, client_id);
            ENetPacket* python_p = enet_packet_create(NULL, length+2, ENET_PACKET_FLAG_RELIABLE);
            PACK_uint16_t(length, (char*)python_p->data, &n1);    //length
            memcpy( ((char*)python_p->data)+2, message, length);
            enet_peer_send (NetServer::pool[client_id]->enet_peer, 2, python_p);
            return;
        }

        static int t_count = 0;
        t_count++;
        printf("py_out: packet %i, size= %i \n", t_count, length);
        
        Net_message* nm = Net_message::acquire( length + 2 );

        PACK_uint16_t(length, nm->buff, &n1);    //length
        memcpy(nm->buff+2, message, length);
        NetServer::pool[client_id]->push_python_message(nm);
        
        //printf("send_python_net_message: export.cpp, need to create a net message packet... \n");
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