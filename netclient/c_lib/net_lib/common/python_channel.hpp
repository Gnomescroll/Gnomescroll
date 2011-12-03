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


//#include <iostream>
//#include <string>

//using namespace std;

//void * memcpy ( void * destination, const void * source, size_t num );
//size_t copy ( char* s, size_t n, size_t pos = 0) const;
//string& erase ( size_t pos = 0, size_t n = npos );

#include <c_lib/template/char_buffer.hpp>



class Python_channel_out {
    public:

    Fifo_char_buffer fcb;

    int sequence_number;
    int pending_bytes_out;

    int size() { return fcb.size; }

    void write_message(char* buff, int n)
    {

        char t[2]; int n1=0; PACK_uint16_t(n+2, t, &n1);
        fcb.write(t,2);
        fcb.write(buff,n);

        pending_bytes_out += n+2;
        /*
        pending_bytes_out += n+2;
        
        char t[2];
        int n1=0;
        PACK_uint16_t(n+2, t, &n1);

        str.reserve(n+2+str.size());
        str.append(t, 2);
        str.append(buff, n);
        */
    }

    Net_message* serialize_to_packet(int max_n)
    {
        int bytes = max_n > pending_bytes_out ? max_n : pending_bytes_out;
        Net_message* nm = Net_message::acquire_reliable(bytes+2);   //need 2 bytes for sequence prefix
        //pack sequence number
        char t[2]; int n1 =0;
        PACK_uint16_t(sequence_number, nm->buff, &n1);
        fcb.write(t, 2);
        sequence_number++;
        //pack data
        fcb.write(nm->buff+2, bytes);
        pending_bytes_out -= bytes;
        return nm;
    }

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
        int size;
        char* buffer;
        //dont need sequence number explicitly
    Channel_message() {
        buffer = NULL;
        size = 0;
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

class Sequence_buffer {
    public:
    Sequence_buffer_element* read_sb;
    int read_index;
    
    int lowest_sequence;
    //int highest_sequence;

    //Sequence_buffer_element* write_sb;
    //int write_index;

    Sequence_buffer()
    {
        read_index = 0;
        //write_index = 0;
        //lowest_sequence = 0;
        //highest_sequence = 0;

        read_sb = new Sequence_buffer_element;
    }

    void insert(char* buff, int size, int sequence)
    {
        Sequence_buffer_element* sbe = read_sb;
        int i = read_index;
        int index = lowest_sequence;
        int _read_index = read_index;

        int count = 0; //debug

        //check to see if next packet in sequence

        if(lowest_sequence == sequence)
        {

            pop(buff, size);
            
            return;
        }

        while(index != sequence) 
        {
            index++; //update index, MOD SOMETHING
            _read_index++;
            if(_read_index == SEQUENCE_BUFFER_SIZE)
            {
                _read_index = 0;
                if(sbe->next == NULL ) 
                {
                    sbe->next = new Sequence_buffer_element; //use pool
                    sbe->next->next = NULL; //for object pool
                }
                sbe = sbe->next;
            }

            count++; //debug
            if(count > 1) printf("python channel: count = %i \n", count); //debug
        }

        Channel_message* cm = &sbe.cm[read_index];

        cm->buffer = new char[size];    //bypass when 
        cm->size = size;
        cm->sequence = sequence;
    }

    void pop(char* buff, int size) 
    {
        if(sbe.cm[read_index].buffer == NULL) printf("python sequence buffer ERROR!!\n");

        //process cm

        read_index++;
        lowest_sequence++; //USE MODULO
        if(read_index == SEQUENCE_BUFFER_SIZE)
        {
            Sequence_buffer_element* sbe = read_sb;
            read_index = 0;
            if(read_sb->next == NULL ) 
            {
                read_sb->next = new Sequence_buffer_element; //use pool
                read_sb->next->next = NULL;
            }
            read_sb = sbe->next;

            delete sbe; //retire old buffer
        }

        Channel_message* cm;

        while(sbe.cm[read_index].buffer != NULL)
        {
            cm = &sbe->cm[read_index];
            //process cm
            delete cm->buffer;
            cm->buffer = NULL;

            read_index++;
            lowest_sequence++;  //USE MODULO

            if(read_index == SEQUENCE_BUFFER_SIZE)
            {
                read_index = 0;
                Sequence_buffer_element* sbe = read_sb;
                read_sb = read_sb->next;
                delete sbe //use object pool
                //return sequence buffer when done
            }
        }
    }
};
class Python_channel_in {
    public:

    void receive_message(char* buff, int n, int sequence)
    {
    }

    Net_message* serialize_to_packet(int max_n)
    {
        return NULL:
    }

    Python_channel_in()
    {
    }
};