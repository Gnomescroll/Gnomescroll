#ifndef net_lib_type_pack_h
#define net_lib_type_pack_h
/*
types:

uint8_t

int16_t
uint16_t

int32_t
uint32_t

float
*/

// uint8_t
static inline void PACK_uint8_t(uint8_t d, void* buffer, int*n) {


}

static inline void UNPACK_uint8_t(uint8_t d, void* buffer, int*n) {


}

// int16_t
static inline void PACK_int16_t(int16_t d, void* buffer, int*n) {

}

static inline void UNPACK_int16_t(int16_t d, void* buffer, int*n) {

}

// uint16_t
static inline void PACK_uint16_t(uint16_t d, void* buffer, int*n) {

}

static inline void UNPACK_uint16_t(uint16_t d, void* buffer, int*n) {

}

// int32_t
static inline void PACK_int32_t(int32_t d, void* buffer, int*n) {

}

static inline void UNPACK_int32_t(int32_t d, void* buffer, int*n) {

}

// uint32_t
static inline void PACK_uint32_t(uint32_t d, void* buffer, int*n) {

}

static inline void UNPACK_uint32_t(uint32_t d, void* buffer, int*n) {

}
// float

static inline void PACK_float(float d, void* buffer, int*n) {

}

static inline void UNPACK_float(float d, void* buffer, int*n) {

}

#endif
//example

/*
int pack_struct_uni(void* buffer, struct uni* s) {
        int n=0;
        PACK_uint8_t(s->t1, buffer, &n);
        PACK_int16_t(s->t4, buffer, &n);
        PACK_uint16_t(s->t5, buffer, &n);
        PACK_int32_t(s->t2, buffer, &n);
        PACK_uint32_t(s->t3, buffer, &n);
        PACK_int16_t(s->t4, buffer, &n);
        PACK_uint16_t(s->t5, buffer, &n);
        PACK_float(s->t6, buffer, &n);
        return n;
}

void unpack_struct_uni(void* buffer, struct uni* s) {
        int n=0;
        UNPACK_uint8_t(s->t1, buffer, &n);
        UNPACK_int16_t(s->t4, buffer, &n);
        UNPACK_uint16_t(s->t5, buffer, &n);
        UNPACK_int32_t(s->t2, buffer, &n);
        UNPACK_uint32_t(s->t3, buffer, &n);

        UNPACK_float(s->t6, buffer, &n);
        return n;
}
*/
