#include "host.hpp"

#include <net_lib/enet/enet.h>

#include <net_lib/global.hpp>

//struct _ENetHost* enet_host; //the host
struct _ENetHost* server_host;
struct _ENetHost* client_host;

void init_network()
{
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return;
    }
    atexit (enet_deinitialize);
}

ENetAddress address;
//ENetHost * server_host = NULL;
//ENetHost * client_host = NULL;

//ENetHost * enet_host = NULL;

void init_net_server()
{
    
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */

    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = 8080;

    server_host = enet_host_create (& address /* the address to bind the server host to */, 
                                 32      /* allow up to 32 clients and/or outgoing connections */,
                                  4      /* allow up to 4 channels to be used*/,
                                  0      /* assume any amount of incoming bandwidth */,
                                  0      /* assume any amount of outgoing bandwidth */);
    if (server_host == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }
    //enet_host_destroy(server);
}

void init_net_client()
{
    client_host = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                4 /* allow up 4 channels*/,
                65536 /* 56K modem with 56 Kbps downstream bandwidth */,
                65536 /* 56K modem with 14 Kbps upstream bandwidth */);

    if (client_host == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

    //enet_host_destroy(client);
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

    address.port = 8011;
    address.host = htonl( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d );

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client_host, & address, 4, 0);    
    
    if (peer == NULL)
    {
       fprintf (stderr, "No available peers for initiating an ENet connection.\n");
       exit (EXIT_FAILURE);
    }
    
    ENetEvent event;
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (client_host, & event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Client connected with server \n");
        NetClient::Server.enet_peer = peer;
        peer -> data = (void*) &NetClient::Server.enet_peer;
        //NetClient::Server.connected = 0;
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (peer);
        NetClient::Server.enet_peer = NULL;
        NetClient::Server.connected = 0;

        puts ("client_connect_to: connection attempt failed \n");
    }

    printf("Client is connected to server \n");
    NetClient::Server.connected = 1;
}


//iterate through client list
void dispatch_network_events()
{
    ENetEvent event;
    
    /* Wait up to 5 milliseconds for an event. */

    #ifdef DC_CLIENT
        while (enet_host_service (client_host, & event, 5) > 0)
    #else
        while (enet_host_service (server_host, & event, 5) > 0)
    #endif
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

        //handle connect
        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);
            /* Store any relevant client information here. */
            //event.peer -> data = "Client information";
            event.peer -> data = NULL;
            break;

        //handle disconnect
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconected.\n", (char*) event.peer -> data);
            /* Reset the peer's client information. */
            event.peer -> data = NULL;
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u channel %u.\n",
                    event.packet -> dataLength,
                    //(char*) event.packet -> data,
                    //(event.peer -> data,
                    event.channelID);
            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            break;
           
        }
    }
}