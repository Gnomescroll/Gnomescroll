#pragma once



//struct struct net_message_buffer; //forward declaration

struct net_message_buffer {
    int reference_count;
    int offset;
    char buffer[NET_MESSAGE_BUFFER_SIZE];
    struct net_message_buffer* next;

};

/*
Pop from top, push to bottom
*/

static const NET_MESSAGE_BUFFER_POOL_SIZE = 4096
const static tDEBUG = 1;

class net_message_buffer_pool {
    private:
        //batch malloc/link method
        void batch_alloc() {
            
            struct net_message_buffer* ar = (struct net_message_buffer*) malloc(NET_MESSAGE_BUFFER_POOL_SIZE*sizeof(struct net_message_buffer));
            
            last.next = &ar[0];

            int i;
            for(i=0;i<NET_MESSAGE_BUFFER_POOL_SIZE-1; i++){
                ar[i].next = &ar[i+1];
            }

            ar[NET_MESSAGE_BUFFER_POOL_SIZE-1].next = NULL;
            last = &ar[NET_MESSAGE_BUFFER_POOL_SIZE-1];
        }

    public:
    struct net_message_buffer* first;
    struct net_message_buffer* last;

    struct net_message_buffer* acquire() 
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

        struct net_message_buffer* tmp = first;
        first = first.next;
        return tmp;
    }

    void retire(struct net_message_buffer* nmb)
    {
        last.next = nmb;
        last = nmb;
    }

    net_message_buffer_pool() {
            struct net_message_buffer* ar = (struct net_message_buffer*) malloc(NET_MESSAGE_BUFFER_POOL_SIZE*sizeof(struct net_message_buffer));
            first = &ar[0];
            int i;
            for(i=0;i<NET_MESSAGE_BUFFER_POOL_SIZE-1; i++){
                ar[i].next = &ar[i+1];
            }
            ar[NET_MESSAGE_BUFFER_POOL_SIZE-1].next = NULL;
            last = &ar[NET_MESSAGE_BUFFER_POOL_SIZE-1];
    }

};

static const NET_MESSAGE_BUFFER_SIZE = 4096;

//when reference count goes to zero, retire
//check offset, when a write would bring offset over buffer size, get new one
struct net_message_buffer {
    int reference_count;
    int offset;
    char buffer[NET_MESSAGE_BUFFER_SIZE];
    struct net_message_buffer* next;

};

struct net_message {
    //int priority;
    struct net_message_buffer* b
    char* buff;
    int len;
};

class Net_message_buffer {
    public:
        struct net_message_buffer* next;


}