#pragma once

#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>

#include <math.h>

//note that c uses row major order
//glsl uses column major order

struct Mat3 {
    union
    {
        float f[3][3];
        float _f[9];
        struct 
        {
            struct Vec3 v[3];
        };
        struct 
        {
            struct Vec3 vx;
            struct Vec3 vy;
            struct Vec3 vz;
        };
    };
};

static struct Mat3 mat3_euler_rotation(float x, float y, float z) __attribute((always_inline));

struct Mat3 mat3_euler_rotation(float x, float y, float z)
{   
    x *= PI;
    y *= PI;
    z *= PI;

    double cx = cos(x);
    double sx = sin(x);
    double cy = cos(y);
    double sy = sin(y);
    double cz = cos(z);
    double sz = sin(z);
    
    struct Mat3 m;

    m.v[0].x = (cy*cx); 
    m.v[0].y = (cy*sx);
    m.v[0].z = (-sy);

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m.v[1].x = (szsy*cx-cz*sx);
    m.v[1].y = (szsy*sx+cz*cx);
    m.v[1].z = (sz*cy);

    m.v[2].x = (czsy*cx+sz*sx);
    m.v[2].y = (czsy*sx-sz*cx);
    m.v[2].z = (cz*cy);

    return m;
}