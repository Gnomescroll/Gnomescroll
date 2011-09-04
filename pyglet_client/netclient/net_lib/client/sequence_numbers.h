#ifndef net_lib_client_sequence_number
#define net_lib_client_sequence_number

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct packet_sequence {
    int seq;
    int ack;
    //int active;
    //int tick; //better than sequence for determining dropped packets
};


struct Pseq {
    struct packet_sequence packet_sequence_buffer[64];
    //int packet_sequence_buffer_n = 0;
    int packet_sequence_number;
};

void init_sequence_numbers(struct Pseq* sq);
uint16_t get_next_sequence_number(struct Pseq* sq);
void check_for_dropped_packets(struct Pseq* sq);
void process_acks(struct Pseq* sq, unsigned short seq, unsigned int flag);

#endif
