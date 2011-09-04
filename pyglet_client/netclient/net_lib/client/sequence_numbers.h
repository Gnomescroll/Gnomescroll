#ifndef net_lib_client_sequence_number
#define net_lib_client_sequence_number

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void init_sequence_numbers();
uint16_t get_next_sequence_number();
void check_for_dropped_packets();
void process_acks(unsigned short seq, unsigned int flag);

#endif
