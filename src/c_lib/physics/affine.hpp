#pragma once
#ifdef __GNUC
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>
#include <physics/mat3.hpp>

struct Affine {
    union
    {
        float _f[12];

        struct Vec3 v[4];

        struct 
        {
            struct Vec3 vx;
            struct Vec3 vy;
            struct Vec3 vz;
            struct Vec3 c;
        };
    };
};

 //__attribute__((aligned (16)));

void print_affine(struct Affine m)
{
    printf("%.2f %.2f %.2f \n", m.v[0].x,m.v[0].y,m.v[0].z);
    printf("%.2f %.2f %.2f \n", m.v[1].x,m.v[1].y,m.v[1].z);
    printf("%.2f %.2f %.2f \n", m.v[2].x,m.v[2].y,m.v[2].z);
    printf("%.2f %.2f %.2f \n", m.v[3].x,m.v[3].y,m.v[3].z);
}

ALWAYS_INLINE
struct Affine affine_mult(Affine a, Affine b)
{
    struct Affine c;

    for(int i = 0; i < 3; i++)
    {
        c._f[3*0+i] = a._f[3*0+i]*b._f[3*0+0] + a._f[3*1+i]*b._f[3*0+1] + a._f[3*2+i]*b._f[3*0+2];
        c._f[3*1+i] = a._f[3*0+i]*b._f[3*1+0] + a._f[3*1+i]*b._f[3*1+1] + a._f[3*2+i]*b._f[3*1+2];
        c._f[3*2+i] = a._f[3*0+i]*b._f[3*2+0] + a._f[3*1+i]*b._f[3*2+1] + a._f[3*2+i]*b._f[3*2+2];
    }

    c.c.x = b.c.x*a.v[0].x + b.c.y*a.v[1].x + b.c.z*a.v[2].x + a.c.x;
    c.c.y = b.c.x*a.v[0].y + b.c.y*a.v[1].y + b.c.z*a.v[2].y + a.c.y;
    c.c.z = b.c.x*a.v[0].z + b.c.y*a.v[1].z + b.c.z*a.v[2].z + a.c.z;

    return c;
}

ALWAYS_INLINE
struct Affine affine_identity()
{   
    struct Affine m;

    m.v[0] = vec3_init(1.0f, 0.0f, 0.0f);
    m.v[1] = vec3_init(0.0f, 1.0f, 0.0f);
    m.v[2] = vec3_init(0.0f, 0.0f, 1.0f);
    m.v[3] = vec3_init(0.0f, 0.0f, 0.0f);

    return m;
}

ALWAYS_INLINE
struct Affine affine_euler_rotation_and_translation(float _x, float _y, float _z, float x, float y, float z)
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
    
    struct Affine m;

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

    m.c = vec3_init(_x, _y, _z);

    return m;
}
