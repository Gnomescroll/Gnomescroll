#include "map_chunk_history.hpp"

namespace t_map
{

void Map_chunk_history::add_subscriber(int client_id, int chunk_alias, int client_map_version)
{
    //check that they are not already subscribed
    for(int i=0; i < subscriber_num; i++)
    {
        if( subscribers[i] == client_id )
        {
            printf("Map_chunk_history::add_subscriber, Error: client %i is already subscribed to map chunk\n", client_id);
            return;
        }
    }
    subscribers[subscriber_num] = client_id;
    chunk_aliases[subscriber_num] = chunk_alias;

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

void Map_chunk_history::remove_subscriber(int client_id)
{
    int i = 0;
    while(i < subscriber_num && subscribers[i] != client_id) i++;
    
    if(i == subscriber_num)
    {
        printf("Map_chunk_history::remove_subscriber, Error: client %i was not subscribed to map chunk!", client_id);
        return;
    }

    subscriber_num--;
    subscribers[i] = subscribers[subscriber_num];
    chunk_aliases[i] = chunk_aliases[subscriber_num]; 
}

}