


/*
    List of Subscribers
    History of changes
    Compression
*/


/*

    Compress chunk and send
    Store history
    Store last compression
    Send last compression and then history

*/

#include "t_map_class.hpp"

namespace t_map
{

/*
            unsigned char block;
            unsigned char palette;
            unsigned char damage;
            unsigned char lighting;
*/

struct CHUNK_HISTORY_ELEMENT
{
    int x, int y, int z;
    struct MAP_ELEMENT e;
};

const int SMC_MAX_SUBSCRIBERS = 64;
const int SMC_HISTORY_ARRAY_SIZE

class SERVER_MAP_CHUNK
{
    public:

    int subscribers[ SMC_MAX_SUBSCRIBERS ];
    int subscriber_num;

    struct CHUNK_HISTORY_ELEMENT history_array[ SMC_HISTORY_ARRAY_SIZE ];
    int last_history;

    SERVER_MAP_CHUNK()
    {
        subscriber_num = 0;
        memset( &history_array, 0, SMC_HISTORY_ARRAY_SIZE*sizeof(struct CHUNK_HISTORY_ELEMENT));
    }

    ~SERVER_MAP_CHUNK()
    {
                
    }

    void add_subscriber(int client_id, int client_map_version);
    void remove_subscriber(int client_id);


};


void SERVER_MAP_CHUNK::add_subscriber(int client_id, int client_map_version)
{
    //check that they are not already subscribed
    for(int i=0; i < subscriber_num; i++)
    {
        if( subscribers[i] == client_id )
        {
            printf("SERVER_MAP_CHUNK::add_subscriber, Error: client %i is already subscribed to map chunk\n", client_id);
            return;
        }
    }
    subscribers[subscriber_num] = client_id;
    subscriber_num++;

    //n00b solution; zip map up and send it

    if(client_map_version == 0xffff)
    {
        /*
            Client does not have any previous data
        */


        //just compress and send for now!
    }
}

void SERVER_MAP_CHUNK::remove_subscriber(int client_id)
{
    int i = 0;
    while(i < subscriber_num && subscribers[i] != client_id) i++;
    
    if(i == subscriber_num)
    {
        printf("SERVER_MAP_CHUNK::remove_subscriber, Error: client %i was not subscribed to map chunk!", client_id);
        return;
    }

    subscriber_num--;
    subscribers[i] = subcribers[subscriber_num];
}

}