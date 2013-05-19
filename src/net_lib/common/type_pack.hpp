#pragma once

#include <stdint.h>
#include <c_lib/physics/vec3.hpp>
#include <c_lib/common/color.hpp>

ALWAYS_INLINE void pack_message_id(uint8_t message_id, char* buff, size_t* buff_n);
ALWAYS_INLINE void unpack_message_id(uint8_t* message_id, char* buff, size_t* buff_n);

ALWAYS_INLINE void pack_float(float* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_vec3(struct Vec3* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_vec3i_u8(struct Vec3i* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_vec3i_u16(struct Vec3i* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_map_position(struct Vec3i* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_color(Color* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_64(int64_t* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_u64(uint64_t* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_32(int32_t* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_u32(uint32_t* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_16(int16_t* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_u16(uint16_t* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_8(int8_t* x, char* buff, size_t* buff_n, bool pack);
ALWAYS_INLINE void pack_u8(uint8_t* x, char* buff, size_t* buff_n, bool pack);

ALWAYS_INLINE void pack_string(char* str, size_t len, char* buff, size_t* buff_n, bool pack);

// TO BRANON: The reinterpret_casts get rid of possible -Wcast-align problems

ALWAYS_INLINE void pack_message_id(uint8_t message_id, char* buff, size_t* buff_n)
{
    *(reinterpret_cast<uint8_t*>(buff+*buff_n)) = message_id;
    *buff_n += sizeof(uint8_t);
}

ALWAYS_INLINE void unpack_message_id(uint8_t* message_id, char* buff, size_t* buff_n)
{
    *message_id = *(reinterpret_cast<uint8_t*>(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}

ALWAYS_INLINE void pack_float(float* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<float*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<float*>(buff+*buff_n));
    *buff_n += sizeof(float);
}

ALWAYS_INLINE void pack_vec3(struct Vec3* x, char* buff, size_t* buff_n, bool pack)
{
    pack_float(&(x->x), buff, buff_n, pack);
    pack_float(&(x->y), buff, buff_n, pack);
    pack_float(&(x->z), buff, buff_n, pack);
}

ALWAYS_INLINE void pack_vec3i_u8(struct Vec3i* x, char* buff, size_t* buff_n, bool pack)
{
    if (!pack)
    {
        x->x = 0;
        x->y = 0;
        x->z = 0;
    }
    pack_u8(reinterpret_cast<uint8_t*>(&x->x), buff, buff_n, pack);
    pack_u8(reinterpret_cast<uint8_t*>(&x->y), buff, buff_n, pack);
    pack_u8(reinterpret_cast<uint8_t*>(&x->z), buff, buff_n, pack);
}

ALWAYS_INLINE void pack_vec3i_u16(struct Vec3i* x, char* buff, size_t* buff_n, bool pack)
{
    if (!pack)
    {
        x->x = 0;
        x->y = 0;
        x->z = 0;
    }
    pack_u16(reinterpret_cast<uint16_t*>(&x->x), buff, buff_n, pack);
    pack_u16(reinterpret_cast<uint16_t*>(&x->y), buff, buff_n, pack);
    pack_u16(reinterpret_cast<uint16_t*>(&x->z), buff, buff_n, pack);
}

ALWAYS_INLINE void pack_map_position(struct Vec3i* x, char* buff, size_t* buff_n, bool pack)
{
    if (!pack)
    {
        x->x = 0;
        x->y = 0;
        x->z = 0;
    }
    pack_u16(reinterpret_cast<uint16_t*>(&x->x), buff, buff_n, pack);
    pack_u16(reinterpret_cast<uint16_t*>(&x->y), buff, buff_n, pack);
    pack_u8(reinterpret_cast<uint8_t*>(&x->z), buff, buff_n, pack);
}

ALWAYS_INLINE void pack_color(Color* x, char* buff, size_t* buff_n, bool pack)
{
    pack_u8(&(x->r), buff, buff_n, pack);
    pack_u8(&(x->g), buff, buff_n, pack);
    pack_u8(&(x->b), buff, buff_n, pack);
}

ALWAYS_INLINE void pack_64(int64_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<int64_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<int64_t*>(buff+*buff_n));
    *buff_n += sizeof(int64_t);
}

ALWAYS_INLINE void pack_u64(uint64_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<uint64_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<uint64_t*>(buff+*buff_n));
    *buff_n += sizeof(uint64_t);
}

ALWAYS_INLINE void pack_32(int32_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<int32_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<int32_t*>(buff+*buff_n));
    *buff_n += sizeof(int32_t);
}

ALWAYS_INLINE void pack_u32(uint32_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<uint32_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<uint32_t*>(buff+*buff_n));
    *buff_n += sizeof(uint32_t);
}

ALWAYS_INLINE void pack_16(int16_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<int16_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<int16_t*>(buff+*buff_n));
    *buff_n += sizeof(int16_t);
}

ALWAYS_INLINE void pack_u16(uint16_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<uint16_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<uint16_t*>(buff+*buff_n));
    *buff_n += sizeof(uint16_t);
}

ALWAYS_INLINE void pack_8(int8_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<int8_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<int8_t*>(buff+*buff_n));
    *buff_n += sizeof(int8_t);
}

ALWAYS_INLINE void pack_u8(uint8_t* x, char* buff, size_t* buff_n, bool pack)
{
    if (pack) *(reinterpret_cast<uint8_t*>(buff+*buff_n)) = *x;
    else      *x = *(reinterpret_cast<uint8_t*>(buff+*buff_n));
    *buff_n += sizeof(uint8_t);
}

ALWAYS_INLINE void pack_string(char* str, size_t len, char* buff, size_t* buff_n, bool pack)
{
    if (pack)
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
