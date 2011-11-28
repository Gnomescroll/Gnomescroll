#pragma once

#include <c_lib/template/object_pool.hpp>

//struct struct net_message_buffer; //forward declaration

//when reference count goes to zero, retire
//check offset, when a write would bring offset over buffer size, get new one

/*
Benchmarks for buffer size for unreliable message packets
*/
//static const int NET_MESSAGE_BUFFER_SIZE = 4092;
static const int NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second
//static const int NET_MESSAGE_BUFFER_SIZE = 8192; // 79,000 per second
//static const int NET_MESSAGE_BUFFER_SIZE = 16384; //  81,000 per second

//static const int NET_MESSAGE_BUFFER_SIZE = 32768; //  71,000 per second

/*
    struct net_message_buffer {
        int reference_count;
        int offset;
        char buffer[NET_MESSAGE_BUFFER_SIZE];
        struct net_message_buffer* next;    //may not be used
    };
*/

class Net_message_buffer {
    private:
    public:
    int reference_count;
    char buffer[NET_MESSAGE_BUFFER_SIZE];
    Net_message_buffer* next;

    Net_message_buffer() { reference_count = 0; }
};

//reliable packet scratch
class Net_message_buffer_pool: public Object_pool<Net_message_buffer, 128> {};
//unreliable
class Net_message_buffer_pool2: public Object_pool<Net_message_buffer, 16> {};

static Net_message_buffer_pool net_message_buffer_pool;   //use for reliable udp packets
static Net_message_buffer_pool2 net_message_buffer_pool2; //use for unreliable udp packets

inline void get_char_buffer(int length, char** b, Net_message_buffer** nmb) 
{
    static Net_message_buffer* current = NULL;
    static int remaining = 0;
    static char* offset = NULL;
    if(remaining < length) 
    {
        current = net_message_buffer_pool.acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;
        offset = current->buffer;
    }
    *b = offset;
    *nmb = current;
    remaining -= length;
    offset += length;
    current->reference_count++;
}

//for unreliable packets, short term storage
inline void get_char_buffer2(int length, char** b, Net_message_buffer** nmb)
{
    static Net_message_buffer* current = NULL;
    static int remaining = 0;
    static char* offset = NULL;
    if(remaining < length) 
    {
        current = net_message_buffer_pool2.acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;
        offset = current->buffer;
    }
    *b = offset;
    *nmb = current;
    remaining -= length;
    offset += length;
    current->reference_count++;
}

//int Net_message_n = 0;

class Net_message {
    private:
    public:
        class Net_message_buffer* b;
        char* buff;
        int len;
        int reference_count;
        Net_message* next;

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

class Net_message_pool: public Object_pool<Net_message, 4096> {}; //use 4096

static Net_message_pool net_message_pool;

//Net_message_n++; printf("Created: %i netmessages\n", Net_message_n);

void inline Net_message::decrement_unreliable() 
{
    reference_count--;
    if(reference_count == 0) 
    {
        b->reference_count--;
        if(b->reference_count == 0) net_message_buffer_pool2.retire(b);
        net_message_pool.retire(this);
    }
}

void inline Net_message::decrement_reliable() 
{
    reference_count--;
    if(reference_count == 0) 
    {
        b->reference_count--;
        if(b->reference_count == 0) net_message_buffer_pool.retire(b);
        net_message_pool.retire(this);
    }
}

class Net_message* Net_message::acquire_unreliable(int length)
{
    Net_message* t = net_message_pool.acquire();
    t->len = length;
    get_char_buffer2(length, &t->buff, &t->b); //set buffer and set char pool
    t->reference_count = 0;
    return t;
}

class Net_message* Net_message::acquire_reliable(int length)
{
    Net_message* t = net_message_pool.acquire();
    t->len = length;
    get_char_buffer(length, &t->buff, &t->b); //set buffer and set char pool
    t->reference_count = 0;
    return t;
}
