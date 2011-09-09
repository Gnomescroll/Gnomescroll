
#include "message_handler.h"

int h_packet_size[256];

typedef int (*pt2handler)(unsigned char*, int);
//pt2handler* handler_array[256];

pt2handler handler_array[256] = {NULL};

//int (*handler_array[256])(unsigned char*, int) = {NULL};

int default_handler_function(unsigned char* buff, int n) {
    //printf("ERROR!!\nNo handler for message_id= %i\n", message_id);
    printf("ERROR! No message handler assigned for this message id!\n");
    return -1;
}

void init_message_handler() {
    int i;
    for(i=0;i<256;i++) {
        handler_array[i] = &default_handler_function;
        h_packet_size[i] = -1;
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

int pop_message(unsigned char* buff, int *n, int max_n) {
    int bytes, size;
    uint8_t message_id;
    printf("Reading message id from byte %i\n", *n);
    UNPACK_uint8_t(&message_id, buff, n);
    size  = h_packet_size[message_id];

    printf("processing: msg= %i len=%i byte %i of %i\n", message_id,size, *n, max_n);

    if(*n-1+size >= max_n) {
        printf("ERROR! message processor would read past end of packet!\n");
        return 0;
    }
    bytes = handler_array[message_id](buff, *n);

    if(bytes == -1) {
        return -1;
    }
    if(bytes != size) {
        printf("ERROR!: message_id= %i, bytes expected= %i, bytes read=%i\n", message_id, size, bytes);
        return 0;
    }
    *n += size;

    //printf("n=%i, max_n=%i \n", *n, max_n);
    if(*n == max_n) { return 0; }
    if(*n < max_n) { return -2; }
    if(*n > max_n) { return -3; }
}

void process_packet_messages(unsigned char* buff, int n, int max_n) {

    int i=0;
    int condition;
    while(1) {
        condition = pop_message(buff, &n, max_n);
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

