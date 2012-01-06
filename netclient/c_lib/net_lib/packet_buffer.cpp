#include "packet_buffer.hpp"



class NetMessageArray_pool: public Object_pool<NetMessageArray, 64>  //set to 64, 2 for testing
{
    public:
        const char* name();
};
const char* NetMessageArray_pool::name() {
    return "NetMessageArray_pool";
} 

static NetMessageArray_pool net_message_array_pool;

void NetMessageArray::retire() 
{
    net_message_array_pool.retire(this);
}

NetMessageArray* NetMessageArray::acquire()
{
    NetMessageArray* a = net_message_array_pool.acquire();
    a->next = NULL; //debug
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

    OBJECT_POOL_OBJECT_MACRO

    Net_message_buffer() { reference_count = 0; }
};

class Net_message_buffer_pool: public Object_pool<Net_message_buffer, 128> 
{
    public:
    const char* name() {  static const char x[] = "Net_message_buffer_pool"; return x;} 

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

static Net_message_buffer_pool reliable_net_message_buffer_pool;




class Net_message_pool: public Object_pool<Net_message, 4096> // {}; //use 4096
{
public:
const char* name() 
    { 
        static const char x[] = "Net_message_pool";
        return x;
    } 
};
static Net_message_pool net_message_pool;

//Net_message_n++; printf("Created: %i netmessages\n", Net_message_n);

void inline Net_message::decrement() 
{
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
        net_message_pool.retire(this);
    }
}

class Net_message* Net_message::acquire(int length)
{
    Net_message* t = net_message_pool.acquire();
    t->len = length;
    net_message_buffer_pool.get_char_buffer(length, &t->buff, &t->b); //set buffer and set char pool
    t->reference_count = 0;
    return t;
}