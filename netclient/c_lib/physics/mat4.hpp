#pragma once
#pragma GCC diagnostic ignored "-Wunused-function"


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

        struct Vec4 v[4];

        struct 
        {
            struct Vec4 vx;
            struct Vec4 vy;
            struct Vec4 vz;
            struct Vec4 c;
        };
    };
} __attribute__((aligned (16)));


static float mat4_row_mult_column(float* r, int i, float *c, int j)  __attribute((always_inline));

float mat4_row_mult_column(float* r, int i, float *c, int j)
{
    return r[4*0+i]*c[4*j+0] + r[4*1+i]*c[4*j+1] + r[4*2+i]*c[4*j+2] + r[4*3+i]*c[4*j+3];
}


static struct Mat4 mat4_mult(Mat4 a, Mat4 b) __attribute((always_inline));

struct Mat4 mat4_mult(Mat4 a, Mat4 b)
{
    struct Mat4 c;
    int i,j;
    for(i=0; i<4; i++) //row
    {
        for(j=0; j<4; j++) 
        {
            c._f[4*j+i] = mat4_row_mult_column( a._f, i, b._f, j );
        }
    }
    return c;
}


static struct Mat4 mat4_identity()  __attribute((always_inline));

struct Mat4 mat4_identity()
{   
    struct Mat4 m;

    m.v[0] = vec4_init(1.0, 0.0, 0.0, 0.0);
    m.v[1] = vec4_init(0.0, 1.0, 0.0, 0.0);
    m.v[2] = vec4_init(0.0, 0.0, 1.0, 0.0);
    m.v[3] = vec4_init(0.0, 0.0, 0.0, 1.0);

    return m;
}

static struct Mat4 mat4_euler_rotation(float x, float y, float z)  __attribute((always_inline));

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

static struct Mat4 mat4_euler_rotation_and_translation(float _x, float _y, float _z, float x, float y, float z)  __attribute((always_inline));

struct Mat4 mat4_euler_rotation_and_translation(float _x, float _y, float _z, float x, float y, float z)
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

    m.v[3] = vec4_init(_x, _y, _z, 1.0);

    return m;
}

static struct Mat4 mat4_apply_rotation(struct Mat3 m)  __attribute((always_inline));

struct Mat4 mat4_apply_mat3_rotation_matrix(struct Mat3 m1, struct Mat3 m2)
{   
    struct Mat4 m;
    //not used/implemented yet
    return m;
}


static struct Vec3 vec3_apply_rotation(struct Vec3 v, struct Mat4 m) __attribute((always_inline));

struct Vec3 vec3_apply_rotation(struct Vec3 v, struct Mat4 m)
{   
    struct Vec3 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;

    return u;
}

static struct Vec4 vec4_apply_rotation(struct Vec4 v, struct Mat4 m) __attribute((always_inline));

struct Vec4 vec4_apply_rotation(struct Vec4 v, struct Mat4 m)
{   
    struct Vec4 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;
    u.w = 0;

    return u;
}