#ifndef net_lib_mgen_size_h 
#define net_lib_mgen_size_h 

#include <stdlib.h> 
#include <stdint.h> 
#include "../type_pack.h"

void DEBUG_net_message_size() { 
	printf("ramsize, netsize\n");
	printf("sizeof(NET_agent_snapshot)=%i, %i\n", sizeof(struct NET_agent_snapshot),sizeof(int16_t)+sizeof(int16_t)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+0);
	printf("sizeof(NET_agent_control_state)=%i, %i\n", sizeof(struct NET_agent_control_state),sizeof(int16_t)+sizeof(int16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t)+0);
}
#endif
