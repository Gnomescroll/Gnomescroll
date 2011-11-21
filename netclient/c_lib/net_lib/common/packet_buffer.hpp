#pragma once

#include <c_lib/template/object_pool.hpp>

//struct struct net_message_buffer; //forward declaration

//when reference count goes to zero, retire
//check offset, when a write would bring offset over buffer size, get new one

//static const int NET_MESSAGE_BUFFER_SIZE = 4092;
static const int NET_MESSAGE_BUFFER_SIZE = 16384;
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

class Net_message_buffer_pool: public Object_pool<Net_message_buffer, 128>
{
    private:
    public:
    inline void get_char_buffer(int length, char** b, Net_message_buffer** nmb) 
    {
        static Net_message_buffer* current = NULL;
        static int remaining = 0;
        static char* offset = NULL;

        if(remaining < length) 
        {
            current = acquire();
            remaining = NET_MESSAGE_BUFFER_SIZE;
            offset = current->buffer;
        }
        *b = offset;
        *nmb = current;
        remaining -= length;
        offset += length;
        current->reference_count++;
    }

    Net_message_buffer_pool() { if(first == NULL) printf("error init\n"); }
};

//use for unreliable packets
class Net_message_buffer_pool2: public Object_pool<Net_message_buffer, 16>
{
    private:
    public:

    inline void get_char_buffer(int length, char** b, Net_message_buffer** nmb) 
    {
        static Net_message_buffer* current = NULL;
        static int remaining = 0;
        static char* offset = NULL;

        if(remaining < length) 
        {
            current = acquire();
            remaining = NET_MESSAGE_BUFFER_SIZE;
            offset = current->buffer;
        }
        *b = offset;
        *nmb = current;
        remaining -= length;
        offset += length;
        current->reference_count++;
    }

    Net_message_buffer_pool2() { if(first == NULL) printf("error init\n"); }
};

Net_message_buffer_pool net_message_buffer_pool;   //use for reliable udp packets
Net_message_buffer_pool2 net_message_buffer_pool2; //use for unreliable udp packets


class Net_message {
    private:
    public:
        class Net_message_buffer* b;
        char* buff;
        int len;
        int reference_count;
        Net_message* next;

    void inline decrement_reliable();
    void inline decrement_unreliable();

    static inline class Net_message* acquire_reliable();
    static inline class Net_message* acquire_unreliable();

        Net_message() {
            reference_count = 0; 
            //increment reference on pushing onto packet buffer
            //decrement on dispatch or completion
        }
};

class Net_message_pool: public Object_pool<Net_message, 4096> {};

Net_message_pool net_message_pool;

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

class Net_message* Net_message::acquire_reliable()
{
    //fix, get buffer
    return new Net_message;
}

class Net_message* Net_message::acquire_unreliable()
{
    //fix, get buffer
    return new Net_message;
}


//Net_message_buffer_pool

/*
struct net_message {
    //int priority;
    struct net_message_buffer* b
    char* buff;
    int len;
};
*/

/*
    Serialize packets to net_message buffers
*/
//need pool for net messages

/*
    Get netmessage working for messages first!!!
    just new and delete for now

    After it is working, then focus on allocating them on buffers
*/

//version with reference count and version without?


/*
class Net_message_buffer {
    public:
        struct net_message_buffer* next;


};
*/

/*
Pop from top, push to bottom
*/

/*
static const NET_MESSAGE_BUFFER_POOL_SIZE = 4096
const static tDEBUG = 1;

class Net_message_buffer_pool {
    private:
        int batch_num;
        //batch malloc/link method
        void batch_alloc() {
            batch_num++;
            printf("Batch Alloc: %i \n", batch_num);
            class Net_message_buffer* ar = (class Net_message_buffer*) malloc(NET_MESSAGE_BUFFER_POOL_SIZE*sizeof(class Net_message_buffer));
            
            last.next = &ar[0];

            int i;
            for(i=0;i<NET_MESSAGE_BUFFER_POOL_SIZE-1; i++){
                ar[i].next = &ar[i+1];
            }

            ar[NET_MESSAGE_BUFFER_POOL_SIZE-1].next = NULL;
            last = &ar[NET_MESSAGE_BUFFER_POOL_SIZE-1];
        }

    public:
    class Net_message_buffer* first;
    class Net_message_buffer* last;

    class Net_message_buffer* acquire() 
    {
        if(tDEBUG) { if(first == NULL) printf("tError1 \n");}

        //if first==last, assuming, that first.next == NULL
        if(first.next == NULL) 
        {
            //first = (struct net_message_buffer*) malloc(sizeof(struct net_message_buffer))
            //first.next = NULL;
            //last = first; 
            
            batch_alloc();       
        }
        if(first == last) printf("tError2 \n");

        class Net_message_buffer* tmp = first;
        first = first.next;
        return tmp;
    }

    void retire(struct net_message_buffer* nmb)
    {
        last.next = nmb;
        last = nmb;
    }

    net_message_buffer_pool() {
            batch_num = 1;
            class Net_message_buffer* ar = (struct Net_message_buffer*) malloc(NET_MESSAGE_BUFFER_POOL_SIZE*sizeof(class Net_message_buffer));
            first = &ar[0];
            int i;
            for(i=0;i<NET_MESSAGE_BUFFER_POOL_SIZE-1; i++){
                ar[i].next = &ar[i+1];
            }
            ar[NET_MESSAGE_BUFFER_POOL_SIZE-1].next = NULL;
            last = &ar[NET_MESSAGE_BUFFER_POOL_SIZE-1];
    }

};
*/
