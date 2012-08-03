#pragma once

void init_network();
void teardown_network();

struct _ENetHost;

//extern struct _ENetHost* enet_host; //the host

//extern ENetAddress address;
extern struct _ENetHost* server_host;
extern struct _ENetHost* client_host;

#if DC_SERVER
namespace NetServer
{

void init_server(int a, int b, int c, int d, int port);
void dispatch_network_events() GNOMESCROLL_API;
void flush_to_net() GNOMESCROLL_API;

void kill_client(ENetPeer* peer);

}
#endif

#if DC_CLIENT
namespace NetClient
{

void init_net_client();
void shutdown_net_client();

void client_connect_to(char* ip, unsigned short port);
void client_connect_to(int a, int b, int c, int d, unsigned short port);

void client_dispatch_network_events();
void flush_to_net() GNOMESCROLL_API;

}

#endif
