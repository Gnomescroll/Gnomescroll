#pragma once

#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/vec4.hpp>
#include <c_lib/physics/mat3.hpp>

//note that c uses row major order
//glsl uses column major order

struct Mat4 {
    union
    {
        float f[4][4];
        float _f[16]; 
        struct 
        {
            struct Vec4 v[4];
        };
        struct 
        {
            struct Vec4 vx;
            struct Vec4 vy;
            struct Vec4 vz;
            struct Vec4 c;
        };
    };
};

static struct Mat4 mat4_euler_rotation(float x, float y, float z)  __attribute((always_inline));

//rotation matrices are symmetric so there goes half of your multiplies

struct Mat4 mat4_euler_rotation(float x, float y, float z)
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
    
    struct Mat4 m;

    m.v[0].x = (cy*cx); 
    m.v[0].y = (cy*sx);
    m.v[0].z = (-sy);
    m.v[0].w = 0.0;

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m.v[1].x = (szsy*cx-cz*sx);
    m.v[1].y = (szsy*sx+cz*cx);
    m.v[1].z = (sz*cy);
    m.v[1].w = 0.0;

    m.v[2].x = (czsy*cx+sz*sx);
    m.v[2].y = (czsy*sx-sz*cx);
    m.v[2].z = (cz*cy);
    m.v[2].w = 0.0;

    m.v[3] = vec4_init(0.0, 0.0, 0.0, 1.0);

    return m;
}

static struct Mat4 mat4_apply_rotation(struct Mat3 m)  __attribute((always_inline));

struct Mat4 mat4_apply_mat3_rotation_matrix(struct Mat3 m1, struct Mat3 m2)
{   
    struct Mat4 m;

    m.v[0].x = 
    m.v[0].y =
    m.v[0].z = 


//    m.v[0].w = 0.0;
//    m.v[1].w = 0.0;
//    m.v[2].w = 0.0;
//    m.v[3] = vec4_init(0.0, 0.0, 0.0, 1.0);
    return m;
}