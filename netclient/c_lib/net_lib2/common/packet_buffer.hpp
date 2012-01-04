#pragma once

#include <c_lib/template/object_pool.hpp>

/*
NetMessage Array
*/

static const int NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second



static const int NET_MESSAGE_ARRAY_SIZE = 256; //use 256


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
        for(int i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        reference_count = 0;
        next = NULL;
    }

    inline void retire() __attribute__((always_inline));

    inline static NetMessageArray* acquire() __attribute__((always_inline));
};


class Net_message {
    private:
    public:
        class Net_message_buffer* b;
        char* buff;
        int len;
        int reference_count;
        Net_message* next;

        OBJECT_POOL_OBJECT_MACRO

    static inline class Net_message* acquire();

    void inline decrement_reliable();
    void inline decrement_unreliable();

    static class Net_message* acquire_reliable(int length);
    static class Net_message* acquire_unreliable(int length);

    Net_message() {
        reference_count = 0; 
        //increment reference on pushing onto packet buffer
        //decrement on dispatch or completion
    }
};
