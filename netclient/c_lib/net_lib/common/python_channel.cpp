#include "python_channel.hpp"

//Python_channel_out
void Python_channel_out::write_message(char* buff, int n)
{
    //printf("write_message: length= %i \n", n);
    char t[2]; int n1=0; PACK_uint16_t(n, t, &n1);
    fcb.write(t,2);
    fcb.write(buff,n);

    pending_bytes_out += n+2;
}

Net_message* Python_channel_out::serialize_to_packet(int max_n)
{
    int bytes = max_n > pending_bytes_out ? pending_bytes_out : max_n;
    Net_message* nm = Net_message::acquire_reliable(bytes+5);   //need 2 bytes for sequence prefix
    /*
    static int _i = 0;
    static int l1 = 0;
    static int l2 = 0;

    if(_i % 3 == 0)
    {
        l1 += 3;
        l2 = 1;
    }
    if(_i % 3 == 1)
    {
        l1 += 0;
        l2 = 2;
    }
    if(_i % 3 == 2)
    {
        l1 += 0;
        l2 = 3;
    }

    int _sequence_number = l1 - l2;

    _i++;
    */

    //printf("sequence= %i \n", sequence_number);

    int n1 =0;
    printf("Py_out: sequence= %i, length= %i \n", sequence_number, bytes);
    PACK_uint8_t(254, nm->buff, &n1);      //message id
    PACK_uint16_t(bytes, nm->buff, &n1);    //length
    PACK_uint16_t(sequence_number, nm->buff, &n1); //sequence number
    //fcb.write(t, 5);
    
    sequence_number++;

    //pack data
    //fcb.write(nm->buff+5, bytes);
    
    //read data into packet
    fcb.read(nm->buff+5, bytes);

    pending_bytes_out -= bytes;
    return nm;
}

//Python_channel_in
void Python_channel_in::insert(char* buff, int length, int sequence)
{
    //printf("py_in packet %i: seq=%i, len=%i lowest_sequence=%i read_index=%i \n", _total_packets, sequence, length, lowest_sequence, read_index);
    //if sequence is expected packet, use fast path and dont store anything
    if(lowest_sequence == sequence)
    {
        pop(buff, length);
        return;
    } 

    Sequence_buffer_element* sbe = read_sb;
    int index = lowest_sequence+1; //use modulo
    int _read_index = read_index;

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
                sbe->next->next = NULL; //new head
            }
            sbe = sbe->next;
        }
    }
    size++; 
    //printf("insert: index= %i, read_index= %i, %i elements \n", index, read_index, size);
    Channel_message* cm = &sbe->cm[_read_index];
    cm->buffer = new char[length];
    cm->length = length;
    memcpy (cm->buffer, buff, length);
}

void Python_channel_in::pop(char* buff, int length) 
{
    //process cm
    fcb.write(buff, length);
    //process(buff, length);
    //printf("1 python packet processed:: seq= %i \n", lowest_sequence);
    lowest_sequence++; //USE MODULO

    if(size == 0)
    {
        process();
        return;  //size equals zero: fast path return   
    }
    Channel_message* cm;
    Sequence_buffer_element* sbe = read_sb;
    while(sbe->cm[read_index].buffer != NULL)
    {
        size--; 
        cm = &sbe->cm[read_index];
        //process cm
        //process(cm->buffer, cm->length);
        fcb.write(cm->buffer, cm->length);
        delete cm->buffer;
        cm->buffer = NULL; //clear message
        //printf("2 python packet processed: seq= %i, %i left in buffer \n", lowest_sequence, size);
        read_index++;
        lowest_sequence++;  //USE MODULO
        if(read_index == SEQUENCE_BUFFER_SIZE)
        {
            read_index = 0;
            Sequence_buffer_element* tmp = read_sb;
            read_sb = read_sb->next;
            delete tmp; //use object pool
        }
    }
    if(size==0) read_index = 0;
    process();
}

#include <net_lib/common/net_peer.hpp>

void Python_channel_in::process()
{
    if(fcb.size != 0 and fcb.size <= 2) printf("py_message 3: #$@432432432\n");
    while(need < fcb.size)
    {
        if(need == 0)
        {
            if(fcb.size <= 2) printf("py_message 2: #$@432432432\n");
            char t[2];
            int _n = 0;
            fcb.read(t,2);
            uint16_t _length;
            UNPACK_uint16_t(&_length, t, &_n);
            need = _length;

            if(need < fcb.size) break;
        }

        char* tmp = new char[need];
        fcb.read(tmp, need);
        
        if(PY_MESSAGE_CALLBACK_GLOBAL == NULL) 
        {
            printf("PY_MESSAGE_CALLBACK_GLOBAL is NULL\n");    
        } else {
            int client_id = 0;
            PY_MESSAGE_CALLBACK_GLOBAL(tmp, need, np->client_id);
        }

        delete tmp;
        need = 0;
    }
}
    


//reference implementation
/*

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

static int _total_packets = 0;

class Python_channel_in {
    public:
    Sequence_buffer_element* read_sb;
    int read_index;
    int lowest_sequence;
    int size;

    Python_channel_in()
    {
        lowest_sequence = 0;
        read_index = 0;
        size = 0;
        read_sb = new Sequence_buffer_element;
    }

    void insert(char* buff, int length, int sequence)
    {
        //printf("py_in packet %i: seq=%i, len=%i lowest_sequence=%i read_index=%i \n", _total_packets, sequence, length, lowest_sequence, read_index);
        //if sequence is expected packet, use fast path and dont store anything
        if(lowest_sequence == sequence)
        {
            pop(buff, length);
            return;
        } 

        Sequence_buffer_element* sbe = read_sb;
        int index = lowest_sequence+1; //use modulo
        int _read_index = read_index;

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
                    sbe->next->next = NULL; //new head
                }
                sbe = sbe->next;
            }
        }
        size++; 
        //printf("insert: index= %i, read_index= %i, %i elements \n", index, read_index, size);
        Channel_message* cm = &sbe->cm[_read_index];
        cm->buffer = new char[length];
        cm->length = length;
        memcpy (cm->buffer, buff, length);
    }

    void pop(char* buff, int length) 
    {
        //process cm
        {
            int _n = 0;
            int length;
            uint16_t length;
            UNPACK_uint16_t(&length, buff, &_n);
        }
        //printf("1 python packet processed:: seq= %i \n", lowest_sequence);
        lowest_sequence++; //USE MODULO

        if(size == 0)  return;  //size equals zero: fast path return
        Channel_message* cm;
        Sequence_buffer_element* sbe = read_sb;
        while(sbe->cm[read_index].buffer != NULL)
        {
            size--; 
            cm = &sbe->cm[read_index];
            //process cm
            delete cm->buffer;
            cm->buffer = NULL; //clear message
            //printf("2 python packet processed: seq= %i, %i left in buffer \n", lowest_sequence, size);
            read_index++;
            lowest_sequence++;  //USE MODULO
            if(read_index == SEQUENCE_BUFFER_SIZE)
            {
                read_index = 0;
                Sequence_buffer_element* tmp = read_sb;
                read_sb = read_sb->next;
                delete tmp; //use object pool
            }
        }
        if(size==0) read_index = 0;
    }
};
*/