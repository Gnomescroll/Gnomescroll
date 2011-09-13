#ifndef net_lib_mgen_struct_packing_h 
#define net_lib_mgen_struct_packing_h 

#include <stdlib.h> 
#include <stdint.h> 
#include "../type_pack.h"

#include "./struct_def.h" 

static inline void pack_struct_NET_agent_snapshot(unsigned char* buffer, struct NET_agent_snapshot* s, int* n) { 
	PACK_int16_t(s->id, buffer, n);
	PACK_int16_t(s->tick, buffer, n);
	PACK_float(s->x, buffer, n);
	PACK_float(s->y, buffer, n);
	PACK_float(s->z, buffer, n);
	PACK_float(s->vx, buffer, n);
	PACK_float(s->vy, buffer, n);
	PACK_float(s->vz, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+0;;
}

static inline void unpack_struct_NET_agent_snapshot(unsigned char* buffer, struct NET_agent_snapshot* s, int* n) { 
	UNPACK_float(&s->vz, buffer, n);
	UNPACK_float(&s->vy, buffer, n);
	UNPACK_float(&s->vx, buffer, n);
	UNPACK_float(&s->z, buffer, n);
	UNPACK_float(&s->y, buffer, n);
	UNPACK_float(&s->x, buffer, n);
	UNPACK_int16_t(&s->tick, buffer, n);
	UNPACK_int16_t(&s->id, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+sizeof(float)+0;;
}

static inline void pack_struct_NET_agent_control_state(unsigned char* buffer, struct NET_agent_control_state* s, int* n) { 
	PACK_int16_t(s->id, buffer, n);
	PACK_int16_t(s->tick, buffer, n);
	PACK_uint8_t(s->seq, buffer, n);
	PACK_uint8_t(s->theta, buffer, n);
	PACK_uint8_t(s->phi, buffer, n);
	PACK_uint32_t(s->cs, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t)+0;;
}

static inline void unpack_struct_NET_agent_control_state(unsigned char* buffer, struct NET_agent_control_state* s, int* n) { 
	UNPACK_uint32_t(&s->cs, buffer, n);
	UNPACK_uint8_t(&s->phi, buffer, n);
	UNPACK_uint8_t(&s->theta, buffer, n);
	UNPACK_uint8_t(&s->seq, buffer, n);
	UNPACK_int16_t(&s->tick, buffer, n);
	UNPACK_int16_t(&s->id, buffer, n);
	//*n += sizeof(int16_t)+sizeof(int16_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint32_t)+0;;
}

#endif
