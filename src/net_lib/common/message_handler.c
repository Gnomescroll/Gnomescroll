#include "message_handler.h"

#include <net_lib/common/type_pack.hpp>

#include <net_lib/global.hpp>

static size_t* h_client_packet_size;
static size_t* h_server_packet_size;

static pt2handler_client* client_handler_array;
static pt2handler_server* server_handler_array;

//should disconnect client
void default_handler_function(char* buff, int n, int* read_bytes)
{
    //printf("ERROR!!\nNo handler for message_id= %i\n", message_id);
    printf("ERROR! No message handler assigned for this message id!\n");
    *read_bytes = -1;
}

void register_server_message_handler(int message_id, size_t size, pt2handler_server fptr)
{
    if (message_id > 255 || message_id <0) {printf("register_server_message_handler: message ID invalid!\n");return;}
    GS_ASSERT(server_handler_array != NULL);
    if (server_handler_array == NULL) return;
    if (server_handler_array[message_id] != NULL) {printf("register_server_message_handler: reassigning message_id %i !!!\n", message_id);}

    GS_ASSERT(h_server_packet_size != NULL);
    if (h_server_packet_size == NULL) return;
    GS_ASSERT(server_handler_array != NULL);
    if (server_handler_array == NULL) return;

    h_server_packet_size[message_id] = size;
    //printf("CtoS: id,size %d,%d\n", message_id, size);
    server_handler_array[message_id] = fptr;
}

void register_client_message_handler(int message_id, size_t size, pt2handler_client fptr)
{
    if (message_id >=256 || message_id <0) {printf("register_client_message_handler: message ID invalid!\n");return;}
    GS_ASSERT(client_handler_array != NULL);
    if (client_handler_array == NULL) return;
    if (client_handler_array[message_id] != NULL) {printf("register_client_message_handler: Reassigning message_id %i !!!\n", message_id);}

    GS_ASSERT(h_client_packet_size != NULL);
    if (h_client_packet_size == NULL) return;
    GS_ASSERT(client_handler_array != NULL);
    if (client_handler_array == NULL) return;

    h_client_packet_size[message_id] = size;
    //printf("StoC: id,size %d,%d\n", message_id, size);
    client_handler_array[message_id] = fptr;
}

void init_message_handler()
{

    #if !NET_STATIC_ARRAYS

    h_client_packet_size = (size_t*) calloc(256, sizeof(size_t));
    h_server_packet_size = (size_t*) calloc(256, sizeof(size_t));

    client_handler_array = (pt2handler_client*) calloc(256, sizeof(pt2handler_server));
    server_handler_array =  (pt2handler_server*) calloc(256, sizeof(pt2handler_server));

    #endif

    for (int i=0;i<256;i++)
    {
        server_handler_array[i] = NULL;
        client_handler_array[i] = NULL;
        h_server_packet_size[i] = 0;
        h_client_packet_size[i] = 0;
    }

}

#if DC_SERVER
int process_packet_messages(char* buff, size_t* n, size_t max_n, ClientID client_id)
#endif
#if DC_CLIENT
int process_packet_messages(char* buff, size_t* n, size_t max_n)
#endif
{

    //printf("*n= %i, max_n= %i \n", *n, max_n);

    size_t size;
    uint8_t message_id;

    size_t read_bytes = 0;

    //static int _in = 0;

    do {
        //UNPACK_uint8_t(&message_id, buff, n);
        unpack_message_id(&message_id, buff, n);
        //GS_ASSERT(message_id >= 0 && message_id < 255); // always true due to data type value range

        //if (IS_CLIENT) printf("pop message: n= %i, message_id= %i \n", _n, message_id);

        //printf("0 n= %i, max_n= %i \n", *n, max_n);

        #if DC_SERVER
        size  = h_server_packet_size[message_id];
        #endif

        #if DC_CLIENT
        size  = h_client_packet_size[message_id];
        #endif

        //printf("%d Receiving packet %d,%d\n", _in++, message_id, size);

        if ((*n)+size-1 > max_n)
        { // > or >= ?
            printf("ERROR! message processor would read past end of packet!\n");

            printf("n= %i, max_n= %i, message_id= %i message_size= %i \n", *n, max_n, message_id, size);
            return -3;
        }


        #if DC_CLIENT
        //remove this!
        if (client_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -4;
        }
        client_handler_array[message_id](buff, *n, &read_bytes);
        #endif

        #if DC_SERVER
        //remove this check
        if (server_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id=%i\n", message_id);
            return -4;
        }
        server_handler_array[message_id](buff, *n, &read_bytes, client_id);
        #endif

        if (read_bytes+1 != size)
        {
            printf("ERROR!: message_id= %i, bytes expected= %i, bytes read=%i\n", message_id, size, read_bytes);
            return -2;
        }

        *n += read_bytes; //works for non fixed sized
        //printf("n= %i, size= %i, read_bytes= %i \n", *n, size, read_bytes);

        //process next message
    } while (*n < max_n);

    //finished procesing messages
    if (*n == max_n)
        return 0;

    //error that should never occur
    if (*n > max_n)
    {   //error, read past buff
        printf("network error!!! Error: read past buffer\n");
        return -1;
    }

    return -5; //should not happen
}

int process_client_map_messages(char* buff, size_t* n, size_t max_n)
{
    #if DC_SERVER
    GS_ASSERT(false);
    printf("process_client_map_messages Error: this should never be called on server\n");
    #endif
    //printf("*n= %i, max_n= %i \n", *n, max_n);
    uint8_t message_id;
    size_t read_bytes;

    do
    {
        unpack_message_id(&message_id, buff, n);

        if (client_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -1;
        }
        client_handler_array[message_id](buff, *n, &read_bytes);

        *n += read_bytes; //works for non fixed sized

        //printf("n= %i, size= %i, read_bytes= %i \n", *n, size, read_bytes);
        //printf("2 n= %i, max_n= %i \n", *n, max_n);

    } while (*n < max_n);

    GS_ASSERT(*n == max_n);
    if (*n != max_n) return -1;
    return 0;
}

#if DC_SERVER
int process_large_messages(char* buff, size_t* n, size_t max_n, ClientID client_id)
#endif
#if DC_CLIENT
int process_large_messages(char* buff, size_t* n, size_t max_n)
#endif
{
    printf("WARNING: process_large_messages, received message on large message channel \n");
    return 0;
}
