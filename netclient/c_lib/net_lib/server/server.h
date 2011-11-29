#pragma once

#include <errno.h>

#include <sys/types.h> //?
//#include <sys/socket.h> //?

#include <net_lib/common/net_lib_common.h>
#include <net_lib/common/sequencer.h>
#include <net_lib/common/net_peer.hpp>

#include <net_lib/common/message_handler.h>

namespace NetServer
{
    const static int HARD_MAX_CONNECTIONS  = 1024;
}

struct ConnectionPool {
    int n_connections;
    class NetPeer* connection[HARD_MAX_CONNECTIONS];
};

namespace NetServer
{
    struct ConnectionPool pool;
    struct Socket server_socket;
}

namespace NetServer
{
    void init_server(unsigned short port);

    void receive_packets(struct Socket* socket);

    void process_packets();

    void poll_connection_timeout();
    void check_pool_for_dropped_packets();
}

//void flush_packets(); //all messages to all agents, out
//void push_message(int client_id, char* buffer, int n_bytes); //que a message for client
//void push_broadcast_message(char* buffer, int n_bytes);  //que a message for broadcast to all clients