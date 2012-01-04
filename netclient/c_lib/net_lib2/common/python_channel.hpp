#pragma once



//#include <net_lib/common/packet_buffer.cpp>
//#include <c_lib/template/object_pool.hpp>

//void * memcpy ( void * destination, const void * source, size_t num );

#include <c_lib/template/char_buffer.hpp>
#include <net_lib/export.hpp>

class Python_channel_out {
    public:

    Fifo_char_buffer fcb;

    int sequence_number;
    int pending_bytes_out;

    void write_message(char* buff, int n);
    Net_message* serialize_to_packet(int max_n);

    Python_channel_out()
    {
        pending_bytes_out = 0;
        sequence_number = 0;
    }
};

/*
Need a list of sequences, need to determine 
*/

class Channel_message {
        
    public:
        int length;
        char* buffer;
        //dont need sequence number explicitly
    Channel_message() {
        buffer = NULL;
        length = 0;
        //sequence = -1;
    }
};


static const int SEQUENCE_BUFFER_SIZE = 32;

class Sequence_buffer_element {
    public:
        Sequence_buffer_element* next;
        Channel_message cm[SEQUENCE_BUFFER_SIZE];

    Sequence_buffer_element() 
    {
        next = NULL;
        for(int i=0; i<SEQUENCE_BUFFER_SIZE; i++) cm[i].buffer = NULL;
    }
};

//static int _total_packets = 0;

class Python_channel_in {
    public:
    Sequence_buffer_element* read_sb;
    int read_index;
    int lowest_sequence;
    int size;

    NetPeer* np;

    Python_channel_in()
    {
        lowest_sequence = 0;
        read_index = 0;
        size = 0;
        need = 0;
        read_sb = new Sequence_buffer_element;
    }

    //handling
    Fifo_char_buffer fcb;
    int need;

    void insert(char* buff, int length, int sequence);
    void pop(char* buff, int length);
    void process();

};

//moved to hpp