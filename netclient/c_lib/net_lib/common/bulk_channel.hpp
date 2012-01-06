#pragma once

//chunks data, resassembles

#include <template/net.hpp>


struct bulk_channel_message;
struct bulk_channel_message_head;

struct bulk_channel_message
{
    bulk_channel_message* next;
    char data[256];

    int id;
    int parts;  //total number of parts
    int seq;    //part num
};

class Bulk_channel_message_head
{
    public:
    int id;
    struct Bulk_channel_message_head* next;     //linked list of ordered linked lists
    struct bulk_channel_message* first;         //head of ordered linked list
    int fragment_count;
    int fragments_needed;

    Bulk_channel_message_head(int _id, struct bulk_channel_message* bcm)
    {
        id = _id;
        next = NULL;
        first = bcm;
        fragments_needed = bcm->parts;
        fragment_count = 0;
    }

    int add_node(struct bulk_channel_message* bcm)
    {
        if(bcm->seq <= first->seq)
        {
            if(bcm->seq == first->seq)
            {
                printf("Bulk Channel: Redundant Data Packet Received \n");
                free bcm;
                return;
            }
            bcm->next = first;
            first = bcm;
        }

        while(1)
        {
            struct bulk_channel_message* itr = first;
            if(itr->next == NULL)
            {
                itr->next = bcm;
                fragment_count++;
                break;    
            }
            if(bcm->seq < itr->next->seq)
            {
                bcm->next = itr->next;
                itr->next = bcm;
                fragment_count++;
                break;
            }
        }
    }

    static void add_node(Bulk_channel_message_head* bcmh, struct bulk_channel_message* bcm)
    {
        printf("add node: %i \n", bcm->seq);

        int id = bcm->id;
        while(1)
        {
            //add to current chain
            if( id == bcmh->id)
            {
                if(bcmh->add_node(bcm) == 1) bcmh;
                return;
            }
            else
            {
                if(next == NULL)
                {
                    next = new Bulk_channel_message_head(id, bcm);
                    //next->add_node(bcm);
                    return;
                }
                else
                {
                    bcmh = next;    
                }
            }
        }
    }

    static void process(Bulk_channel_message_head* bcmh)
    {
        if(bcmh->fragment_count == bcmh->fragments_needed)
        {
            struct bulk_channel_message* itr = bcmh->first;
            int i = 0;
            while(itr != NULL)
            {
                i++;
                itr = itr->next;
            }
            printf("bmc: %i of %i of %i \n", i, bcmh->fragments_needed, bcmh->fragment_count);
            return 1;
        }
        else
        {
            printf("Bulk message channel: messaged %i of %i received \n", fragment_count, fragments_needed );
        }

        return;
    }
};


/*
    Not thread safe
*/

class Bulk_channel_message_to_client: public FixedSizeReliableNetPacketToServer<Bulk_channel_message_to_client>
{
    public:
        int id;
        int parts;
        int seq;

        static const data_size = 256;
        char data[data_size]; //256

        Bulk_channel_message_to_client()
        {
            data = NULL; //for safety/debugging
        }
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u16(&parts, buff, buff_n, pack);
            pack_u16(&seq, buff, buff_n, pack);
            if(pack)    //serialize from pointer
            {
                //copy from data into buffer
                memcpy(buff+*buff_n, data, data_size);
                *buff_n += data_size;                  
            }
            if(!pack)
            {
                //copy data to buffer
                memcpy(data, buff+*buff_n, data_size);
                *buff_n += data_size;  
            }
        }

        inline void handle()
        {
            

        }
};

class Bulk_channel
{
    Bulk_channel()
    {
        

    }



};