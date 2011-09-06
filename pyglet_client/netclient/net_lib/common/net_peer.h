#ifndef net_lib_common_net_peer_h
#define net_lib_common_net_peer_h

#include "net_lib_common.h"

#include "sequencer.h"

struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};


struct NetPeer {
    int client_id;
    int connected;

    //remote server
    int socket;
    uint32_t ip;
    uint16_t port;
    struct sockaddr_in address;

    //ttl
    unsigned int ttl;
    unsigned int ttl_max;
    ///packet sequencer

    //x to y: x state
    struct packet_sequence packet_sequence_buffer[64];
    int packet_sequence_number;
    //x to y: y state
    struct packet_sequence2 seqbuff[64];
    int highest_packet_sequence_number;
};

#define TTL_MAX_DEFAULT 150


//struct NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
//struct NetPeer* create_raw_net_peer(struct sockaddr_in address);

struct Socket* create_socket(uint16_t port);

struct NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port,unsigned short local_port);
struct NetPeer* create_net_peer_from_address(struct sockaddr_in address);

#endif
