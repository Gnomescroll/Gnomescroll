
#include "message_handler.h"

#include <net_lib/global.hpp>

static int h_client_packet_size[256]; 
static int h_server_packet_size[256]; 

typedef void (*pt2handler)(char*, int, int* read_bytes, int client_id);

pt2handler handler_array[256] = {NULL};

pt2handler client_handler_array[256] = {NULL};
pt2handler server_handler_array[256] = {NULL};

//should disconnect client
void default_handler_function(char* buff, int n, int* read_bytes, int client_id) {
    //printf("ERROR!!\nNo handler for message_id= %i\n", message_id);
    printf("ERROR! No message handler assigned for this message id!\n");
    *read_bytes = -1;
}

void register_server_message_handler(int message_id, int size, pt2handler fptr) {
    if(message_id > 255 || message_id <0) {printf("register_server_message_handler: message ID invalid!\n");return;}
    if(server_handler_array[message_id] != NULL) {printf("register_server_message_handler: reassigning message_id %i !!!\n", message_id);}

    h_server_packet_size[message_id] = size;
    server_handler_array[message_id] = fptr;
}

void register_client_message_handler(int message_id, int size, pt2handler fptr) {
    if(message_id >=256 || message_id <0) {printf("register_client_message_handler: message ID invalid!\n");return;}
    if(client_handler_array[message_id] != NULL) {printf("register_client_message_handler: Reassigning message_id %i !!!\n", message_id);}

    h_client_packet_size[message_id] = size;
    client_handler_array[message_id] = fptr;
}

void init_message_handler() {
    int i;
    for(i=0;i<256;i++) {
        server_handler_array[i] = NULL;
        client_handler_array[i] = NULL;
        h_server_packet_size[i] = -1;
        h_client_packet_size[i] = -1;
    }

}

int process_packet_messages(char* buff, int *n, int max_n, int client_id) 
{

    //printf("*n= %i, max_n= %i \n", *n, max_n);

    int size;
    int message_id;

    //int _n = *n;
    int read_bytes;

PROCESS:

    //UNPACK_uint8_t(&message_id, buff, n);
    unpack_message_id(&message_id, buff, n);
    //if(IS_CLIENT) printf("pop message: n= %i, message_id= %i \n", _n, message_id);

    //printf("0 n= %i, max_n= %i \n", *n, max_n);

#ifdef DC_SERVER
        size  = h_server_packet_size[message_id];
#endif

#ifdef DC_CLIENT
        size  = h_client_packet_size[message_id];
#endif

    if(*n+size-1 > max_n) 
    { // > or >= ?
        printf("ERROR! message processor would read past end of packet!\n");

        printf("n= %i, max_n= %i \n", *n, max_n);
        return 0;
    }

    
#ifdef DC_CLIENT
        //remove this!
        if(client_handler_array[message_id] == NULL) {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -4;
        }
        client_handler_array[message_id](buff, *n, &read_bytes, client_id);
#endif

#ifdef DC_SERVER
        //remove this check
        if(server_handler_array[message_id] == NULL) 
        {
            printf("message_handler error: no handler for message_id=%i\n", message_id);
            return -5;
        }
        server_handler_array[message_id](buff, *n, &read_bytes, client_id);
#endif

    //printf("1 n= %i, max_n= %i \n", *n, max_n);

    if(read_bytes+1 != size) 
    {
        printf("ERROR!: message_id= %i, bytes expected= %i, bytes read=%i\n", message_id, size, read_bytes);
        return 0;
    }

    *n += read_bytes; //works for non fixed sized
    //printf("n= %i, size= %i, read_bytes= %i \n", *n, size, read_bytes);

    //printf("2 n= %i, max_n= %i \n", *n, max_n);
    //process next message
    if(*n < max_n) 
    { 
        goto PROCESS;     
    }
    //finished procesing messages
    if(*n == max_n) 
    { 
        return 0; 
    }       

    //error that should never occur
    if(*n > max_n) 
    {                    
        //error, read past buff 
        printf("network error!!! Error: read past buffer\n");
        return -1; 
    }

    return 0; //should not happen
}


/*
    Client to server is safe
*/
int process_client_map_messages(char* buff, int *n, int max_n, int client_id) 
{
#ifdef DC_SERVER
    printf("process_client_map_messages Error: this should never be called on server\n");
#endif
    //printf("*n= %i, max_n= %i \n", *n, max_n);

    int message_id;

    int read_bytes;

    MAP_MESSAGE_LOOP:

    unpack_message_id(&message_id, buff, n);

    if(client_handler_array[message_id] == NULL) 
    {
        printf("message_handler error: no handler for message_id= %i\n", message_id);
        return -1;
    }
    client_handler_array[message_id](buff, *n, &read_bytes, client_id);

    *n += read_bytes; //works for non fixed sized
    
    //printf("n= %i, size= %i, read_bytes= %i \n", *n, size, read_bytes);
    //printf("2 n= %i, max_n= %i \n", *n, max_n);

    if(*n < max_n) goto MAP_MESSAGE_LOOP;     

    //finished procesing messages
    if(*n == max_n) 
    { 
        return 0; 
    }       

    //error that should never occur
    if(*n > max_n) 
    {                    
        //error, read past buff 
        printf("process_client_map_messages: network error!!! Error: read past buffer\n");
        return -1; 
    }

    return 0; //should not happen
}

int process_large_messages(char* buff, int *n, int max_n, int client_id)
{
    printf("WARNING: process_large_messages, received message on large message channel \n");
    return 0;
}
