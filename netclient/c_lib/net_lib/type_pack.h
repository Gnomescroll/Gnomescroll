#pragma once


//always inline

/*
__attribute((always_inline)); will force gcc to inline
*/
static inline void pack_message_id(int message_id, char* buff, int*buff_n)  __attribute((always_inline));
static inline void unpack_message_id(int* message_id, char* buff, int*buff_n)  __attribute((always_inline));

static inline void pack_float(float* x, char* buff, int*buff_n, bool pack) __attribute((always_inline));

static inline void pack_32(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u32(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u32_t(uint32_t* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));

static inline void pack_16(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u16(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u16(uint16_t* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));

static inline void pack_8(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u8(int* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));
static inline void pack_u8(uint8_t* x, char* buff, int* buff_n, bool pack) __attribute((always_inline));

//combined pack/unpack


static inline void pack_message_id(int message_id, char* buff, int*buff_n) {
    *((uint8_t*)(buff+*buff_n)) = message_id;
    *buff_n += sizeof(uint8_t);
}

static inline void unpack_message_id(int* message_id, char* buff, int*buff_n) {
    *message_id = *((uint8_t*)(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}

static inline void pack_float(float* x, char* buff, int*buff_n, bool pack)
{
    if(pack == true) *((float*)(buff+*buff_n)) = *x;
    if(pack == false) *x = *((float*)(buff+*buff_n));
    *buff_n += sizeof(float);
}

static inline void pack_32(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int32_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((int32_t*)(buff+*buff_n));
    *buff_n += sizeof(int32_t);
}

static inline void pack_u32(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += sizeof(uint32_t);
}

static inline void pack_u32_t(uint32_t* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x;
    if(pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += sizeof(uint32_t);
}

static inline void pack_16(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int16_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((int16_t*)(buff+*buff_n));
    *buff_n += sizeof(int16_t);
}

static inline void pack_u16(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint16_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint16_t*)(buff+*buff_n));
    *buff_n += sizeof(uint16_t);
}

static inline void pack_u16(uint16_t* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint16_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint16_t*)(buff+*buff_n));
    *buff_n += sizeof(uint16_t);
}

static inline void pack_8(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((int8_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((int8_t*)(buff+*buff_n));
    *buff_n += sizeof(int8_t);
}

static inline void pack_u8(int* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint8_t*)(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}

static inline void pack_u8(uint8_t* x, char* buff, int* buff_n, bool pack)
{
    if(pack == true)    *((uint8_t*)(buff+*buff_n)) = *x; 
    if(pack == false)   *x = *((uint8_t*)(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}


//pop variables into a new one
static inline uint8_t UPACK_uint8_t(char* buffer, int*n) {
        uint8_t d = *((uint8_t*)(buffer+*n));
        *n += sizeof(uint8_t);
        return d;
}

static inline int16_t UPACK_uint16_t(char* buffer, int*n) {
        uint16_t d = *((uint16_t*)(buffer+*n));
        *n += sizeof(uint16_t);
        return d;
}

static inline int32_t UPACK_uint32_t(char* buffer, int*n) {
        int16_t d = *((int32_t*)(buffer+*n));
        *n += sizeof(int32_t);
        return d;
}

static inline float UPACK_float(char* buffer, int*n) {
        float d = *((float*)(buffer+*n));
        *n += sizeof(float);
        return d;
}

// other stuff

static inline void PACK_uint8_t(uint8_t d, char* buffer, int* n) {
        *((uint8_t*)(buffer+*n)) = d;
        *n += sizeof(uint8_t);
}

static inline void UNPACK_uint8_t(uint8_t* d, char* buffer, int*n) {
        *d = *((uint8_t*)(buffer+*n));
        *n += sizeof(uint8_t);
}


static inline void PACK_int16_t(int16_t d, char* buffer, int* n) {
        *((int16_t*)(buffer+*n)) = d;
        *n += sizeof(int16_t);
}

static inline void UNPACK_int16_t(int16_t* d, char* buffer, int*n) {
        *d = *((int16_t*)(buffer+*n));
        *n += sizeof(int16_t);
}


static inline void PACK_uint16_t(uint16_t d, char* buffer, int* n) {
        *((uint16_t*)(buffer+*n)) = d;
        *n += sizeof(uint16_t);
}

static inline void UNPACK_uint16_t(uint16_t* d, char* buffer, int*n) {
        *d = *((uint16_t*)(buffer+*n));
        *n += sizeof(uint16_t);
}

static inline void PACK_int32_t(int32_t d, char* buffer, int* n) {
        *((int32_t*)(buffer+*n)) = d;
        *n += sizeof(int32_t);
}


static inline void UNPACK_int32_t(int32_t* d, char* buffer, int*n) {
        *d = *((int32_t*)(buffer+*n));
        *n += sizeof(int32_t);
}

static inline void PACK_uint32_t(uint32_t d, char* buffer, int* n) {
        *((uint32_t*)(buffer+*n)) = d;
        *n += sizeof(uint32_t);
}


static inline void UNPACK_uint32_t(uint32_t* d, char* buffer, int*n) {
        *d = *((uint32_t*)(buffer+*n));
        *n += sizeof(uint32_t);
}


static inline void PACK_float(float d, char* buffer, int* n) {
        *((float*)(buffer+*n)) = d;
        *n += sizeof(float);
}

static inline void UNPACK_float(float* d, char* buffer, int*n) {
        *d = *((float*)(buffer+*n));
        *n += sizeof(float);
}
