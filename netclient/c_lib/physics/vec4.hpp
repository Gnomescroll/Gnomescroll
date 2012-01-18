#pragma once

#include <math.h>

#include <c_lib/physics/vec3.hpp>

struct Vec4 {
    union
    {
        float f[4];
        struct 
        {
            float x,y,z,w;
        };
        struct 
        {
            Vec3 v3;    
        };
    };
};

inline static struct Vec4 Vec4_init(float x, float y, float z, float w) __attribute((always_inline)); 

struct Vec4 vec4_init(float x, float y, float z, float w)
{
    struct Vec4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w
    return v;
}
