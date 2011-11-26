#pragma once


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include <net_lib/common/net_lib_common.h>
#include <net_lib/common/message_handler.h>

#include <net_lib/client/pviz.h>
#include <net_lib/common/net_peer.hpp>

namespace NetClient {

class NetPeer NPserver;
struct Socket client_socket;

int header_size1();

void init_client();
class NetPeer* CLIENT_get_NP();

void attempt_connection_with_server();

void set_server(int a, int b, int c, int d, unsigned short port);
int validate_packet(char* buffer, int n, struct sockaddr_in*);
void process_incoming_packets();
void process_packet(char* buffer, int n);

int poll_connection_timeout();
int decrement_ttl();

}