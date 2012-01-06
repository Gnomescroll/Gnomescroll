#pragma once

#include "net_lib_common.h"

#include "./sequencer.h"

#ifdef DC_CLIENT
#include <net_lib/client/pviz.h>
#endif

#include <net_lib/common/sequencer.h>
#include <net_lib/common/net_peer.hpp>

#include <net_lib/common/packet_buffer.hpp>

#include <net_lib/common/python_channel.hpp>

struct Socket {
    uint32_t ip;
    uint16_t port;
    int socket;
    struct sockaddr_in address;
};

class NetMessageArray; //forward declaration

struct packet_sequence {
    class NetMessageArray* nma;   //pointer to buffer
    short read_index;  //starting index in buffer
    short messages_n;    //number of packets

    short seq;
    short ack;
    int time;

    //int retry_attempts; //can only use for reliable, ordered packet sequences
};

struct packet_sequence2 {
    short seq;
    short received;
};

/*
NetPeer
*/

#include <packet_buffer.hpp>

class NetPeer
{
    private:
    public:
    int client_id;
    int connected;

    //remote server
    //struct Socket* socket;

    uint32_t ip;
    uint16_t port;

    int pending_bytes_out;
    int pending_unreliable_bytes_out;
    int pending_reliable_bytes_out;

    int pending_reliable_packets_out;


    //void push_reliable_packet(Net_message* np);

    inline void flush_unreliable_to_buffer(char* buff, int* index);
    inline void flush_reliable_to_buffer(char* buff_, int* _index, struct packet_sequence* ps);
    inline void flush_python_to_buffer(char* buff_, int* _index, struct packet_sequence* ps);

    void flush_to_net();

    void ack_packet(struct packet_sequence* ps);

    //unreliable packets
    Net_message* unreliable_net_message_array[256];
    int unreliable_net_message_array_index;
    


    class NetMessageManager reliable_message_manager;
    class NetMessageManager unreliable_message_manager;
    class NetMessageManager python_message_manager;

    void push_unreliable_packet(class Net_message* nm);
    void push_reliable_message(class Net_message* nm);
    void push_python_message(class Net_message* nm);


    /*
        packet sequencer
    */

    //x to y: x state
    struct packet_sequence packet_sequence_buffer[256];
    int packet_sequence_number;
    //x to y: y state
    struct packet_sequence2 seqbuff[256];
    int highest_packet_sequence_number;

    void init() {}

    NetPeer();

};