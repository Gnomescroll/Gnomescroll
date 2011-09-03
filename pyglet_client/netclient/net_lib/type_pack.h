#ifndef net_lib_type_pack_h
#define net_lib_type_pack_h

static inline void PACK_uint8_t(uint8_t d, unsigned char* buffer, int* n) {
        *((uint8_t*)(buffer+*n)) = d;
        *n += sizeof(uint8_t);
}

static inline void UNPACK_uint8_t(uint8_t* d, unsigned char* buffer, int*n) {
        *d = *((uint8_t*)(buffer+*n));
        *n += sizeof(uint8_t);
}


static inline void PACK_int16_t(int16_t d, unsigned char* buffer, int* n) {
        *((int16_t*)(buffer+*n)) = d;
        *n += sizeof(int16_t);
}

static inline void UNPACK_int16_t(int16_t* d, unsigned char* buffer, int*n) {
        *d = *((int16_t*)(buffer+*n));
        *n += sizeof(int16_t);
}


static inline void PACK_uint16_t(uint16_t d, unsigned char* buffer, int* n) {
        *((uint16_t*)(buffer+*n)) = d;
        *n += sizeof(uint16_t);
}

static inline void UNPACK_uint16_t(uint16_t* d, unsigned char* buffer, int*n) {
        *d = *((uint16_t*)(buffer+*n));
        *n += sizeof(uint16_t);
}

static inline void PACK_int32_t(int32_t d, unsigned char* buffer, int* n) {
        *((int32_t*)(buffer+*n)) = d;
        *n += sizeof(int32_t);
}


static inline void UNPACK_int32_t(int32_t* d, unsigned char* buffer, int*n) {
        *d = *((int32_t*)(buffer+*n));
        *n += sizeof(int32_t);
}

static inline void PACK_uint32_t(uint32_t d, unsigned char* buffer, int* n) {
        *((uint32_t*)(buffer+*n)) = d;
        *n += sizeof(uint32_t);
}


static inline void UNPACK_uint32_t(uint32_t* d, unsigned char* buffer, int*n) {
        *d = *((uint32_t*)(buffer+*n));
        *n += sizeof(uint32_t);
}


static inline void PACK_float(float d, unsigned char* buffer, int* n) {
        *((float*)(buffer+*n)) = d;
        *n += sizeof(float);
}

static inline void UNPACK_float(float* d, unsigned char* buffer, int*n) {
        *d = *((float*)(buffer+*n));
        *n += sizeof(float);
}
