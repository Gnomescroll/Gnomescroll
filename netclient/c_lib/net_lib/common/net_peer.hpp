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
    int seq;
    int ack;
    int time;

    class NetMessageArray* nma;   //pointer to buffer
    int read_index;  //starting index in buffer
    int messages_n;    //number of packets
};

struct packet_sequence2 {
    int seq;
    int received;
};

/*
NetPeer
*/

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

    Net_message* unreliable_net_message_array[256];
    int unreliable_net_message_array_index;
    

    int pending_bytes_out;
    int pending_unreliable_bytes_out;
    int pending_reliable_bytes_out;

    int pending_reliable_packets_out;

    void push_unreliable_packet(Net_message* np) ;
    //void push_reliable_packet(Net_message* np);

    inline void flush_unreliable_to_buffer(char* buff, int* index);
    inline void flush_reliable_to_buffer(char* buff_, int* _index, struct packet_sequence* ps);
    void flush_to_net();

    void ack_packet(struct packet_sequence* ps);
    void resend_packet(struct packet_sequence* ps);

    //reliable packets
    class NetMessageArray* rnma_insert; //reliable net message array, for inserting
    int rnma_insert_index;  //index for insertions

    class NetMessageArray* rnma_read;   //offset for reading off net packets
    int rnma_read_index;  //index for insertions
    int rnma_pending_messages;

    void push_reliable_packet(class Net_message* nm);

    Python_channel_in py_in;
    Python_channel_out py_out;

    void write_python_packet(char* buff, int bytes)
    {
        py_out.write_message(buff, bytes);

    }
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

    int received_since_last_send;

    NetPeer() {
        for(int i=0; i< 256; i++) unreliable_net_message_array[i] = NULL;
        unreliable_net_message_array_index = 0;

        //reliable message que
        rnma_insert = NetMessageArray::acquire();
        rnma_insert->reference_count = 1;
        rnma_insert_index = 0;
        rnma_read = rnma_insert;
        rnma_read_index = 0;
        rnma_pending_messages = 0;

        pending_bytes_out = 0;
        pending_reliable_packets_out = 0;

        pending_unreliable_bytes_out = 0;
        pending_reliable_bytes_out = 0;

        py_in.np = this;

        received_since_last_send = 0;
    }


};


#define TTL_MAX_DEFAULT 120

struct Socket* create_socket(uint16_t port);

class NetPeer* create_net_peer_by_remote_IP(int a, int b, int c, int d, unsigned short port);
class NetPeer* create_net_peer_from_address(struct sockaddr_in address);


    //reference
    /*
    inline void consume(NetMessageArray* nma, int consume_index, int num) 
    {
        int i;
        class Net_message* nm;
        for(i=0; i < num; i++)
        {
            nm = nma->net_message_array[consume_index];
            //do something

            nma->reference_count--;

            consume_index++;
            if(consume_index == NET_MESSAGE_ARRAY_SIZE)
            {
                NetMessageArray* _next = nma->next;
                if(nma->reference_count == 0) nma->retire(); //check 1
                nma = _next;
                consume_index=0;
            }
        }
        if(nma->reference_count == 0) nma->retire(); //check 2
    }
    */