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

struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};

struct NetPeer {
    uint32_t ip;
    uint16_t port;
    struct sockaddr_in address;
};


struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
//port=0 to get any port
struct Socket* create_socket(uint32_t IP, uint16_t  port);

int send_packet(struct Socket* socket, struct NetPeer* p, char* packet_data, int packet_size);
void receive_packets(struct Socket* socket);


#endif
