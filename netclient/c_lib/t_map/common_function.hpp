#include once

static inline int hash_function2(int x,int y,int z) __attribute((always_inline));
static inline int hash_function3(int x,int y,int z) __attribute((always_inline));

static inline int hash_function2(int x,int y,int z)
 {
    unsigned int v = ((x*967 + y)*337 + 17*z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    v &= 0xf;
    return (0x6996 >> v) & 1;
}

static inline int hash_function3(int x,int y,int z)
{
    unsigned int v = ((x*967 + y)*337 + 17*z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    return v % 3;
    //take modulos of the last byte
}