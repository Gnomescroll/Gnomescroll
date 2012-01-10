#pragma once

//#include "./sequencer.h"
//#include <net_lib/client/pviz.h>
//#include <net_lib/common/sequencer.h>
//#include <net_lib/common/python_channel.hpp>

#include <net_lib/common/packet_buffer.hpp>

#include <net_lib/enet/enet.h>

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


//struct ENetPeer;

class NetPeer
{
    private:

    public:
    int client_id;
    int connected;
    
    ENetPeer *  enet_peer;

    //uint32_t ip;
    //uint16_t port;

    void flush_to_net();

    class NetMessageManager reliable_message_manager;
    class NetMessageManager unreliable_message_manager;
    class NetMessageManager python_message_manager;

/*
    void push_unreliable_message(class Net_message* nm) __attribute__((always_inline));
    void push_reliable_message(class Net_message* nm) __attribute__((always_inline));
    void push_python_message(class Net_message* nm) __attribute__((always_inline));
*/

    void push_unreliable_message(class Net_message* nm);
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