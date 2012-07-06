#pragma once
#pragma GCC diagnostic ignored "-Wunused-function"

#include <math.h>
#include <common/random.h>
#include <physics/common.hpp>

#include <physics/constants.hpp>

#define PI 3.14159265f

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
//static float vec3_dot(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline)); 

/*
    Vec3 operations
*/

static float vec3_dot(struct Vec3 v1, struct Vec3 v2) __attribute((always_inline));

static void normalize_vector(struct Vec3* v)
{
    float len = sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
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

static struct Vec3 vec3_init(float p[3]) __attribute((always_inline)); 

struct Vec3 vec3_init(float p[3])
{
    struct Vec3 v;
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
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
    float l = (float)(1.0/sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
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

static struct Vec3 vec3_mult(struct Vec3 vl, struct Vec3 v2) __attribute((always_inline));

struct Vec3 vec3_mult(struct Vec3 v1, struct Vec3 v2)
{
    struct Vec3 v;
    v.x = v1.x * v2.x;
    v.y = v1.y * v2.y;
    v.z = v1.z * v2.z;
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

static struct Vec3 vec3_add3(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3) __attribute((always_inline)); 

struct Vec3 vec3_add3(struct Vec3 v1, struct Vec3 v2, struct Vec3 v3) 
{
    struct Vec3 v;
    v.x = v1.x + v2.x + v3.x;
    v.y = v1.y + v2.y + v3.y;
    v.z = v1.z + v2.z + v3.z;
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
    //v = vec3_add(i, vec3_scalar_mult(vec3_scalar_mult(n, 2.0f*vec3_dot(n,i)), -1));
    v = vec3_sub(i, vec3_scalar_mult(n, 2.0f*vec3_dot(n,i)));
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

    m[0].x = (float)(cy*cx); 
    m[0].y = (float)(cy*sx);
    m[0].z = (float)(-sy);

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m[1].x = (float)(szsy*cx-cz*sx);
    m[1].y = (float)(szsy*sx+cz*cx);
    m[1].z = (float)(sz*cy);

    m[2].x = (float)(czsy*cx+sz*sx);
    m[2].y = (float)(czsy*sx-sz*cx);
    m[2].z = (float)(cz*cy);

    struct Vec3 u;

    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x, 
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y, 
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}

static struct Vec3 vec3_init_from_angles(float theta, float phi, float rho) __attribute((always_inline));

static struct Vec3 vec3_init_from_angles(float theta, float phi, float rho)
{
    Vec3 unit = vec3_init(1,0,0);
    unit = vec3_euler_rotation(unit, theta, phi, rho);
    normalize_vector(&unit);
    return unit;
}

static struct Vec3 vec3_bias_random(Vec3 v, const float bias) __attribute((always_inline));

Vec3 vec3_bias_random(Vec3 v, const float bias)
{
    const float arc = bias / 360.0f;
    const float theta = ((randf() * PI * 2) - PI) * arc;
    const float phi = ((randf() * PI * 2) - PI) * arc;
    const float rho = ((randf() * PI * 2) - PI) * arc;
    return vec3_euler_rotation(v, theta, phi, rho);
}

static struct Vec3 vec3_bias(Vec3 v, const float bias) __attribute((always_inline));

Vec3 vec3_bias(Vec3 v, const float bias)
{
    const float arc = bias / 360.0f;
    const float factor = ((PI * 2) - PI) * arc;
    return vec3_euler_rotation(v, factor, factor, factor);
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
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static float vec3_length_squared(struct Vec3 v) __attribute((always_inline));

float vec3_length_squared(struct Vec3 v) 
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

static float vec3_distance(struct Vec3 v) __attribute((always_inline));
float vec3_distance(struct Vec3 v1, struct Vec3 v2)
{
    return sqrtf((v2.x-v1.x)*(v2.x-v1.x) + (v2.y-v1.y)*(v2.y-v1.y) + (v2.z-v1.z)*(v2.z-v1.z));
}

static float vec3_distance_squared(struct Vec3 v) __attribute((always_inline));
float vec3_distance_squared(struct Vec3 v1, struct Vec3 v2)
{
    return (v2.x-v1.x)*(v2.x-v1.x) + (v2.y-v1.y)*(v2.y-v1.y) + (v2.z-v1.z)*(v2.z-v1.z);
}

// Angle from look vector vx,vy,vz to point x,y,z
static float vec3_angle_to_point(Vec3 pt, Vec3 look, Vec3 pt2) __attribute((always_inline));
float vec3_angle_to_point(Vec3 pt, Vec3 look, Vec3 pt2)
{
    normalize_vector(&look);
    pt2 = vec3_sub(pt2, pt);
    normalize_vector(&pt2);
    float x = vec3_dot(look, pt2);
    x = acosf(x);
    return x;
}

static float vec3_to_theta(Vec3 direction) __attribute((always_inline));
static float vec3_to_theta(Vec3 direction)
{
    direction.z = 0;
    normalize_vector(&direction);
    float t = acosf(direction.x) / PI;
    if (direction.y < 0) t = -t;
    return t;
}

//  x- and y-angle (theta,phi) rotation between two Vec3s
static void vec3_to_angles(Vec3 direction, float* theta, float* phi) __attribute((always_inline));
static void vec3_to_angles(Vec3 direction, float* theta, float* phi)
{
    normalize_vector(&direction);
    float z = direction.z;
    direction.z = 0;
    normalize_vector(&direction);

    float t = acosf(direction.x) / PI;
    if (direction.y < 0) t = -t;
    *theta = t;

    t = asinf(z) / PI;
    t += 0.5f;
    *phi = t;
}


static bool vec3_equal(Vec3 v1, Vec3 v2) __attribute((always_inline));
static bool vec3_equal(Vec3 v1, Vec3 v2)
{
    if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) return true;
    return false;
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
    float l = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("Vec3_length= %fs \n", l);
}


//diagnostic
void vec3_print_dot(struct Vec3 v1, struct Vec3 v2) 
{
    float d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}
