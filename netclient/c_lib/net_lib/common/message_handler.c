

int h_packet_size[256];

typedef int (*pt2handler)(int, unsigned char*);

pt2handler handler_array[256];

void init_message_handler() {
    int i;
    for(i=0;i<256;i++) {
        handler_array[i] = &default_handler_function;
    }

}

void register_handler(int message_id, int size, pt2handler fptr) {



}


void default_handler_function(int message_id, unsigned char* buff) {
    printf()
}

void char*
