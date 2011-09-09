
#include "message_handler.h"

int h_packet_size[256];

//typedef int (*pt2handler)(int, unsigned char*);
pt2handler handler_array[256];

void default_handler_function(int message_id, unsigned char* buff) {
    printf("ERROR!!\nNo handler for message_id= %i\n", message_id);
    return;
}

void init_message_handler() {
    int i;
    for(i=0;i<256;i++) {
        handler_array[i] = &default_handler_function;
    }

}

void register_message_handler(int message_id, int size, pt2handler fptr) {

    if(message_id >=256 || message_id <0) {
        printf("Message ID invalid!\n");
        return;
    }

    if(handler_array[message_id] != &default_handler_function) {
        printf("Reassigning message_id %i !!!\n", message_id);
    }
    h_packet_size[message_id] = size;
    handler_array[message_id] = fptr;
}


void process_packet_messages(unsigned char* buff, int *n, int max_n) {

int i=0;
    while(pop_message(unsigned char* buff, int *n, int max_n) == 1) {
        i++;
    };
    printf("Packet Finished: processed %i messages\n", i);

}

int pop_message(unsigned char* buff, int *n, int max_n) {
    int bytes, size;
    size  = h_packet_size[message_id];
    uint8_t message_id;
    UNPACK_uint8_t(&message_id, buff, n);


    printf("processing: msg= %i len=%i byte %i of %i\n", message_id,size, *n-1, max_n);

    if(n+size > max_n) {
        printf("ERROR! message processor would read past end of packet!\n");
        return 0;
    }
    byte = handler_array[message_id](buff);

    if(bytes != size) {
        printf("ERROR!: message_id= %i, bytes expected= %i, bytes read=%i\n", message_id, size, bytes);
    }
    *n += size;

    printf("n=%i, max_n=%i \n", *n, max_n);
    if(*n < max_n) {
        return 1;
    } else {
        return 0;
    }
}
