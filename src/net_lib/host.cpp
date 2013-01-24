#include "host.hpp"

#include <net_lib/enet/enet.h>
#include <net_lib/global.hpp>
#include <net_lib/common/net_peer.hpp>
#include <net_lib/common/message_handler.h>
#include <agent/net_agent.hpp>
#include <state/packet_init.hpp>
#include <common/common.hpp>
#include <options/options.hpp>

#if DC_CLIENT
# include <net_lib/client.hpp>
# include <state/client_state.hpp>
# include <hud/error.hpp>
#endif

#if DC_SERVER
# include <net_lib/server.hpp>
# include <common/analytics/sessions.hpp>
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
                8 /* allow up 4 channels*/,
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
        enet_peer_disconnect(NetClient::Server.enet_peer, DISCONNECT_GRACEFULLY);   //graceful shutdown
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
    ClientState::on_connect();
    Hud::unset_error_status(GS_ERROR_NOT_CONNECTED);
}

//client disconnect event
static void client_disconnect(ENetEvent* event)
{
    Hud::set_error_status(GS_ERROR_WAS_DISCONNECTED);

    if (event->data == DISCONNECT_TIMEOUT)
        Hud::set_server_disconnect_message("Client timeout from server");
    else
    if (event->data == DISCONNECT_GRACEFULLY)
        Hud::set_server_disconnect_message("Client disconnected from server");
    else
    if (event->data == DISCONNECT_FORCED)
        Hud::set_server_disconnect_message("Client was force disconnected by server");
    else
    if (event->data == DISCONNECT_FULL)
        Hud::set_server_disconnect_message("Could not connect to server: server full");
    else
    if (event->data == DISCONNECT_BAD_PACKET)
        Hud::set_server_disconnect_message("Client was disconnected for sending malformed data");
    else
    if (event->data == DISCONNECT_AUTH_TIMEOUT)
        Hud::set_server_disconnect_message("Client failed to authorize");
    else
    if (event->data == DISCONNECT_AUTH_EXPIRED)
        Hud::set_server_disconnect_message("Client authorization expired");
    else
    if (event->data == DISCONNECT_SERVER_ERROR)
        Hud::set_server_disconnect_message("Client was disconnected because of an error in the server");
    else
    if (event->data == DISCONNECT_LOGIN_ELSEWHERE)
        Hud::set_server_disconnect_message("Client was disconnected because it logged in as another client");
    else
    if (event->data == DISCONNECT_AUTH_LIMIT)
        Hud::set_server_disconnect_message("Client was disconnected because it had too many failed authorizations");
    else
        Hud::set_server_disconnect_message("Client disconnected from server");

    event->peer->data = NULL;
    
    NetClient::Server.disconnect_code = (DisconnectType)event->data;
    NetClient::Server.connected = 0;
    NetClient::Server.client_id = NULL_CLIENT;

    ClientState::on_disconnect();
}

