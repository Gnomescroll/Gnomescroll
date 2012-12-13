#pragma once
#ifdef __GNUC
# pragma GCC diagnostic ignored "-Wunused-function"
#endif


#include <physics/vec3.hpp>
#include <physics/vec4.hpp>

#include <math.h>

//note that c uses row major order
//glsl uses column major order

struct Mat3
{
    union
    {
        float f[3][3];
        float _f[9];

        struct Vec3 v[3];

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

    m.v[0].x = (float)(cy*cx); 
    m.v[0].y = (float)(cy*sx);
    m.v[0].z = (float)(-sy);

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m.v[1].x = (float)(szsy*cx-cz*sx);
    m.v[1].y = (float)(szsy*sx+cz*cx);
    m.v[1].z = (float)(sz*cy);

    m.v[2].x = (float)(czsy*cx+sz*sx);
    m.v[2].y = (float)(czsy*sx-sz*cx);
    m.v[2].z = (float)(cz*cy);

    return m;
}

static struct Vec3 vec3_apply_rotation(struct Vec3 v, struct Mat3 m) __attribute((always_inline));

struct Vec3 vec3_apply_rotation(struct Vec3 v, struct Mat3 m)
{   
    struct Vec3 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;

    return u;
}

static struct Vec4 vec4_apply_rotation(struct Vec4 v, struct Mat3 m) __attribute((always_inline));

struct Vec4 vec4_apply_rotation(struct Vec4 v, struct Mat3 m)
{   
    struct Vec4 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;
    u.w = 0;

    return u;
}

void mat3_from_vec3(struct Mat3& m, struct Vec3 f, struct Vec3 r, struct Vec3 n)
{
    for (size_t i=0, j=0; i<3 && j<3; i++, j++)
        m._f[i] = f.f[j];
    for (size_t i=3, j=0; i<6 && j<3; i++, j++)
        m._f[i] = r.f[j];
    for (size_t i=6, j=0; i<9 && j<3; i++, j++)
        m._f[i] = n.f[j];
}
