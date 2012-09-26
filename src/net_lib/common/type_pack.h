#pragma once

#include <stdint.h>
#include <c_lib/physics/vec3.hpp>

//always inline

/*
__attribute__((always_inline)); will force gcc to inline
*/
static inline void pack_message_id(uint8_t message_id, char* buff, unsigned int* buff_n)  __attribute__((always_inline));
static inline void unpack_message_id(uint8_t* message_id, char* buff, unsigned int* buff_n)  __attribute__((always_inline));

static inline void pack_float(float* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_vec3(struct Vec3* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_64(int64_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
static inline void pack_u64(uint64_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_32(int32_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
static inline void pack_u32(uint32_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_16(int16_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
static inline void pack_u16(uint16_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_8(int8_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
static inline void pack_u8(uint8_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

static inline void pack_string(char* str, unsigned int len, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

//use pack_string


//combined pack/unpack


static inline void pack_message_id(uint8_t message_id, char* buff, unsigned int* buff_n)
{
    *((uint8_t*)(buff+*buff_n)) = message_id;
    *buff_n += (unsigned int)sizeof(uint8_t);
}

static inline void unpack_message_id(uint8_t* message_id, char* buff, unsigned int* buff_n)
{
    *message_id = *((uint8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint8_t);
}

static inline void pack_float(float* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true) *((float*)(buff+*buff_n)) = *x;
    if (pack == false) *x = *((float*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(float);
}

static inline void pack_vec3(struct Vec3* x, char* buff, unsigned int* buff_n, bool pack)
{
    pack_float(&(x->x), buff, buff_n, pack);
    pack_float(&(x->y), buff, buff_n, pack);
    pack_float(&(x->z), buff, buff_n, pack);
}

static inline void pack_64(int64_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int64_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((int64_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int64_t);
}

static inline void pack_u64(uint64_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint64_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((uint64_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint64_t);
}

static inline void pack_32(int32_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int32_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((int32_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int32_t);
}

static inline void pack_u32(uint32_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint32_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint32_t);
}

static inline void pack_16(int16_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int16_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((int16_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int16_t);
}

static inline void pack_u16(uint16_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint16_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((uint16_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint16_t);
}

static inline void pack_8(int8_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int8_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((int8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int8_t);
}

static inline void pack_u8(uint8_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint8_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((uint8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint8_t);
}

static inline void pack_string(char* str, unsigned int len, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)
    {
        memcpy(buff+*buff_n, str, len);
        *buff_n += len;
    }
    else
    {
        memcpy(str, buff+*buff_n, len);
        *buff_n += len;
        str[len-1] = '\0';
    }
}

//pop variables into a new one
static inline uint8_t UPACK_uint8_t(char* buffer, int*n) {
        uint8_t d = *((uint8_t*)(buffer+*n));
        *n += (int)sizeof(uint8_t);
        return d;
}

static inline int16_t UPACK_uint16_t(char* buffer, int*n) {
        uint16_t d = *((uint16_t*)(buffer+*n));
        *n += (int)sizeof(uint16_t);
        return d;
}

static inline int32_t UPACK_uint32_t(char* buffer, int*n) {
        uint32_t d = *((uint32_t*)(buffer+*n));
        *n += (int)sizeof(uint32_t);
        return d;
}

static inline float UPACK_float(char* buffer, int*n) {
        float d = *((float*)(buffer+*n));
        *n += (int)sizeof(float);
        return d;
}

// other stuff

static inline void PACK_uint8_t(uint8_t d, char* buffer, int* n) {
        *((uint8_t*)(buffer+*n)) = d;
        *n += (int)sizeof(uint8_t);
}

static inline void UNPACK_uint8_t(uint8_t* d, char* buffer, int*n) {
        *d = *((uint8_t*)(buffer+*n));
        *n += (int)sizeof(uint8_t);
}


static inline void PACK_int16_t(int16_t d, char* buffer, int* n) {
        *((int16_t*)(buffer+*n)) = d;
        *n += (int)sizeof(int16_t);
}

static inline void UNPACK_int16_t(int16_t* d, char* buffer, int*n) {
        *d = *((int16_t*)(buffer+*n));
        *n += (int)sizeof(int16_t);
}


static inline void PACK_uint16_t(uint16_t d, char* buffer, int* n) {
        *((uint16_t*)(buffer+*n)) = d;
        *n += (int)sizeof(uint16_t);
}

static inline void UNPACK_uint16_t(uint16_t* d, char* buffer, int*n) {
        *d = *((uint16_t*)(buffer+*n));
        *n += (int)sizeof(uint16_t);
}

static inline void PACK_int32_t(int32_t d, char* buffer, int* n) {
        *((int32_t*)(buffer+*n)) = d;
        *n += (int)sizeof(int32_t);
}


static inline void UNPACK_int32_t(int32_t* d, char* buffer, int*n) {
        *d = *((int32_t*)(buffer+*n));
        *n += (int)sizeof(int32_t);
}

static inline void PACK_uint32_t(uint32_t d, char* buffer, int* n) {
        *((uint32_t*)(buffer+*n)) = d;
        *n += (int)sizeof(uint32_t);
}


static inline void UNPACK_uint32_t(uint32_t* d, char* buffer, int*n) {
        *d = *((uint32_t*)(buffer+*n));
        *n += (int)sizeof(uint32_t);
}


static inline void PACK_float(float d, char* buffer, int* n) {
        *((float*)(buffer+*n)) = d;
        *n += (int)sizeof(float);
}

static inline void UNPACK_float(float* d, char* buffer, int*n) {
        *d = *((float*)(buffer+*n));
        *n += (int)sizeof(float);
}
