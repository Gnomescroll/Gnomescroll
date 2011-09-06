#ifndef net_lib_client2_h
#define net_lib_client2_h


/*
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
*/

#include "../common/net_lib_common.h"
#include "sequencer.h"
#include "netpeer.h"

struct NetClient {
    uint16_t client_id;
    int ttl; //for connection
    int connected;

    int socket;

    struct sockaddr_in local_address;
    uint16_t local_port;

    struct sockaddr_in server_address;
    uint32_t server_ip;
    uint16_t server_port;
};


void init_client();
struct NetClient* CLIENT_get_NC();

void send_packet(unsigned char* buffer, int n);
void send_packet2(); //adding sequence number and acks

void attempt_connection_with_server();

void set_server(int a, int b, int c, int d, unsigned short port);
int validate_packet(unsigned char* buffer, int n, struct sockaddr_in*);
void process_incoming_packets();
void process_packet(unsigned char* buffer, int n);
void process_outgoing_packets();

#endif
