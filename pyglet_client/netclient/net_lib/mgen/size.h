#ifndef net_lib_mgen_size_h
#define net_lib_mgen_size_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../type_pack.h"

void DEBUG_net_message_size() {
	printf("ramsize, netsize\n");
	printf("sizeof(test1)=%i, %i\n", sizeof(struct test1),sizeof(uint8_t)+sizeof(float)+0);
	printf("sizeof(test2)=%i, %i\n", sizeof(struct test2),sizeof(int16_t)+sizeof(int32_t)+0);
	printf("sizeof(test3)=%i, %i\n", sizeof(struct test3),sizeof(uint16_t)+sizeof(uint32_t)+0);
	printf("sizeof(uni)=%i, %i\n", sizeof(struct uni),sizeof(uint8_t)+sizeof(int16_t)+sizeof(uint16_t)+sizeof(int32_t)+sizeof(uint32_t)+sizeof(float)+0);
}
#endif
