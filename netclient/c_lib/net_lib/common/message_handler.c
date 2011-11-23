
#include "message_handler.h"

int h_packet_size[256];

typedef void (*pt2handler)(char*, int, int* read_bytes, int client_id);
//pt2handler* handler_array[256];

pt2handler handler_array[256] = {NULL};

pt2handler client_handler_array[256] = {NULL};
pt2handler server_handler_array[256] = {NULL};

//int (*handler_array[256])(char*, int) = {NULL};

void default_handler_function(char* buff, int n, int* read_bytes, int client_id) {
    //printf("ERROR!!\nNo handler for message_id= %i\n", message_id);
    printf("ERROR! No message handler assigned for this message id!\n");
    *read_bytes = -1;
}

//base

//typedef const void (*_pt2handler)(char*, int, int*);
void register_server_message_handler(int message_id, int size, pt2handler fptr) {
    if(message_id >=256 || message_id <0) {printf("register_server_message_handler: message ID invalid!\n");return;}
    if(server_handler_array[message_id] != NULL) {printf("register_server_message_handler: reassigning message_id %i !!!\n", message_id);}
    h_packet_size[message_id] = size;
    server_handler_array[message_id] = fptr;
}

void register_client_message_handler(int message_id, int size, pt2handler fptr) {
    if(message_id >=256 || message_id <0) {printf("register_client_message_handler: message ID invalid!\n");return;}
    if(client_handler_array[message_id] != NULL) {printf("register_client_message_handler: Reassigning message_id %i !!!\n", message_id);}
    h_packet_size[message_id] = size;
    client_handler_array[message_id] = fptr;
}

void init_message_handler() {
    int i;
    for(i=0;i<256;i++) {
        server_handler_array[i] = NULL;
        client_handler_array[i] = NULL;
        h_packet_size[i] = -1;
    }

}

int pop_message(char* buff, int *n, int max_n, int client_id) {

    if(*n == max_n) {
        //printf("Processed Empty Packet\n");
        return 0;
    }
    int size;
    uint8_t message_id;

    int _n = *n;
    UNPACK_uint8_t(&message_id, buff, &_n);
    size  = h_packet_size[message_id];

    if(*n-1+size > max_n) { // > or >= ?
        printf("ERROR! message processor would read past end of packet!\n");
        return 0;
    }

#ifdef DC_CLIENT
    if(client_handler_array[message_id] == NULL) {
        printf("message_handler error: no handler for message_id=%i\n", message_id);
        return -4;
    }
    int read_bytes = -1;
    client_handler_array[message_id](buff, *n, &read_bytes, client_id);
#endif

#ifdef DC_SERVER
    if(server_handler_array[message_id] == NULL) {
        printf("message_handler error: no handler for message_id=%i\n", message_id);
        return -5;
    }
    int read_bytes = -1;
    server_handler_array[message_id](buff, *n, &read_bytes, client_id);
#endif

    if(read_bytes == -1) {
        return -1;
    }
    if(read_bytes != size) {
        printf("ERROR!: message_id= %i, bytes expected= %i, bytes read=%i\n", message_id, size, read_bytes);
        return 0;
    }
    *n += size;

    //printf("n=%i, max_n=%i \n", *n, max_n);

    if(*n < max_n) { return 1; }        //more messages
    if(*n == max_n) { return 0; }       //finished
    if(*n > max_n) {                    //error, read past buff 
        printf("network error!!! Error: read past buffer\n");
        return -3; 
    }
    return -4;
}

void process_packet_messages(char* buff, int n, int max_n, int client_id) {
/*
    if(n!=max_n) {
        printf("packet is %i, %i bytes\n", n,max_n);
    }
*/
    int i=0;
    int condition;
    while(n != max_n) {
        condition = pop_message(buff, &n, max_n, client_id);
        if(condition == 1) continue;
        if(condition < 0  ) {
            printf("Packet processing terminated with error: %i\n", condition);
            break;
        }
        if(condition == 0) {
            //printf("Packet processing terminated with success\n");
            break;
        }

        i++;
    };
    //printf("Packet Finished: processed %i messages\n", i);

}

