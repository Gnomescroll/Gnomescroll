#include "packet_buffer.hpp"

#define PACKET_BUFFER_MALLOC_DEBUG 0

#define NET_MESSAGE_ARRAY_MALLOC_DEBUG 0

//static NetMessageArrayPool net_message_array_pool;

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

class NetMessageBuffer
{
    public:
        int reference_count;
        char buffer[NET_MESSAGE_BUFFER_SIZE];
        NetMessageBuffer* next;
        OBJECT_POOL_OBJECT_MACRO

    NetMessageBuffer() :
        reference_count(0)
    {
    }
};

class NetMessageBufferPool: public ObjectPool<NetMessageBufferPool, NetMessageBuffer, 128>
{
    public:
    static const char* name()
    {
        return "NetMessageBufferPool";
    }

    NetMessageBuffer* current;
    int remaining;
    char* offset;

    NetMessageBufferPool()
    {
        //printf("=== init ====\n");
        current = this->acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;;
        offset = current->buffer;
        current->reference_count = 1;
    }

    inline void get_char_buffer(int length, char** b, NetMessageBuffer** nmb)
    {
        if (remaining < length)
        {
            current->reference_count--;
            if (current->reference_count == 0) this->retire(current);

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

static NetMessageBufferPool net_message_buffer_pool;




class NetMessagePool: public ObjectPool<NetMessagePool, NetMessage, 4096> // {}; //use 4096
{
    public:
    static const char* name()
    {
        return "NetMessagePool";
    }
};

static NetMessagePool net_message_pool;

//Net_message_n++; printf("Created: %i netmessages\n", Net_message_n);

void inline NetMessage::decrement()
{

#if PACKET_BUFFER_MALLOC_DEBUG
    reference_count--;
    if (reference_count == 0)
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
    if (reference_count == 0)
    {
        b->reference_count--;
        if (b->reference_count == 0)
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

class NetMessage* NetMessage::acquire(size_t length)
{
    if (length <= 0) return NULL;
#if PACKET_BUFFER_MALLOC_DEBUG
    NetMessage* t = new NetMessage;
    t->len = length;
    t->buff = new char[length];
    t->reference_count = 0;
    return t;
#else
    NetMessage* t = net_message_pool.acquire();
    t->len = length;
    net_message_buffer_pool.get_char_buffer(length, &t->buff, &t->b); //set buffer and set char pool
    t->reference_count = 0;
    return t;
#endif
}

class NetMessage* arbitrary_acquire(size_t size)
{
    if (size == 0) return NULL;
    class NetMessage* nm = net_message_pool.acquire();
    nm->len = size;
    net_message_buffer_pool.get_char_buffer(size, &nm->buff, &nm->b); //set buffer and set char pool
    nm->reference_count = 0;
    return nm;
}

/* NetMessageManager */

NetMessageManager::NetMessageManager()
{
    nma_insert = NetMessageArray::acquire();

    nma_insert_index = 0;
    nma_read = nma_insert;
    nma_read_index = 0;

    pending_messages = 0;
    pending_bytes_out = 0;
}


void NetMessageManager::push_message(NetMessage* nm)
{
    //if (nm->len == 0) {printf("NETMESSAGEERROR!!!!\n");}

    pending_bytes_out += nm->len;
    pending_messages++;

    nm->reference_count++;  //increment reference count?

    //insert message
    nma_insert->net_message_array[nma_insert_index] = nm;

    //insert
    nma_insert_index++;
    if (nma_insert_index == NET_MESSAGE_ARRAY_SIZE)
    {
        //printf("NetMessageManager::push_message, rare condition \n");
        //DEBUG?
        nma_insert->next = NetMessageArray::acquire();
        nma_insert->next->next = NULL; //head of list
        nma_insert = nma_insert->next;
        nma_insert_index = 0;
    }
}

void NetMessageManager::serialize_messages(char* buff_, size_t index)
{
    size_t max = pending_bytes_out;
    //printf("Starting serialization at address %i \n", buff_);
/*
    if (pending_messages == 0)
    {
        printf("impossible error \n");
        return;
    }
*/
    /*
        Create packet and serialize to it
    */

    class NetMessage* nm;

    for (int i=0; i < pending_messages; i++)
    {
        nm = nma_read->net_message_array[nma_read_index];

        memcpy(buff_+index, nm->buff, nm->len);
        index += nm->len;

/*
        if (index > max)
        {
            printf("BLOODY HELL: %i, %i \n", index, max);
        }
*/
        nm->decrement(); //reference count on packet

        nma_read_index++;
        if (nma_read_index == NET_MESSAGE_ARRAY_SIZE)
        {
            //printf("NetMessageManager::serialize_messages, rare condition \n");
            NetMessageArray* tmp = nma_read;
            nma_read = nma_read->next;
            tmp->retire();
            nma_read_index=0;
        }
    }

    if (max != index)
    {
        printf("NetMessageManager, ERROR, index exceeds bytes to be write: index= %i, max= %i \n", index, max);
    }
    //reset to virgin state
    nma_insert_index = 0;
    nma_read = nma_insert;
    nma_read_index = 0;

    pending_messages = 0;
    pending_bytes_out = 0;
}
