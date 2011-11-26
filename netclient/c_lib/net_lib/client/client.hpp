#pragma once


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include <net_lib/common/net_lib_common.h>
#include <net_lib/common/message_handler.h>

#include <net_lib/client/pviz.h>
#include <net_lib/common/net_peer.hpp>


/*
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
*/

//class Netpeer; //forward declaration

namespace NetClient {

class NetPeer NPserver;
struct Socket client_socket;
//int get_socket();

int header_size1();

void init_client();
class NetPeer* CLIENT_get_NP();

//void send_packet(char* buffer, int n);
//void send_packet2(); //adding sequence number and acks
//void send_packet3(char* buff, int n_size);

void attempt_connection_with_server();

void set_server(int a, int b, int c, int d, unsigned short port);
int validate_packet(char* buffer, int n, struct sockaddr_in*);
void process_incoming_packets();
void process_packet(char* buffer, int n);
//void process_outgoing_packets(); //does not exist

int poll_connection_timeout();
int decrement_ttl();


//void flush_outgoing_packets();

//extern char client_out_buff[1500];
//extern int client_out_buff_n; //buffer for writing packets out to;

//char client_out_buff[1500];
//int client_out_buff_n = 11; //header length;


	void reset_client_out_buffer();

	char* get_client_out_buffer();
	int* get_client_out_buffer_n();

}