#include "host.hpp"

#include <net_lib/enet/enet.h>

#include <net_lib/global.hpp>

#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/message_handler.h>

#include <agent/net_agent.hpp>
#include <state/packet_init.hpp>

#include <common/common.hpp>
#include <common/analytics/sessions.hpp>

#include <options/options.hpp>

#if DC_CLIENT
#include <state/client_state.hpp>
#endif

const int DEFAULT_PORT = 4096;

struct _ENetHost* server_host;
struct _ENetHost* client_host;

static bool enet_inited = false;

void init_network()
{
    init_message_handler();
    PacketInit::RegisterMessages();

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        enet_inited = false;
        return;
    }
    enet_inited = true;
}

void teardown_network()
{
    if (enet_inited) enet_deinitialize();
}

ENetAddress address;

#if DC_CLIENT

namespace NetClient
{

static void client_connect(ENetEvent* event);
static void client_disconnect(ENetEvent* event);
void client_dispatch_network_events();

void init_net_client()
{
    client_host = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                4 /* allow up 4 channels*/,
                0 /* 56K modem with 56 Kbps downstream bandwidth */,
                0 /* 56K modem with 14 Kbps upstream bandwidth */);

    if (client_host == NULL)
    {
        fprintf (stderr, "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

    //enet_host_destroy(client);
}

void shutdown_net_client()
{
    if (NetClient::Server.enet_peer != NULL)
    {
        enet_peer_disconnect(NetClient::Server.enet_peer, 1);   //graceful shutdown
        enet_host_flush(client_host);   //flush packets
        //client_dispatch_network_events();
    }
    NetClient::Server.enet_peer = NULL;

    if (client_host != NULL)
        enet_host_destroy(client_host);
    client_host = NULL;
}

static void client_connect(ENetEvent* event)
{
    GS_ASSERT(event != NULL);
    if (event == NULL) return;
    NetClient::Server.enet_peer = event->peer;
    event->peer -> data = (void*) &NetClient::Server;
    NetClient::Server.connected = 1;

    printf("Client connected with server \n");
    #if DC_CLIENT
    ClientState::on_connect();
    #endif
}

//client disconnect event
static void client_disconnect(ENetEvent* event)
{
    if ( event->data == 0)
    {
        printf("Client timeout from server\n");
    } 
    else
    {
        printf("Client was disconnected by server \n");

        if (event->data == 2) 
            printf("Server is full\n");
    }

    event->peer -> data = NULL;
    //enet_peer_reset(event->peer); //TEST
    
    NetClient::Server.connected = 0;
    NetClient::Server.client_id = -1;

    printf("Client disconnected from server\n");
    #if DC_CLIENT
    ClientState::on_disconnect();
    #endif
}

void client_connect_to(int a, int b, int c, int d, unsigned short port) 
{
    ENetPeer *peer;

    if (client_host == NULL)
    {
        fprintf (stderr, "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

    if (port == 0)
    {
        address.port = (Options::port) ? Options::port : DEFAULT_PORT;
    }
    else
    {
        address.port = port;
    }

    //use local host if 0,0,0,0
    if (a==0 && b==0 && c==0 && d== 0)
    {
        a=127;b=0;c=0;d=1;
    }
    printf("connecting to %i.%i.%i.%i on port %i \n", a,b,c,d,port);
    
    address.host = htonl( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d );

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client_host, & address, 4, 0);    
    
    if (peer == NULL)
    {
       fprintf (stderr, "No available peers for initiating an ENet connection.\n");
       exit (EXIT_FAILURE);
    }

}

//enet_peer_reset (peer);

void client_dispatch_network_events()
{
    if (client_host == NULL) return;
    
    ENetEvent event;
    
    /* Wait up to 5 milliseconds for an event. */

    unsigned int index = 0;

    int timeout = 1;
    while (enet_host_service (client_host, & event, timeout) > 0)
    {
        switch (event.type)
        {

        //ENET_EVENT_TYPE_NONE       = 0,  
        //ENET_EVENT_TYPE_CONNECT    = 1,  
        //ENET_EVENT_TYPE_DISCONNECT = 2,  
        //ENET_EVENT_TYPE_RECEIVE    = 3

        case ENET_EVENT_TYPE_NONE:
            printf("ENET_EVENT_TYPE_NONE: Nothing happened \n");
            break;

        case ENET_EVENT_TYPE_CONNECT:
            NetClient::client_connect(&event);

            //printf("Client connected to server \n");
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            NetClient::client_disconnect(&event);
            //printf("Client disconnected from server \n");
            break;

        case ENET_EVENT_TYPE_RECEIVE:
        /*
            printf ("A packet of length %u channel %u.\n",
                    event.packet -> dataLength,
                    //(char*) event.packet -> data,
                    //(event.peer -> data,
                    event.channelID);
        */
            switch(event.channelID)
            {
                case 0:
                    //printf("server received channel 0 message \n");
                    index = 0;
                    process_packet_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                case 1:
                    printf("server received channel 1 message \n");
                    index = 0;
                    process_large_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                case 3:
                    //printf("client received channel3 message of of size: %i \n", event.packet->dataLength);

                    index = 0;
                    //process_client_map_messages(char* buff, unsigned int* n, unsigned int max_n, int client_id);
                    process_client_map_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                default:
                    printf("server received unhandled channel %d message\n", event.channelID);
                    break;
            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            break;
        }
    }    
}

void flush_to_net()
{
    NetClient::Server.flush_to_net();
}


}
#endif

#if DC_SERVER

namespace NetServer
{

static void client_connect(ENetEvent* event);
static void client_disconnect(ENetEvent* event);

static int client_id_offset = -1;

void init_server(int a, int b, int c, int d, int port)
{
    
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */

    if (a==0 && b==0 && c==0 && d== 0)
    {
        address.host = ENET_HOST_ANY;
    } 
    else 
    {
        address.host = htonl( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d );
    }


    /* Bind the server to port 1234. */

    if (port == 0)
    {
        address.port = (Options::port) ? Options::port : DEFAULT_PORT;
    }
    else
    {
        address.port = port;
    }
    printf( "Starting server on %i.%i.%i.%i port %i \n", a, b, c, d, address.port);

    server_host = enet_host_create (& address /* the address to bind the server host to */, 
                                 32      /* allow up to 32 clients and/or outgoing connections */,
                                  4      /* allow up to 4 channels to be used*/,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (server_host == NULL)
    {
        fprintf (stderr, "An error occurred while trying to create ENet server host: check that no other server is running on port %i \n", address.port);
        exit (EXIT_FAILURE);
    }
    //enet_host_destroy(server);
}

//server
void dispatch_network_events()
{
    GS_ASSERT(server_host != NULL);
    if (server_host == NULL) return;
    
    ENetEvent event;
    
    /* Wait up to 5 milliseconds for an event. */

    unsigned int index = 0;
    int timeout = 1;
    int ret = 0;
    while (enet_host_service (server_host, &event, timeout) > 0)
    {
        switch (event.type)
        {

        //ENET_EVENT_TYPE_NONE       = 0,  
        //ENET_EVENT_TYPE_CONNECT    = 1,  
        //ENET_EVENT_TYPE_DISCONNECT = 2,  
        //ENET_EVENT_TYPE_RECEIVE    = 3

        case ENET_EVENT_TYPE_NONE:
            printf("ENET_EVENT_TYPE_NONE: Nothing happened \n");
            break;

        case ENET_EVENT_TYPE_CONNECT:
            printf("Client Connection Attempt \n");
            NetServer::client_connect(&event);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            NetServer::client_disconnect(&event);
            break;

        case ENET_EVENT_TYPE_RECEIVE:
        /*
            printf ("A packet of length %u channel %u.\n",
                    event.packet -> dataLength,
                    //(char*) event.packet -> data,
                    //(event.peer -> data,
                    event.channelID);
        */
            switch(event.channelID)
            {
                case 0:
                    //printf("server received channel 0 message \n");
                    index = 0;
                    GS_ASSERT(event.peer->data != NULL);
                    if (event.peer->data != NULL)
                    {
                        ret = process_packet_messages(
                            (char*) event.packet -> data, 
                            &index, 
                            event.packet->dataLength, 
                            ((class NetPeer*)event.peer->data)->client_id 
                            );
                    }
                    break;
                case 1:
                    printf("server received channel 1 message \n");
                    index= 0;
                    GS_ASSERT(event.peer->data != NULL);
                    if (event.peer->data != NULL)
                    {
                        process_large_messages(
                            (char*) event.packet -> data, 
                            &index, 
                            event.packet->dataLength, 
                            ((class NetPeer*)event.peer->data)->client_id 
                            );
                    }
                    break;
                case 2:
                    printf("server received channel 2 message \n");
                    break;
                case 3:
                    printf("server received channel 3 message \n");
                    index= 0;
                    break;
                default:
                    printf("server received unhandled channel %d message\n", event.channelID);
                    break;
            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            break;
           
        }
    }
    
    if (ret == -2 || ret == -3)
    {   // invalid data in packets, disconnect client
        GS_ASSERT(event.peer->data != NULL);
        int client_id = -1;
        if (event.peer->data != NULL)
            client_id = ((class NetPeer*)event.peer->data)->client_id;
        printf("Force disconnecting client %d for sending bad packets.\n", client_id);
        kill_client(event.peer);
    }
}

static void client_connect(ENetEvent* event)
{
    GS_ASSERT(event != NULL);
    if (event == NULL) return;
        
    if ((int)NetServer::number_of_clients >= NetServer::HARD_MAX_CONNECTIONS)
    {
        printf("Cannot allow client connection: hard max connection reached \n");
        //send a disconnect reason packet
        enet_peer_disconnect(event->peer, 2); //gracefull disconnect client
        //enet_peer_reset(event->peer);   //force disconnect client, does not notify client
        return;
    }
    NetServer::number_of_clients++;

    NetPeer* nc = NULL;
    NetPeerManager* npm = NULL;

    int client_id = client_id_offset;
    client_id_offset++;

    for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS; i++)
    {   // find free peer slot
        client_id = (client_id+1) % NetServer::HARD_MAX_CONNECTIONS;
        if (NetServer::staging_pool[client_id] != NULL || NetServer::pool[client_id] != NULL) continue;
        nc = new NetPeer;
        nc->enet_peer = event->peer;
        nc->client_id = client_id;
        nc->connected = 1;
        NetServer::staging_pool[client_id] = nc;
        GS_ASSERT(event->peer != NULL);
        if (event->peer != NULL)
            event->peer->data = (NetPeer*) nc;

        npm = new NetPeerManager();
        npm->init(client_id);
        NetServer::clients[client_id] = npm;
        break;
    }

    if (event->peer != NULL)
    {
        uint8_t address[4];
        address_from_uint32(event->peer->address.host, address);

        printf(
            "client %d connected from %d.%d.%d.%d:%d. %d clients connected\n", 
            client_id,
            address[0], address[1], address[2], address[3],
            event->peer->address.port, 
            NetServer::number_of_clients
        );
    
        Session* session = begin_session(event->peer->address.host, client_id);
        users->assign_session_to_user(session);
    }
    
    if (nc != NULL)
        nc->flush_to_net();
    //enet_host_flush(server_host);
    //client_dispatch_network_events();
}

static void client_disconnect(ENetEvent* event)
{
    NetServer::number_of_clients--;
    NetPeer* nc = (NetPeer*) event->peer -> data;
    GS_ASSERT(nc != NULL);
    if (nc == NULL) return;
    
    int client_id = nc->client_id;
    GS_ASSERT(client_id >= 0 && client_id < HARD_MAX_CONNECTIONS);
    if (client_id < 0 || client_id >= HARD_MAX_CONNECTIONS) return;
    
    //figure out client disconnection code
    if (event->data == 0)
    {
        printf("Client %i timed out \n", client_id);
    }
    else
    {
        if (event->data == 1)
        {
            printf("Client %i disconnected gracefully\n", client_id);
        }
        else
        {
            printf("Client %i disconnected with unknown code. code= %i !!!\n", client_id, event->data);
        }
    }

    NetPeerManager* npm = NetServer::clients[client_id];
    GS_ASSERT(npm != NULL);
    if (npm != NULL) npm->teardown();

    class User* user = users->get_user(event->peer->address.host);
    GS_ASSERT(user != NULL);
    if (user != NULL)
    {
        class Session* session = user->get_latest_session(npm->client_id);
        GS_ASSERT(session != NULL);
        if (session != NULL) end_session(session);
    }

    event->peer->data = NULL;

    GS_ASSERT(!(NetServer::pool[client_id] != NULL && NetServer::staging_pool[client_id] != NULL));
    NetServer::pool[client_id] = NULL;
    NetServer::staging_pool[client_id] = NULL;
    NetServer::agents[client_id] = NULL;
    NetServer::clients[client_id] = NULL;
    
    delete nc;
    delete npm;
}

void kill_client(ENetPeer* peer)
{
    GS_ASSERT(peer != NULL);
    if (peer == NULL) return;
    enet_peer_disconnect(peer, 1);
    
    // log it
    GS_ASSERT(peer->data != NULL);
    if (peer->data != NULL)
    {
        int client_id = ((class NetPeer*)peer->data)->client_id;
        NetServer::users->record_client_force_disconnect(client_id);
    }
}

void flush_to_net()
{

    for (int i=0; i<NetServer::HARD_MAX_CONNECTIONS ;i++)
    {
        if (NetServer::staging_pool[i] != NULL)
            NetServer::staging_pool[i]->flush_to_net();
        if (NetServer::pool[i] != NULL)
            NetServer::pool[i]->flush_to_net();
    }

}

}

#endif
