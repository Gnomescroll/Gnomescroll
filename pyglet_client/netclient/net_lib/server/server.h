#ifndef net_lib_server_h
#define net_lib_server_h

    #define PLATFORM_WINDOWS  1
    #define PLATFORM_MAC      2
    #define PLATFORM_UNIX     3

    #if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
    #elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
    #else
    #define PLATFORM PLATFORM_UNIX
    #endif

    #if PLATFORM == PLATFORM_WINDOWS

        #include <winsock2.h>

    #elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <fcntl.h>

    #endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>

#include <sys/types.h> //?
#include <sys/socket.h> //?

#include "../net_lib_common.h"

#include "../net_packets.h"

#include "../client/sequence_numbers.h"

struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};

#define TTL_MAX 150
//5 seconds timeout?

struct NetPeer {
    unsigned short id;
    uint32_t ip;
    uint16_t port;
    uint16_t client_id;
    struct sockaddr_in address;

    unsigned int ttl;

    struct Pseq2 sq2;
};

#define HARD_MAX_CONNECTIONS 1024
struct ConnectionPool {
    int n_connections;
    struct NetPeer* connection[HARD_MAX_CONNECTIONS];

    struct Socket socket;
};

struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
struct NetPeer* create_raw_net_peer(struct sockaddr_in address);
//port=0 to get any port
struct Socket* create_socket(uint32_t IP, uint16_t  port);

int send_packet(struct Socket* socket, struct NetPeer* p, unsigned char* packet_data, int packet_size);
void broad_cast_packet(); //every 30 ms

void receive_packets(struct Socket* socket);

void init_server(unsigned short port);
inline int error_check_packet(unsigned char* data, int n);
void process_packets();

void decrement_ttl();
#endif
