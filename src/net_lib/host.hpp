/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

void init_network();
void teardown_network();

struct _ENetHost;

#if DC_SERVER
extern struct _ENetHost* server_host;
#endif
#if DC_CLIENT
extern struct _ENetHost* client_host;
#endif

#if DC_SERVER
namespace NetServer
{

void init_server(int a, int b, int c, int d, int port);
void dispatch_network_events() GNOMESCROLL_API;
void flush_to_net() GNOMESCROLL_API;

void kill_client(class NetPeer* peer, DisconnectType error_code);

}
#endif

#if DC_CLIENT
namespace NetClient
{

void init_net_client();
void shutdown_net_client();

void client_connect_to(int a, int b, int c, int d, int port);

void client_dispatch_network_events();
void flush_to_net() GNOMESCROLL_API;

}

#endif
