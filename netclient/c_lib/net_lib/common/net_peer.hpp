#pragma once

#include "net_lib_common.h"

#include "./sequencer.h"

#include <net_lib/common/packet_buffer.hpp>

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

class Net_message_list {
    private:
    public:
    /*
        Make variable length later or ignore
    */
    class Net_message* unreliable_net_message_array[256];
    int unreliable_net_message_array_index;
    
    class Net_message* reliable_net_message_array[256];
    int reliable_net_message_array_index;

    int pending_bytes_out;
    //char buff[1500]; //out buffer, write unreliable packets directly to buff
    //int buff_n;

    Net_message_list() {
        for(int i=0; i< 256; i++) unreliable_net_message_array[i] = NULL;
        for(int i=0; i< 256; i++) reliable_net_message_array[i] = NULL;

        unreliable_net_message_array_index = 0;
        reliable_net_message_array_index = 0;

        pending_bytes_out = 0;
    }

    /*
        Use arrays/pointers/pool later for packets, to remove limits
    */
    void push_unreliable_packet(Net_message* np) {
        unreliable_net_message_array[unreliable_net_message_array_index] = np;
        unreliable_net_message_array_index++;
        pending_bytes_out += np->len;
        if(unreliable_net_message_array_index > 256) printf("Net_message_list Push_unreliable_packet overflow 1\n");   //debug
    }

    void push_reliable_packet(Net_message* np) {
        reliable_net_message_array[reliable_net_message_array_index] = np;
        reliable_net_message_array_index++;
        pending_bytes_out += np->len;
        if(reliable_net_message_array_index > 256) printf("Net_message_list Push_reliable_packet overflow 2\n");     //debug
    }
    //void * memcpy ( void * destination, const void * source, size_t num );
    void flush_to_buffer(char* buff, int* index) {
        if(pending_bytes_out > 1500) printf("Net_message_list Error: too much data in packet buffer");
        Net_message* np;
        char* offset = *index + buff;
        for(int i=0; i< unreliable_net_message_array_index; i++)
        {
            np = unreliable_net_message_array[i];
            memcpy(offset, np->buff, np->len);
            offset += np->len;
            np->decrement_unreliable();
        }     
        for(int i=0; i< reliable_net_message_array_index; i++)
        {
            np = unreliable_net_message_array[i];
            memcpy(offset, np->buff, np->len);
            offset += np->len;
        }
        *index = buff - offset;

        pending_bytes_out = 0;
        unreliable_net_message_array_index = 0;
        reliable_net_message_array_index = 0;
    }
};

//class NetPeer {
class NetPeer: public Net_message_list 
{
    private:
    public:
    int client_id;
    int connected;

    //remote server
    int socket;
    uint32_t ip;
    uint16_t port;
    struct sockaddr_in address;

    //buffer
    int buff_n;
    unsigned char buff[1500]; //out buffer
    /*
        push packets ont net_message_array and flush from here
    */

    //ttl
    unsigned int ttl;
    unsigned int ttl_max;
    unsigned int last_packet_time;
    ///packet sequencer

    //x to y: x state
    struct packet_sequence packet_sequence_buffer[256];
    int packet_sequence_number;
    //x to y: y state
    struct packet_sequence2 seqbuff[256];
    int highest_packet_sequence_number;

    //struct net_message out_messages[1024];
    // message pool for sent messages
};

#define TTL_MAX_DEFAULT 120


//class NetPeer* create_net_peer(int a, int b, int c, int d, unsigned short port);
//class NetPeer* create_raw_net_peer(struct sockaddr_in address);

void reset_NetPeer_buffer(class NetPeer* s);

struct Socket* create_socket(uint16_t port);

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port);
class NetPeer* create_net_peer_from_address(struct sockaddr_in address);