void client_connect_to(int a, int b, int c, int d, unsigned short port) 
{
    ENetPeer *peer;

    if (client_host == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
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
        a = 127;
        b = 0;
        c = 0;
        d = 1;
    }
    printf("Connecting to %d.%d.%d.%d on port %d\n", a, b, c, d, port);
    
    address.host = htonl( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d );

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client_host, & address, 8, 0);    //channel count
    
    if (peer == NULL)
    {
       fprintf(stderr, "No available peers for initiating an ENet connection.\n");
       exit(EXIT_FAILURE);
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

    while (1)
    {
        int ret = enet_host_service(client_host, & event, timeout);

        if( ret == 0)
            break;
        if( ret < 0)
        {
            printf("client_dispatch_network_events: ERROR code %d \n", ret);
            break;
        }

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
                        event.packet->dataLength);
                    break;
                case 1:
                    printf("server received channel 1 message \n");
                    index = 0;
                    process_large_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength); 
                    break;
                case 3:
                    //printf("client received channel3 message of of size: %i \n", event.packet->dataLength);

                    index = 0;
                    //process_client_map_messages(char* buff, unsigned int* n, unsigned int max_n, ClientID client_id);
                    process_client_map_messages(
                        (char*) event.packet -> data, 
                        &index, 
                        event.packet->dataLength); 
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
static void client_disconnect(ENetPeer* peer, enet_uint32 data);

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
                                128      /* allow up to 32 clients and/or outgoing connections */,
                                8       /* allow up to 4 channels to be used*/,
                                0       /* assume any amount of incoming bandwidth */,
                                0       /* assume any amount of outgoing bandwidth */);
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
    int pret = 0;

    while (1)
    {
        int ret = enet_host_service(server_host, & event, timeout);

        if( ret == 0)
            break;
        if( ret < 0)
        {
            printf("NetServer dispatch_network_events: ERROR code %d \n", ret);
            break;
        }

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
            NetServer::client_disconnect(event.peer, event.data);
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
                        pret = process_packet_messages(
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

        if (pret != 0)
        {   // invalid data in packets, disconnect client
            GS_ASSERT(event.peer != NULL);
            if (event.peer != NULL)
            {
                GS_ASSERT(event.peer->data != NULL);
                if (event.peer->data != NULL)
                    ((class NetPeer*)event.peer->data)->disconnect_code = DISCONNECT_BAD_PACKET;
            }
        }

        pret = 0;
    }

    for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
    {
        NetPeer* peer = NetServer::staging_pool[i];
        if (peer == NULL)
            peer = NetServer::pool[i];
        if (peer != NULL && peer->should_disconnect())
            kill_client(peer, peer->disconnect_code);
    }
}

static void client_connect(ENetEvent* event)
{
    GS_ASSERT(event != NULL);
    if (event == NULL) return;
    GS_ASSERT(event->peer != NULL);

    // have to create it here so we can attach disconnect data if server full
    class NetPeer* nc = new NetPeer;
    if (event->peer != NULL)
        event->peer->data = nc;
    nc->enet_peer = event->peer;

    if ((int)NetServer::number_of_clients >= HARD_MAX_CONNECTIONS)
    {   // send a disconnect reason packet
        if (event->peer != NULL && event->peer->data != NULL)
            ((class NetPeer*)event->peer->data)->disconnect_code = DISCONNECT_FULL;
        enet_peer_disconnect(event->peer, DISCONNECT_FULL); //gracefull disconnect client
        return;
    }

    NetServer::number_of_clients++;
    NetPeerManager* npm = NULL;

    static int _client_id_offset = 0;
    for (int i=0; i<HARD_MAX_CONNECTIONS; i++)
    {   // find free peer slot
        int client_id = (_client_id_offset+i) % HARD_MAX_CONNECTIONS;
        if (NetServer::staging_pool[client_id] != NULL || NetServer::pool[client_id] != NULL) continue;
        _client_id_offset = (client_id + 1) % HARD_MAX_CONNECTIONS;
        nc->client_id = (ClientID)client_id;
        nc->connected = 1;
        NetServer::staging_pool[client_id] = nc;
        GS_ASSERT(event->peer != NULL);
        if (event->peer != NULL)
            event->peer->data = (NetPeer*) nc;

        npm = new NetPeerManager;
        NetServer::clients[client_id] = npm; // must be added to array before init
        npm->init(nc->client_id);
        break;
    }
    
    if (event->peer != NULL)
    {
        uint8_t address[4];
        address_from_uint32(event->peer->address.host, address);

        printf("Client %d connected from %d.%d.%d.%d:%d. %d clients connected\n", 
               npm->client_id,
               address[0], address[1], address[2], address[3],
               event->peer->address.port, 
               NetServer::number_of_clients);
    
        Session* session = begin_session(event->peer->address.host, npm->client_id);
        users->assign_session_to_user(session);
    }

    if (!Options::auth)
    {   // just connect the client
        // DONT MOVE THIS -- must be called here
        const char username_fmt[] = "debuguser%d";
        char* username = (char*)malloc(sizeof(username_fmt) * sizeof(char));
        sprintf(username, username_fmt, npm->client_id);
        NetServer::client_authorized(npm->client_id, (UserID)(NetServer::number_of_clients), utc_now()+3600-30, username);
        free(username);
    }
    
    if (nc != NULL)
        nc->flush_to_net();
}

