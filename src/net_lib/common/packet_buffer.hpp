/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

#include <common/template/object_pool.hpp>

#include <string.h>

/*
NetMessage Array
*/

static const size_t NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second
static const size_t NET_MESSAGE_ARRAY_SIZE = 256; //use 256

#define PACKET_BUFFER_DEBUG 1

//used by net_peer
class NetMessageArray
{
    public:
        ssize_t reference_count;
        class NetMessageArray* next;
        class NetMessage* net_message_array[NET_MESSAGE_ARRAY_SIZE];

        ENTITY_POOL_ENTITY_MACRO

    NetMessageArray()
    {
        #if PACKET_BUFFER_DEBUG
        for (size_t i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        #endif
        reference_count = 0;
        next = NULL;
    }

    ALWAYS_INLINE void retire();

    ALWAYS_INLINE static NetMessageArray* acquire();
};


class NetMessageArrayPool: public ObjectPool<NetMessageArrayPool, NetMessageArray, 64>  //set to 64, 2 for testing
{
    public:
    static const char* name()
    {
        return "NetMessageArrayPool";
    }
};



class NetMessage
{
    public:
        class NetMessageBuffer* b;
        char* buff;
    #if PACKET_BUFFER_DEBUG
        size_t len;
        ssize_t reference_count;
    #else
        unsigned short len;
        short reference_count;
    #endif
        NetMessage* next;

        ENTITY_POOL_ENTITY_MACRO

    static class NetMessage* acquire(size_t length);

    void inline decrement();

    NetMessage() :
        len(0), reference_count(0)
    {
        reference_count = 0;
        //increment reference on pushing onto packet buffer
        //decrement on dispatch or completion
    }
};


class NetMessageManager
{
    public:

        size_t pending_messages;
        size_t pending_bytes_out;

        class NetMessageArray* nma_insert; //array for inserting
        size_t nma_insert_index;              //index for insertions

        class NetMessageArray* nma_read;   //array for reading
        size_t nma_read_index;                //index for reads

    NetMessageManager();

    void push_message(NetMessage* nm);
    void serialize_messages(char* buff_, size_t index);

};

class NetMessage* arbitrary_acquire(size_t size);
