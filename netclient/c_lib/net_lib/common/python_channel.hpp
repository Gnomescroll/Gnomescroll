#pragma once

/*
Two aspects: messages out, messages in


*/

//#include <net_lib/common/packet_buffer.cpp>
//#include <c_lib/template/object_pool.hpp>

/*
Net_message_buffer {
    private:
    public:
    int reference_count;
    char buffer[NET_MESSAGE_BUFFER_SIZE];
    Net_message_buffer* next;

    Net_message_buffer() { reference_count = 0; }
};
*/
//static const int NET_MESSAGE_BUFFER_SIZE = 4096;

//python channel


#include <iostream>
#include <string>

//using namespace std;

//void * memcpy ( void * destination, const void * source, size_t num );
//size_t copy ( char* s, size_t n, size_t pos = 0) const;
//string& erase ( size_t pos = 0, size_t n = npos );

Class Python_channel_out {
    public:

    int pending_bytes_out;  //pending bytes out
    std::string str;

    int sequence_number;

    void write_message(char* buff, int n)
    {
        pending_bytes_out += n+2
        
        char t[2];
        int n1=0;
        PACK_uint16_t(n+2, t, &n1);

        str.reserve(n+2+str.size())
        str.append(t, 2);
        str.append(buff, n);
    }

    Net_message* serialize_to_packet(int max_n)
    {
        int bytes = max_n > pending_bytes_out ? max_n : pending_bytes_out;
        Net_message* nm = Net_message::acquire_reliable(bytes+2);   //need 2 bytes for sequence prefix
        //pack sequence number
        int n1 =0;
        PACK_uint16_t(sequence_number, nm->buffer, &n1);
        sequence_number++;
        //pack data
        str.copy(nm->buffer+2, bytes, 0); //copy bytes
        //clean string
        str.erase(0, bytes);            //erase bytes
        pending_bytes_out -= bytes
        return nm;
    }

    Python_channel_out()
    {
        pending_bytes_out = 0;
        sequence_number = 0;
    }
}

/*
Need a list of sequences, need to determine 
*/

Class Python_channel_in_message {
        
    public:
        int sequence;
        int bytes;
        std::string* str;

    Python_channel_in_message(int seq)
    {
        //sequence = NULL;
        sequence = seq;
        bytes = NULL;
        str = NULL;
    }

    Python_channel_in_message(int seq, std::str _str)
    {
        //sequence = NULL;
        sequence = seq;
        str = _str;
        bytes = _str.size();
    }

    ~Python_channel_in_message {}

};

#include <iostream>
#include <deque>

//template <Python_channel_in_message> class pcim_deque;

//vector<int>::iterator myIntVectorIterator;

deque<Python_channel_in_message> pcim_deque;
deque<Python_channel_in_message>::iterator pcim_dequeIterator;

Class Python_channel_in {
    public:
    //int pending_bytes_out;  //pending bytes out
    //std::string str;

    pcim_deque deq;

    int len;
    int bytes_received;
    int bytes_next_message;

    int min_sequence;
    int 
    int max_sequence;


    void receive_message(char* buff, int n, int sequence)
    {
        while(sequence > max_sequence) {
            Python_channel_in_message tmp (max_sequence);
            deq = push_back(tmp)
            max_sequence++;
        }

        while(max_sequence != sequence) {
            
            
            sequence++;

        } 
    }

    Net_message* serialize_to_packet(int max_n)
    {

    }

    Python_channel_in()
    {
        len = 0;
        bytes_received = 0;
        bytes_next_message = 0;
    }
}