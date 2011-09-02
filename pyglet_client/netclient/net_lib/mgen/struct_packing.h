#ifndef net_lib_mgen_struct_packing_h 
#define net_lib_mgen_struct_packing_h 

#include <stdlib.h> 
#include <stdint.h> 
#include "../type_pack.h"

#include "./struct_def.h" 

static inline void pack_struct_test1(unsigned char* buffer, struct test1* s, int* n) { 
	PACK_uint8_t(s->testbyte, buffer, n);
	PACK_float(s->testfloat, buffer, n);
	//*n += sizeof(uint8_t)+sizeof(float)+0;;
}

static inline void unpack_struct_test1(unsigned char* buffer, struct test1* s, int* n) { 
	UNPACK_float(&s->testfloat, buffer, n);
	UNPACK_uint8_t(&s->testbyte, buffer, n);
	//*n += sizeof(uint8_t)+sizeof(float)+0;;
}

static inline void pack_struct_test2(unsigned char* buffer, struct test2* s, int* n) { 
	PACK_int16_t(s->testshort, buffer, n);
	PACK_int32_t(s->testint, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int32_t)+0;;
}

static inline void unpack_struct_test2(unsigned char* buffer, struct test2* s, int* n) { 
	UNPACK_int32_t(&s->testint, buffer, n);
	UNPACK_int16_t(&s->testshort, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int32_t)+0;;
}

static inline void pack_struct_test3(unsigned char* buffer, struct test3* s, int* n) { 
	PACK_uint16_t(s->testushort, buffer, n);
	PACK_uint32_t(s->testuint, buffer, n);
	//*n += sizeof(uint16_t)+sizeof(uint32_t)+0;;
}

static inline void unpack_struct_test3(unsigned char* buffer, struct test3* s, int* n) { 
	UNPACK_uint32_t(&s->testuint, buffer, n);
	UNPACK_uint16_t(&s->testushort, buffer, n);
	//*n += sizeof(uint16_t)+sizeof(uint32_t)+0;;
}

static inline void pack_struct_uni(unsigned char* buffer, struct uni* s, int* n) { 
	PACK_uint8_t(s->t1, buffer, n);
	PACK_int16_t(s->t4, buffer, n);
	PACK_uint16_t(s->t5, buffer, n);
	PACK_int32_t(s->t2, buffer, n);
	PACK_uint32_t(s->t3, buffer, n);
	PACK_float(s->t6, buffer, n);
	//*n += sizeof(uint8_t)+sizeof(int16_t)+sizeof(uint16_t)+sizeof(int32_t)+sizeof(uint32_t)+sizeof(float)+0;;
}

static inline void unpack_struct_uni(unsigned char* buffer, struct uni* s, int* n) { 
	UNPACK_float(&s->t6, buffer, n);
	UNPACK_uint32_t(&s->t3, buffer, n);
	UNPACK_int32_t(&s->t2, buffer, n);
	UNPACK_uint16_t(&s->t5, buffer, n);
	UNPACK_int16_t(&s->t4, buffer, n);
	UNPACK_uint8_t(&s->t1, buffer, n);
	//*n += sizeof(uint8_t)+sizeof(int16_t)+sizeof(uint16_t)+sizeof(int32_t)+sizeof(uint32_t)+sizeof(float)+0;;
}

#endif
