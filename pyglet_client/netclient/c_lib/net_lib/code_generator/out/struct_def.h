#ifndef net_lib_mgen_struct_def_h 
#define net_lib_mgen_struct_def_h 

#include <stdlib.h> 
#include <stdint.h> 
#include "../type_pack.h"

struct test1 { 
	uint8_t testbyte;
	float testfloat;
};

struct test2 { 
	int16_t testshort;
	int32_t testint;
};

struct test3 { 
	uint16_t testushort;
	uint32_t testuint;
};

struct uni { 
	uint8_t t1;
	int16_t t4;
	uint16_t t5;
	int32_t t2;
	uint32_t t3;
	float t6;
};

#endif
