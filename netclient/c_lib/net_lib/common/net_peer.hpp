#pragma once

#include "net_lib_common.h"

#include "./sequencer.h"

#include <net_lib/common/packet_buffer.hpp>


#ifdef DC_CLIENT
#include <net_lib/client/pviz.h>
#endif

#include <net_lib/common/sequencer.h>
#include <net_lib/common/net_peer.hpp>


struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};


struct packet_sequence {
    int seq;
    int ack;
    int time;
};

struct packet_sequence2 {
    int seq;
    int received;
};

#include <net_lib/common/packet_buffer.hpp>

/*
struct net_message_list {
    class Net_message* net_message_array[1024];
    int net_message_array_index;
    int pending_bytes_out;
};
*/

/*
    NetMessage
    class Net_message_buffer* b;
    char* buff;
    int len;
    int reference_count;
*/



//class NetPeer {

//#include <sys/mman.h>

//int GLOBAL_ = 0;

class NetPeer
{
    private:
    public:
    int client_id;
    int connected;

    //remote server
    struct Socket* socket;

    uint32_t ip;
    uint16_t port;
    struct sockaddr_in address;

    //buffer
    int buff_n;
    char buff[1500]; //out buffer
    /*
        push packets ont net_message_array and flush from here
    */

    /*
        NetMessageList
    */
    class Net_message* unreliable_net_message_array[256];
    int unreliable_net_message_array_index;
    
    class Net_message* reliable_net_message_array[256];
    int reliable_net_message_array_index;

    int pending_bytes_out;

    void push_unreliable_packet(Net_message* np) ;
    void push_reliable_packet(Net_message* np);

    void flush_to_buffer(char* buff, int* index);
    void flush_to_net();

    /*
        TTL
    */

    unsigned int ttl;
    unsigned int ttl_max;
    unsigned int last_packet_time;

    /*
        packet sequencer
    */

    //x to y: x state
    struct packet_sequence packet_sequence_buffer[256];
    int packet_sequence_number;
    //x to y: y state
    struct packet_sequence2 seqbuff[256];
    int highest_packet_sequence_number;

    /*
        Init
    */

    void init() {}

    NetPeer() {
        for(int i=0; i< 256; i++) unreliable_net_message_array[i] = NULL;
        for(int i=0; i< 256; i++) reliable_net_message_array[i] = NULL;

        unreliable_net_message_array_index = 0;
        reliable_net_message_array_index = 0;

        pending_bytes_out = 0;
    }
};


#define TTL_MAX_DEFAULT 120


//class NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
//class NetPeer* create_raw_net_peer(struct sockaddr_in address);

void reset_NetPeer_buffer(class NetPeer* s);

struct Socket* create_socket(uint16_t port);

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port);
class NetPeer* create_net_peer_from_address(struct sockaddr_in address);
