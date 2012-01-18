#pragma once

#include <math.h>

struct Vec3 
{
    union
    {
        float f[3];
        struct 
        {
            float x,y,z;
        };
    };
};

/*
    vector operations
*/

struct Vec3 vec3_init(float x, float y, float z) __attribute((always_inline)); 

struct Vec3 vec3_init(float x, float y, float z)
{
    struct Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

struct Vec3 vec3_normalize(struct Vec3 v) __attribute((always_inline)); 

struct Vec3 vec3_normalize(struct Vec3 v) 
{
    float l = 1.0/sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x *= l; 
    v.y *= l;
    v.z *= l;
    return v;
}

struct Vec3 vec3_cross(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

struct Vec3 vec3_cross(struct Vec3 v1, struct Vec3 v2)
{
    struct Vec3 v;
    v.x = v1.y*v2.z - v1.z*v2.y;
    v.y = v1.z*v2.x - v1.x*v2.z;
    v.z = v1.x*v2.y - v1.y*v2.x;
    return v;
}

struct Vec3 vec3_sub(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

struct Vec3 vec3_sub(struct Vec3 v1, struct Vec3 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

struct Vec3 vec3_scalar_mult(struct Vec3 v, float scalar) __attribute((always_inline)); 

struct Vec3 vec3_scalar_mult(struct Vec3 v, float scalar)
{
    v.x *= scalar;
    v.y *= scalar;
    v.z *= scalar;
    return v;
}

struct Vec3 vec3_add4(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3, struct Vec3 v4) __attribute((always_inline)); 

struct Vec3 vec3_add4(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3, struct Vec3 v4) 
{
    struct Vec3 v;
    v.x = v1.x + v2.x + v3.x + v4.x;
    v.y = v1.y + v2.y + v3.y + v4.y;
    v.z = v1.z + v2.z + v3.z + v4.z;
    return v;
}

struct Vec3 vec3_euler_rotation(Vector v, float x, float y, float z) __attribute((always_inline));

struct Vec3 vec3_euler_rotation(Vector v, float x, float y, float z)
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
    
    struct Vec3 m[3];

    m[0].x = (cy*cx); 
    m[0].y = (cy*sx);
    m[0].z = (-sy);

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m[1].x = (szsy*cx-cz*sx);
    m[1].y = (szsy*sx+cz*cx);
    m[1].z = (sz*cy);

    m[2].x = (czsy*cx+sz*sx);
    m[2].y = (czsy*sx-sz*cx);
    m[2].z = (cz*cy);

    struct Vec3 u;

    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x, 
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y, 
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}

/*
    scalar return
*/
float vec3_dot(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

float vec3_dot(struct Vec3 v1, struct Vec3 v2) 
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
}

float vec3_length(struct Vec3 v) __attribute((always_inline)); 

float vec3_length(struct Vec3 v) 
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}


/*
    diagnostic
*/
void vec3_print_dot(struct Vector v1, struct Vector v2);
void vec3_print_length(struct Vec3 v);



void vec3_print_length(struct Vec3 v) 
{
    float l = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("vector_length= %fs \n", l);
}


//diagnostic
void vec3_print_dot(struct Vector v1, struct Vector v2) 
{
    float d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}