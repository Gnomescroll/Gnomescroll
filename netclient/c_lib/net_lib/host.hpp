#pragma once


struct ENetHost;

void init_network();

extern ENetHost * enet_host; //the host

//extern ENetAddress address;
extern ENetHost * server_host;
extern ENetHost * client_host;

void init_net_server();
void init_net_client();



void client_connect_to(int a, int b, int c, int d, unsigned short port) ;

//iterate through client list
void dispatch_network_events();