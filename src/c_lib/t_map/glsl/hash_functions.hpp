#pragma once

static ALWAYS_INLINE int hash_function2(int x,int y,int z);
static ALWAYS_INLINE int hash_function3(int x,int y,int z);

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

static inline int hash_function4(int x,int y,int z)
{
    unsigned int v = ((x*967 + y)*337 + 17*z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    return v;
}



static const int _vi2[3*4*6] =
{
    1,1,1, 0,1,1, 0,0,1, 1,0,1, //top
    0,1,0, 1,1,0, 1,0,0, 0,0,0, //bottom
    1,0,1, 1,0,0, 1,1,0, 1,1,1, //north
    0,1,1, 0,1,0, 0,0,0, 0,0,1, //south
    1,1,1, 1,1,0, 0,1,0, 0,1,1, //west
    0,0,1, 0,0,0, 1,0,0, 1,0,1  //east
};


static inline int hash_function_perlin(int x,int y,int z, int index)
{
    x += _vi2[index+0];
    y += _vi2[index+1];
    z += _vi2[index+2];

    unsigned int v = ((x*967 + y)*337 + 17*z);
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    return v;
}
