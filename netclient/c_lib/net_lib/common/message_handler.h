#ifndef net_lib_type_pack_h
#define net_lib_type_pack_h

#include "type_pack.h"

typedef int (*pt2handler)(int, unsigned char*);


//void default_handler_function(int message_id, unsigned char* buff);

void init_message_handler();
void register_message_handler(int message_id, int size, pt2handler fptr);
void process_packet_messages(unsigned char* buff, int *n, int max_n);
int pop_message(unsigned char* buff, int *n, int max_n);


#endif
