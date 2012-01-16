#include "packet_buffer.hpp"


#define PACKET_BUFFER_MALLOC_DEBUG 1

#define NET_MESSAGE_ARRAY_MALLOC_DEBUG 1

class NetMessageArray_pool: public Object_pool<NetMessageArray_pool, NetMessageArray, 64>  //set to 64, 2 for testing
{
    public:
        static char* name();
};

char* NetMessageArray_pool::name() {
    static char* x = (char*) "NetMessageArray_pool"; return x;
} 

static NetMessageArray_pool net_message_array_pool;

void NetMessageArray::retire() 
{
#if NET_MESSAGE_ARRAY_MALLOC_DEBUG
    delete this;
#else
    net_message_array_pool.retire(this);
#endif
}

class NetMessageArray* NetMessageArray::acquire()
{
#if NET_MESSAGE_ARRAY_MALLOC_DEBUG
    NetMessageArray* a = new NetMessageArray;
    return a;
#else
    NetMessageArray* a = net_message_array_pool.acquire();
    a->next = NULL; //debug
    //a->reference_count = 0; //set to 1 automaticly
    return a;
#endif
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

    OBJECT_POOL_OBJECT_MACRO

    Net_message_buffer() { reference_count = 0; }
};

class Net_message_buffer_pool: public Object_pool<Net_message_buffer_pool, Net_message_buffer, 128> 
{
    public:
    static char* name() { static char* x = (char*) "Net_message_buffer_pool"; return x; } 

    Net_message_buffer* current;
    int remaining;
    char* offset;

    Net_message_buffer_pool()
    {
        //printf("=== init ====\n");
        current = this->acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;;
        offset = current->buffer;
        current->reference_count = 1;
    }

    inline void get_char_buffer(int length, char** b, Net_message_buffer** nmb) 
    {
        if(remaining < length) 
        {
            current->reference_count--;
            if(current->reference_count == 0) this->retire(current);

            current = this->acquire();
            current->reference_count = 1; //hold reference until done
            remaining = NET_MESSAGE_BUFFER_SIZE;
            offset = current->buffer;
        }
        *b = offset;
        *nmb = current;
        remaining -= length;
        offset += length;
        current->reference_count++;
    }
};

static Net_message_buffer_pool net_message_buffer_pool;




class Net_message_pool: public Object_pool<Net_message_pool, Net_message, 4096> // {}; //use 4096
{
public:
    static char* name() 
    { 
        static char* x = (char*) "Net_message_pool";
        return x;
    } 
};
static Net_message_pool net_message_pool;

//Net_message_n++; printf("Created: %i netmessages\n", Net_message_n);

void inline Net_message::decrement() 
{

#if PACKET_BUFFER_MALLOC_DEBUG
    reference_count--;
    if(reference_count == 0) 
    {
        delete[] buff;
        delete this;
        len = 0;
        return;
    }
    return;
#else


    //printf("net message decrement: object=%lx ref count= %i, b->refcount= %i \n", (long)this, reference_count, b->reference_count);
    reference_count--;
    if(reference_count == 0) 
    {
        b->reference_count--;
        if(b->reference_count == 0) 
        {
            //printf("net message reference count == 0\n");
            //printf("alloc= %i \n", allocated);
            net_message_buffer_pool.retire(b);
            b = NULL;   //debug
        }
        //cannot retire it here
        len = 0; //for safety
        net_message_pool.retire(this);
    }
#endif
}

class Net_message* Net_message::acquire(int length)
{
#if PACKET_BUFFER_MALLOC_DEBUG
    Net_message* t = new Net_message;
    t->len = length;
    t->buff = new char[length];
    t->reference_count = 0;
    return t;
#else
    Net_message* t = net_message_pool.acquire();
    t->len = length;
    net_message_buffer_pool.get_char_buffer(length, &t->buff, &t->b); //set buffer and set char pool
    t->reference_count = 0;
    return t;
#endif
}