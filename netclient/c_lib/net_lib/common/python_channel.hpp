#pragma once



//#include <net_lib/common/packet_buffer.cpp>
//#include <c_lib/template/object_pool.hpp>

//void * memcpy ( void * destination, const void * source, size_t num );

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
    
    int size;

    Sequence_buffer()
    {
        read_index = 0;
        size = 0;
        read_sb = new Sequence_buffer_element;
    }

    void insert(char* buff, int size, int sequence)
    {
        Sequence_buffer_element* sbe = read_sb;

        int index = lowest_sequence;
        int _read_index = read_index;

        int count = 0; //debug

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

        size++; printf("size increment= %i\n", size);
        Channel_message* cm = &sbe->cm[read_index];
        cm->buffer = new char[size];    //bypass when 
        cm->size = size;
    }

    void pop(char* buff, int size) 
    {
        if(read_sb->cm[read_index].buffer == NULL) printf("python sequence buffer ERROR!!\n");
        //process cm
        printf("processed: %i \n", lowest_sequence);
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
        //if(size == 0) return;
        Channel_message* cm;
        Sequence_buffer_element* sbe = read_sb;
        while(sbe->cm[read_index].buffer != NULL)
        {
            size--; printf("size decrement= %i\n", size);
            cm = &sbe->cm[read_index];
            //process cm
            delete cm->buffer;
            cm->buffer = NULL;
            printf("processed: %i \n", lowest_sequence);
            read_index++;
            lowest_sequence++;  //USE MODULO
            if(read_index == SEQUENCE_BUFFER_SIZE)
            {
                read_index = 0;
                Sequence_buffer_element* tmp = read_sb;
                read_sb = read_sb->next;
                delete tmp; //use object pool
                //return sequence buffer when done
            }
        }
    }
};


class Python_channel_in {
    public:
    Sequence_buffer_element* read_sb;
    int read_index;
    
    int lowest_sequence;
    
    int size;

    Python_channel_in()
    {
        read_index = 0;
        size = 0;
        read_sb = new Sequence_buffer_element;
    }

    void insert(char* buff, int size, int sequence)
    {
        Sequence_buffer_element* sbe = read_sb;

        int index = lowest_sequence;
        int _read_index = read_index;

        int count = 0; //debug

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

        size++; printf("size increment= %i\n", size);
        Channel_message* cm = &sbe->cm[read_index];
        cm->buffer = new char[size];    //bypass when 
        cm->size = size;
    }

    void pop(char* buff, int size) 
    {
        if(read_sb->cm[read_index].buffer == NULL) printf("python sequence buffer ERROR!!\n");
        //process cm
        printf("processed: %i \n", lowest_sequence);
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
        //if(size == 0) return;
        Channel_message* cm;
        Sequence_buffer_element* sbe = read_sb;
        while(sbe->cm[read_index].buffer != NULL)
        {
            size--; printf("size decrement= %i\n", size);
            cm = &sbe->cm[read_index];
            //process cm
            delete cm->buffer;
            cm->buffer = NULL;
            printf("processed: %i \n", lowest_sequence);
            read_index++;
            lowest_sequence++;  //USE MODULO
            if(read_index == SEQUENCE_BUFFER_SIZE)
            {
                read_index = 0;
                Sequence_buffer_element* tmp = read_sb;
                read_sb = read_sb->next;
                delete tmp; //use object pool
                //return sequence buffer when done
            }
        }
    }
};