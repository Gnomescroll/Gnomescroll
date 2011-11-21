#pragma once

#include <c_lib/template/object_pool.hpp>

//struct struct net_message_buffer; //forward declaration

//when reference count goes to zero, retire
//check offset, when a write would bring offset over buffer size, get new one

static const int NET_MESSAGE_BUFFER_SIZE = 4096;

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
    int reference_count;    char* offset;
    char buffer[NET_MESSAGE_BUFFER_SIZE];
    class Net_message_buffer* next;    //may not be used
    
    static class Net_message_buffer* acquire() {
        //get from pool

        class Net_message_buffer* a = new Net_message_buffer;
        return a;
    }

    inline void decrement() {
            

    }

    inline void retire() {
        //return to object pool
        delete this;
    }


};

class Net_message_buffer_pool: public Object_pool<Net_message_buffer_pool, 128> {
    
    Net_message_buffer* current_0;
    int remaining_0;
    char* offset_0;

    Net_message_buffer* current_1;
    int remaining_1;
    char* offset_1;

    inline char* get_char_buffer_0(int length) {

        if(remaining < length) {
            current = acquire();
            remaining = NET_MESSAGE_BUFFER_SIZE;
        }
        char* b = current->offset;
        remaining -= length
        offset += length;
    }

    Net_message_buffer_pool() {
        
        if(first == NULL) printf("error init\n");

        current_0 = acquire();
        remaining_0 = NET_MESSAGE_BUFFER_SIZE;;
        offset_0 = current_0->buffer;

        //current_1 = acquire();
        //remaining_1 = NET_MESSAGE_BUFFER_SIZE;;
        //offset_1 = current_1->buffer;
    }
};




char* get_buffer_char_buffer_1(int length) {
    static Net_message_buffer* current = acquire();;
    static int remaining = NET_MESSAGE_BUFFER_SIZE;;
    static char* offset = current->buffer;

    if(remaining < length) {
        current = acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;
    }
    char* b = current->offset;
    remaining -= length
    offset += length;
}

{
    
    Net_message_buffer* current;
    int remaining;
    char* offset;

    char* get_buffer(int length) {
        if(remaining < length) {
            current = acquire;
            remaining = NET_MESSAGE_BUFFER_SIZE;
        }
        char* b = current->offset;
        remaining -= length
        offset += length;

    }

    Net_message_buffer_pool() {
        //test
        if(first==NULL) {
            printf("ERROR!!\n");
        }
        current = acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;
    }
};

class Net_message {
    private:
        //bool reliable; //if not reliable, decrement reference count after sending
        //reliable can be in send method
    public:
        class Net_message_buffer* b;
        char* buff;
        int len;
        int reference_count;

        void inline decrement_reliable() {
            return;
            reference_count--;
            if(reference_count == 0) 
            {
                Net_message_buffer->decrement();
                retire();
            }
        }
        Net_message() {
            reference_count = 1;
        }
        //not used
        inline void retire();
        static class Net_message* acquire(int size) { 
            //check pool
        }

        static class Net_message* acquire_reliable() { return new Net_message; }
        static class Net_message* acquire_unreliable() { return new Net_message; }
};

inline void Net_message::retire() {
    return;

    delete this;
    return;

    b->reference_count--;
    if(b->reference_count == 0) b->retire();
    //return to pool
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
