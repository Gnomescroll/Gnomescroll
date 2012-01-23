#include "host.hpp"

#include <net_lib/enet/enet.h>

#include <net_lib/global.hpp>
#include <net_lib/export.hpp>
#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/message_handler.h>

#include <c_lib/game/packets.hpp>
#include <c_lib/state/packet_init.hpp>


const int DEFAULT_PORT = 4096;

struct _ENetHost* server_host;
struct _ENetHost* client_host;

void init_network()
{
    PacketInit::RegisterMessages();

    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return;
    }
    atexit (enet_deinitialize);
}

ENetAddress address;

void init_net_server(int a, int b, int c, int d, int port)
{
    
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */

    if(a==0 && b==0 && c==0 && d== 0)
    {
        address.host = ENET_HOST_ANY;
    } 
    else 
    {
        address.host = htonl( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d );
    }


    /* Bind the server to port 1234. */

    if(port == 0)
    {
        address.port = DEFAULT_PORT;
    }
    else
    {
        address.port = port;
    }

    server_host = enet_host_create (& address /* the address to bind the server host to */, 
                                 32      /* allow up to 32 clients and/or outgoing connections */,
                                  4      /* allow up to 4 channels to be used*/,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (server_host == NULL)
    {
        fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }
    //enet_host_destroy(server);
}

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

    if(NetClient::Server.enet_peer != NULL)
    {
        enet_peer_disconnect(NetClient::Server.enet_peer, 0);   //graceful shutdown
        enet_host_flush(client_host);   //flush packets
    }

    enet_host_destroy(client_host);
}

namespace NetClient
{

static void client_connect(ENetEvent* event)
{
    printf("Client connected with server \n");

    NetClient::Server.enet_peer = event->peer;
    event->peer -> data = (void*) &NetClient::Server;
    NetClient::Server.connected = 1;
 
    //dont send until client id received
    //client_connect_event(nc->client_id);
    

}

static void client_disconnect(ENetEvent* event)
{
    printf ("Client disconected from server\n");

    event->peer -> data = NULL;
    enet_peer_reset(event->peer); //TEST
    NetClient::Server.connected = 0;
    NetClient::Server.client_id = -1;

    client_disconnect_event(0);
}

}

void client_connect_to(int a, int b, int c, int d, unsigned short port) 
{
    printf("client attempting connection with server \n");

    ENetPeer *peer;

    if (client_host == NULL)
    {
        fprintf (stderr, "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

    if(port == 0)
    {
        address.port = DEFAULT_PORT;
    }
    else
    {
        address.port = port;
    }

    //use local host if 0,0,0,0
    if(a==0 && b==0 && c==0 && d== 0)
    {
        a=127;b=0;c=0;d=1;
    }

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
    ENetEvent event;
    
    /* Wait up to 5 milliseconds for an event. */

    int index = 0;

    int timeout = 5;
    while (enet_host_service (client_host, & event, timeout) > 0)
    {
        switch (event.type)
        {

        //ENET_EVENT_TYPE_NONE       = 0,  
        //ENET_EVENT_TYPE_CONNECT    = 1,  
        //ENET_EVENT_TYPE_DISCONNECT = 2,  
        //ENET_EVENT_TYPE_RECEIVE    = 3

        case ENET_EVENT_TYPE_NONE:
            printf("Nothing happened \n");
            break;

        case ENET_EVENT_TYPE_CONNECT:
            NetClient::client_connect(&event);
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            NetClient::client_disconnect(&event);
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
                    index= 0;
                    process_packet_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                case 1:
                    printf("server received channel 1 message \n");
                    index= 0;
                    process_large_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                case 2:
                    //printf("server received channel 2 message \n");
                    index= 0;
                    process_python_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        0
                        ); 
                    break;
                case 3:
                    printf("server received channel 3 message \n");
                    index= 0;
                    break;
            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            break;
        }
    }
}


namespace NetServer
{

static int client_id_offset = 1;

static void client_connect(ENetEvent* event)
{
    class NetPeer* nc = NULL;

    if(NetServer::number_of_clients == NetServer::HARD_MAX_CONNECTIONS)
    {
        printf("Cannot allow client connection: hard max connection reached \n");
        //send a disconnect reason packet
        //enet_peer_disconnect(event->peer, 0); //gracefull disconnect client
        enet_peer_reset(event->peer);   //force disconnect client, does not notify client
        return;
    }
    NetServer::number_of_clients++;

    int index = client_id_offset;
    client_id_offset++;

    for(int i=0; i<NetServer::HARD_MAX_CONNECTIONS ;i++)
    {
        index = (index+1) % NetServer::HARD_MAX_CONNECTIONS;
        if(NetServer::pool[index] != NULL) continue;
        nc = new NetPeer;
        nc->enet_peer = event->peer;
        nc->client_id = index;
        nc->connected = 1;
        NetServer::pool[index]= nc;
        event->peer->data = (NetPeer*) nc;
        break;    
    }

    printf ("client %i connected from %x:%u. %i clients connected\n", 
    index,
    event->peer -> address.host, 
    event->peer -> address.port, 
    NetServer::number_of_clients+1);
    //startup sequence

    /*
        Send client id, flush netclient, then flush enet packet que
    */

    SendClientId_StoC u;
    u.client_id = nc->client_id;
    u.sendToClient(nc->client_id);

    nc->flush_to_net();
    enet_host_flush(server_host);

    client_connect_event(nc->client_id);
}

static void client_disconnect(ENetEvent* event)
{
    NetServer::number_of_clients--;
    class NetPeer* nc = (NetPeer*) event->peer -> data;
    
    int client_id = nc->client_id;

    NetServer::pool[client_id] = NULL;

    client_disconnect_event(client_id);
    printf("Client %i disconnected, %i clients connected \n", client_id, NetServer::number_of_clients);

    enet_peer_reset(event->peer); //TEST
    event->peer -> data = NULL;

    delete nc;
    //printf ("%s disconected.\n", (char*) event->peer -> data);
    /* Reset the peer's client information. */
}

}

void server_dispatch_network_events()
{
    ENetEvent event;
    
    /* Wait up to 5 milliseconds for an event. */

    int index = 0;
    int timeout = 5;
    while (enet_host_service (server_host, &event, timeout) > 0)
    {
        switch (event.type)
        {

        //ENET_EVENT_TYPE_NONE       = 0,  
        //ENET_EVENT_TYPE_CONNECT    = 1,  
        //ENET_EVENT_TYPE_DISCONNECT = 2,  
        //ENET_EVENT_TYPE_RECEIVE    = 3

        case ENET_EVENT_TYPE_NONE:
            printf("Nothing happened \n");
            break;

        case ENET_EVENT_TYPE_CONNECT:
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
                    index= 0;
                    process_packet_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        ((class NetPeer*)event.peer->data)->client_id 
                        ); 
                    break;
                case 1:
                    printf("server received channel 1 message \n");
                    index= 0;
                    process_large_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        ((class NetPeer*)event.peer->data)->client_id 
                        ); 
                    break;
                case 2:
                    //printf("server received channel 2 message \n");
                    index= 0;
                    process_python_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength, 
                        ((class NetPeer*)event.peer->data)->client_id 
                        ); 
                    break;
                case 3:
                    printf("server received channel 4 message \n");
                    index= 0;
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
#ifdef DC_CLIENT
    NetClient::Server.flush_to_net();
#endif

#ifdef DC_SERVER
    for(int i=0; i<NetServer::HARD_MAX_CONNECTIONS ;i++)
    {
        if(NetServer::pool[i] == NULL) continue;
        NetServer::pool[i]->flush_to_net();
    }
#endif


}
