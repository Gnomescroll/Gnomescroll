#pragma once


struct ENetHost;

void init_network();

extern ENetHost * enet_host; //the host

//extern ENetAddress address;
extern ENetHost * server_host;
extern ENetHost * client_host;

void init_server();
void init_client();



//iterate through client list
void dispatch_network_events();