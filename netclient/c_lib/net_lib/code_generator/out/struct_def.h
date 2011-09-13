#ifndef net_lib_mgen_struct_def_h 
#define net_lib_mgen_struct_def_h 

#include <stdlib.h> 
#include <stdint.h> 
#include "../type_pack.h"

struct NET_agent_snapshot { 
	int16_t id;
	int16_t tick;
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
};

struct NET_agent_control_state { 
	int16_t id;
	int16_t tick;
	uint8_t seq;
	uint8_t theta;
	uint8_t phi;
	uint32_t cs;
};

#endif
