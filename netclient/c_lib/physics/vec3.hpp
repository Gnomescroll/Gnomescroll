#pragma once
#pragma GCC diagnostic ignored "-Wunused-function"

#include <math.h>

#define PI 3.14159265

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

//__attribute__((aligned (16)));
static float vec3_dot(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

/*
    Vec3 operations
*/

static void normalize_vector(struct Vec3* v)
{
    float len = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

static struct Vec3 vec3_init(float x, float y, float z) __attribute((always_inline)); 

struct Vec3 vec3_init(float x, float y, float z)
{
    struct Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static struct Vec3 vec3_copy(Vec3 f) __attribute((always_inline)); 

struct Vec3 vec3_copy(Vec3 f)
{
    struct Vec3 v;
    v.x = f.x;
    v.y = f.y;
    v.z = f.z;
    return v;
}

static struct Vec3 vec3_normalize(struct Vec3 v) __attribute((always_inline)); 

struct Vec3 vec3_normalize(struct Vec3 v) 
{
    float l = 1.0/sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x *= l; 
    v.y *= l;
    v.z *= l;
    return v;
}

static struct Vec3 vec3_cross(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

struct Vec3 vec3_cross(struct Vec3 v1, struct Vec3 v2)
{
    struct Vec3 v;
    v.x = v1.y*v2.z - v1.z*v2.y;
    v.y = v1.z*v2.x - v1.x*v2.z;
    v.z = v1.x*v2.y - v1.y*v2.x;
    return v;
}

static struct Vec3 vec3_sub(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

struct Vec3 vec3_sub(struct Vec3 v1, struct Vec3 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

static struct Vec3 vec3_scalar_mult(struct Vec3 v, float scalar) __attribute((always_inline)); 

struct Vec3 vec3_scalar_mult(struct Vec3 v, float scalar)
{
    v.x *= scalar;
    v.y *= scalar;
    v.z *= scalar;
    return v;
}

static struct Vec3 vec3_scalar_add(struct Vec3 v, float scalar) __attribute((always_inline)); 

struct Vec3 vec3_scalar_add(struct Vec3 v, float scalar)
{
    v.x += scalar;
    v.y += scalar;
    v.z += scalar;
    return v;
}

static struct Vec3 vec3_add(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

struct Vec3 vec3_add(struct Vec3 v1, struct Vec3 v2) 
{
    struct Vec3 v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    v.z = v1.z + v2.z;
    return v;
}

static struct Vec3 vec3_add4(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3, struct Vec3 v4) __attribute((always_inline)); 

struct Vec3 vec3_add4(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3, struct Vec3 v4) 
{
    struct Vec3 v;
    v.x = v1.x + v2.x + v3.x + v4.x;
    v.y = v1.y + v2.y + v3.y + v4.y;
    v.z = v1.z + v2.z + v3.z + v4.z;
    return v;
}

static struct Vec3 vec3_reflect(struct Vec3 i, struct Vec3 n) __attribute((always_inline));

static struct Vec3 vec3_reflect(struct Vec3 i, struct Vec3 n)
{
    struct Vec3 v;
    v = vec3_add(i, vec3_scalar_mult(vec3_scalar_mult(n, 2.0f*vec3_dot(n,i)), -1));
    //v = inc - 2*(nor.inc)*nor   <<< pseudocode
    return v;
}

static struct Vec3 vec3_euler_rotation(Vec3 v, float x, float y, float z) __attribute((always_inline));

struct Vec3 vec3_euler_rotation(Vec3 v, float x, float y, float z)
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

float vec3_dot(struct Vec3 v1, struct Vec3 v2) 
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
}

static float vec3_length(struct Vec3 v) __attribute((always_inline)); 

float vec3_length(struct Vec3 v) 
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

static float vec3_distance(struct Vec3 v) __attribute((always_inline));
float vec3_distance(struct Vec3 v1, struct Vec3 v2)
{
    return sqrt((v2.x-v1.x)*(v2.x-v1.x) + (v2.y-v1.y)*(v2.y-v1.y) + (v2.z-v1.z)*(v2.z-v1.z));
}

// Angle from look vector vx,vy,vz to point x,y,z
static float vec3_angle_to_point(Vec3 pt, Vec3 look, Vec3 pt2) __attribute((always_inline));
float vec3_angle_to_point(Vec3 pt, Vec3 look, Vec3 pt2)
{
    normalize_vector(&look);
    pt2 = vec3_sub(pt2, pt);
    normalize_vector(&pt2);
    float x = vec3_dot(look, pt2);
    x = acos(x);
    return x;
}

/*
    diagnostic
*/
void vec3_print_dot(struct Vec3 v1, struct Vec3 v2);
void vec3_print_length(struct Vec3 v);

void vec3_print(struct Vec3 v);

void vec3_print(struct Vec3 v)
{
    printf("Vec3: %0.2f %0.2f %0.2f\n", v.x, v.y, v.z);
}

void vec3_print_length(struct Vec3 v) 
{
    float l = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("Vec3_length= %fs \n", l);
}


//diagnostic
void vec3_print_dot(struct Vec3 v1, struct Vec3 v2) 
{
    float d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}
