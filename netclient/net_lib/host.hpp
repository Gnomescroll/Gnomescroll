#pragma once

void init_network();

struct _ENetHost;

//extern struct _ENetHost* enet_host; //the host

//extern ENetAddress address;
extern struct _ENetHost* server_host;
extern struct _ENetHost* client_host;

void init_net_server();
void init_net_client();



void client_connect_to(int a, int b, int c, int d, unsigned short port) ;

//iterate through client list
void client_dispatch_network_events();
void server_dispatch_network_events();
void flush_to_net();