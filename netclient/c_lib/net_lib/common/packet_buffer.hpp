#pragma once

#include <c_lib/template/object_pool.hpp>

/*
NetMessage Array
*/

static const int NET_MESSAGE_BUFFER_SIZE = 4096; // 69,000 per second, 74-78k per second



static const int NET_MESSAGE_ARRAY_SIZE = 256; //use 256

class NetMessageArray {
    private:
    public:
    int reference_count;
    class NetMessageArray* next;
    class Net_message* net_message_array[NET_MESSAGE_ARRAY_SIZE];


    NetMessageArray()
    {
        for(int i=0; i < NET_MESSAGE_ARRAY_SIZE; i++) net_message_array[i] = NULL;
        reference_count = 0;
        next = NULL;
    }

    inline void retire();

    inline static NetMessageArray* acquire();
};

class NetMessageArray_pool: public Object_pool<NetMessageArray, 64>  //set to 64, 2 for testing
{
    public:
    char* name() { static char x[] = "NetMessageArray_pool"; return x; } 
};

static NetMessageArray_pool net_message_array_pool;

inline void NetMessageArray::retire() 
{
    net_message_array_pool.retire(this);
}

NetMessageArray* NetMessageArray::acquire()
{
    NetMessageArray* a = net_message_array_pool.acquire();
    a->next = NULL;
    //a->reference_count = 0; //set to 1 automaticly
    return a;
}    

/*
Net message buffer
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
class Net_message_buffer_pool: public Object_pool<Net_message_buffer, 128> 
{
public:
char* name() 
    { 
        static char x[] = "Net_message_buffer_pool";
        return x;
    } 
};
//unreliable
class Net_message_buffer_pool2: public Object_pool<Net_message_buffer, 16>
{
public:
char* name() 
    { 
        static char x[] = "Net_message_buffer_pool2";
        return x;
    } 
};

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
        current->reference_count = 0; //needed? BUG?
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
        current->reference_count = 0; //needed? BUG?
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

class Net_message_pool: public Object_pool<Net_message, 4096> // {}; //use 4096
{
public:
char* name() 
    { 
        static char x[] = "Net_message_pool";
        return x;
    } 
};
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
