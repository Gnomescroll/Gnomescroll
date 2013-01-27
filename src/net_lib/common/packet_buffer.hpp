#pragma once

#include <common/template/object_pool.hpp>

#include <string.h>

/*
NetMessage Array
*/

static const int NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second
static const int NET_MESSAGE_ARRAY_SIZE = 256; //use 256

#define PACKET_BUFFER_DEBUG 1

//used by net_peer
class NetMessageArray {
    private:
    public:
    int reference_count;
    class NetMessageArray* next;
    class Net_message* net_message_array[NET_MESSAGE_ARRAY_SIZE];

    OBJECT_POOL_OBJECT_MACRO

    NetMessageArray()
    {
        #if PACKET_BUFFER_DEBUG
            for (int i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        #endif
        reference_count = 0;
        next = NULL;
    }

    inline void retire() __attribute__((always_inline));

    inline static NetMessageArray* acquire() __attribute__((always_inline));
};


class NetMessageArray_pool: public Object_pool<NetMessageArray_pool, NetMessageArray, 64>  //set to 64, 2 for testing
{
    public:
        static char* name();
};

char* NetMessageArray_pool::name() {
    static char* x = (char*) "NetMessageArray_pool"; return x;
}


class Net_message {
    private:
    public:
        class Net_message_buffer* b;
        char* buff;
    #if PACKET_BUFFER_DEBUG
        unsigned int len;
        int reference_count;
    #else
        unsigned short len;
        short reference_count;
    #endif
        Net_message* next;

        OBJECT_POOL_OBJECT_MACRO

    static class Net_message* acquire(unsigned int length);
    // __attribute((always_inline));    // wont compile gcc4.4.3 (ubuntu10.04)

    void inline decrement();

    Net_message()
    : len(0), reference_count(0)
    {
        reference_count = 0;
        //increment reference on pushing onto packet buffer
        //decrement on dispatch or completion
    }
};


class NetMessageManager
{
    public:

    int pending_messages;
    unsigned int pending_bytes_out;

    class NetMessageArray* nma_insert; //array for inserting
    int nma_insert_index;              //index for insertions

    class NetMessageArray* nma_read;   //array for reading
    int nma_read_index;                //index for reads

    NetMessageManager();

    void push_message(Net_message* nm);
    void serialize_messages(char* buff_, unsigned int index);

};

class Net_message* arbitrary_acquire(unsigned int size);