static void client_disconnect(ENetPeer* peer, enet_uint32 data) 
{
    GS_ASSERT(peer != NULL);
    if (peer == NULL) return;
    
    NetPeer* nc = (NetPeer*) peer->data;

    ClientID client_id = NULL_CLIENT;
    if (nc != NULL && nc->client_id >= 0 && nc->client_id < HARD_MAX_CONNECTIONS)
    {
        NetServer::number_of_clients--;
        client_id = nc->client_id;
    }

    if (data == DISCONNECT_NONE)
    {
        // no disconnect code sent by client
        // assume the server intitiated the disconnect of the client
        // inspect peer data for disconnect reason 

        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_TIMEOUT)
            printf("Client %d timed out\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_FORCED)
            printf("Client %d force disconnected\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_FULL)
            printf("Client %d disconnected because server is full\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_BAD_PACKET)
            printf("Client %d disconnected because it sent bad packets\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_AUTH_TIMEOUT)
            printf("Client %d disconnected because failed to authorize in time\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_AUTH_EXPIRED)
            printf("Client %d disconnected because authorization expired\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_SERVER_ERROR)
            printf("Client %d disconnected because of a server error\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_LOGIN_ELSEWHERE)
            printf("Client %d disconnected because it logged in as another client\n", client_id);
        else
        if (((class NetPeer*)peer->data)->disconnect_code == DISCONNECT_AUTH_LIMIT)
            printf("Client %d disconnected because it had too many failed authorizations\n", client_id);
        else
        {
            GS_ASSERT(false);
        }
    }
    else
    {
        if (data == DISCONNECT_GRACEFULLY)
            printf("Client %d disconnected gracefully\n", client_id);
        else
        {
            GS_ASSERT(false);
            printf("Client %d disconnected with unknown code %d\n", client_id, data);
        }
    }
    
    peer->data = NULL;
    
    NetPeerManager* npm = NULL;
    if (client_id >= 0 && client_id < HARD_MAX_CONNECTIONS)
    {
        npm = NetServer::clients[client_id];
        GS_ASSERT(npm != NULL);
    }
    if (npm != NULL) npm->teardown();

    class User* user = users->get_user(peer->address.host);
    if (user != NULL)
    {
        class Session* session = user->get_latest_session(client_id);
        GS_ASSERT(session != NULL);
        if (session != NULL) end_session(session);
    }

    if (client_id >= 0 && client_id < HARD_MAX_CONNECTIONS)
    {
        GS_ASSERT(!(NetServer::pool[client_id] != NULL && NetServer::staging_pool[client_id] != NULL));
        NetServer::pool[client_id] = NULL;
        NetServer::staging_pool[client_id] = NULL;
        NetServer::agents[client_id] = NULL;
        NetServer::clients[client_id] = NULL;
    }
    
    if (nc != NULL) delete nc;
    if (npm != NULL) delete npm;
}

void kill_client(class NetPeer* peer, DisconnectType error_code)
{
    GS_ASSERT(peer != NULL);
    if (peer == NULL) return;
    peer->disconnect_code = error_code;
    GS_ASSERT(peer->enet_peer != NULL);
    if (peer->enet_peer != NULL)
        enet_peer_disconnect(peer->enet_peer, error_code);
    
    // log it
    ClientID client_id = peer->client_id;
    NetServer::users->record_client_force_disconnect(client_id);
}

void flush_to_net()
{
    for (int i=0; i<HARD_MAX_CONNECTIONS ;i++)
    {
        if (NetServer::staging_pool[i] != NULL)
            NetServer::staging_pool[i]->flush_to_net();
        if (NetServer::pool[i] != NULL)
            NetServer::pool[i]->flush_to_net();
    }
}

}

#endif
