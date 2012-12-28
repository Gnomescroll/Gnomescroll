#pragma once

#include <stdint.h>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/common/color.hpp>

//always inline

/*
__attribute__((always_inline)); will force gcc to inline
*/
inline void pack_message_id(uint8_t message_id, char* buff, unsigned int* buff_n)  __attribute__((always_inline));
inline void unpack_message_id(uint8_t* message_id, char* buff, unsigned int* buff_n)  __attribute__((always_inline));

inline void pack_float(float* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_vec3(struct Vec3* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
inline void pack_color(Color* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_64(int64_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
inline void pack_u64(uint64_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_32(int32_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
inline void pack_u32(uint32_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_16(int16_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
inline void pack_u16(uint16_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_8(int8_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));
inline void pack_u8(uint8_t* x, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

inline void pack_string(char* str, unsigned int len, char* buff, unsigned int* buff_n, bool pack) __attribute__((always_inline));

//use pack_string


//combined pack/unpack


inline void pack_message_id(uint8_t message_id, char* buff, unsigned int* buff_n)
{
    *((uint8_t*)(buff+*buff_n)) = message_id;
    *buff_n += (unsigned int)sizeof(uint8_t);
}

inline void unpack_message_id(uint8_t* message_id, char* buff, unsigned int* buff_n)
{
    *message_id = *((uint8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint8_t);
}

inline void pack_float(float* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true) *((float*)(buff+*buff_n)) = *x;
    if (pack == false) *x = *((float*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(float);
}

inline void pack_vec3(struct Vec3* x, char* buff, unsigned int* buff_n, bool pack)
{
    pack_float(&(x->x), buff, buff_n, pack);
    pack_float(&(x->y), buff, buff_n, pack);
    pack_float(&(x->z), buff, buff_n, pack);
}

inline void pack_color(Color* x, char* buff, unsigned int* buff_n, bool pack)
{
    pack_u8(&(x->r), buff, buff_n, pack);
    pack_u8(&(x->g), buff, buff_n, pack);
    pack_u8(&(x->b), buff, buff_n, pack);
}

inline void pack_64(int64_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int64_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((int64_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int64_t);
}

inline void pack_u64(uint64_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint64_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((uint64_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint64_t);
}

inline void pack_32(int32_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int32_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((int32_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int32_t);
}

inline void pack_u32(uint32_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint32_t*)(buff+*buff_n)) = *x;
    if (pack == false)   *x = *((uint32_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint32_t);
}

inline void pack_16(int16_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int16_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((int16_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int16_t);
}

inline void pack_u16(uint16_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint16_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((uint16_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint16_t);
}

inline void pack_8(int8_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((int8_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((int8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(int8_t);
}

inline void pack_u8(uint8_t* x, char* buff, unsigned int* buff_n, bool pack)
{
    if (pack == true)    *((uint8_t*)(buff+*buff_n)) = *x; 
    if (pack == false)   *x = *((uint8_t*)(buff+*buff_n));
    *buff_n += (unsigned int)sizeof(uint8_t);
}

inline void pack_string(char* str, unsigned int len, char* buff, unsigned int* buff_n, bool pack)
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
