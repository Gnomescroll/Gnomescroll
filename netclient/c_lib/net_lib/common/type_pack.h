#ifndef net_lib_type_pack_h
#define net_lib_type_pack_h

//pop variables into a new one
static inline uint8_t UPACK_uint8_t(unsigned char* buffer, int*n) {
        uint8_t d = *((uint8_t*)(buffer+*n));
        *n += sizeof(uint8_t);
        return d;
}

static inline int16_t UPACK_uint16_t(unsigned char* buffer, int*n) {
        uint16_t d = *((uint16_t*)(buffer+*n));
        *n += sizeof(uint16_t);
        return d;
}

static inline int32_t UPACK_uint32_t(unsigned char* buffer, int*n) {
        int16_t d = *((int32_t*)(buffer+*n));
        *n += sizeof(int32_t);
        return d;
}

static inline float UPACK_float(unsigned char* buffer, int*n) {
        float d = *((float*)(buffer+*n));
        *n += sizeof(float);
        return d;
}

// other stuff

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

#endif
