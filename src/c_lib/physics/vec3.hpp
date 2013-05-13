#pragma once

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <math.h>
#include <common/random.hpp>
#include <physics/common.hpp>
#include <physics/constants.hpp>
#include <common/macros.hpp>
#include <t_map/common/types.hpp>
#include <common/common.hpp>

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

/*
    Vec3 operations
*/

ALWAYS_INLINE float vec3_dot(struct Vec3 a, struct Vec3 b);

ALWAYS_INLINE
struct Vec3 vec3_init(float x, float y, float z)
{
    struct Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_init(float p[3])
{
    struct Vec3 v;
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_init(int p[3])
{
    struct Vec3 v;
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    return v;
}

ALWAYS_INLINE struct Vec3 vec3_init(float n)
{
    return vec3_init(n, n, n);
}

ALWAYS_INLINE struct Vec3 vec3_init(int n)
{
    return vec3_init(n, n, n);
}

ALWAYS_INLINE struct Vec3 vec3_init(struct Vec3i pos);

ALWAYS_INLINE
struct Vec3 vec3_copy(struct Vec3 f)
{
    struct Vec3 v;
    v.x = f.x;
    v.y = f.y;
    v.z = f.z;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_normalize(struct Vec3 v)
{
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    IF_ASSERT(len == 0.0f)
        return v;
    len = 1.0f/len;
    v.x *= len;
    v.y *= len;
    v.z *= len;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_abs(struct Vec3 v)
{
    v.x = fabsf(v.x);
    v.y = fabsf(v.y);
    v.z = fabsf(v.z);
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_cross(struct Vec3 a, struct Vec3 b)
{
    struct Vec3 v;
    v.x = a.y*b.z - a.z*b.y;
    v.y = a.z*b.x - a.x*b.z;
    v.z = a.x*b.y - a.y*b.x;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_mult(struct Vec3 a, struct Vec3 b)
{
    struct Vec3 v;
    v.x = a.x * b.x;
    v.y = a.y * b.y;
    v.z = a.z * b.z;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_sub(struct Vec3 a, struct Vec3 b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

ALWAYS_INLINE
struct Vec3 vec3_scalar_mult(struct Vec3 v, float scalar)
{
    v.x *= scalar;
    v.y *= scalar;
    v.z *= scalar;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_scalar_add(struct Vec3 v, float scalar)
{
    v.x += scalar;
    v.y += scalar;
    v.z += scalar;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_add(struct Vec3 a, struct Vec3 b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

ALWAYS_INLINE
struct Vec3 vec3_add3(struct Vec3 a, struct Vec3 b, struct Vec3 c)
{
    struct Vec3 v;
    v.x = a.x + b.x + c.x;
    v.y = a.y + b.y + c.y;
    v.z = a.z + b.z + c.z;
    return v;
}

ALWAYS_INLINE
struct Vec3 vec3_add4(struct Vec3 a, struct Vec3 b, struct Vec3 c, struct Vec3 d)
{
    struct Vec3 v;
    v.x = a.x + b.x + c.x + d.x;
    v.y = a.y + b.y + c.y + d.y;
    v.z = a.z + b.z + c.z + d.z;
    return v;
}

ALWAYS_INLINE
static Vec3 vec3_reflect(struct Vec3 i, struct Vec3 n)
{
    return vec3_sub(i, vec3_scalar_mult(n, 2.0f*vec3_dot(n, i)));
}

ALWAYS_INLINE
static Vec3 vec3_interpolate(struct Vec3 a, struct Vec3 b, float d)
{
    return vec3_add(a, vec3_scalar_mult(vec3_sub(b, a), d));
}

ALWAYS_INLINE
struct Vec3 vec3_euler_rotation(Vec3 v, float x, float y, float z)
{
    // x is actually yaw (a rotation around the Z axis)
    // y is actually roll (a rotation around the Y axis)
    // z is actually pitch (a rotation around the X axis)
    x *= PI;
    y *= PI;
    z *= PI;

    float cx = cosf(x);
    float sx = sinf(x);
    float cy = cosf(y);
    float sy = sinf(y);
    float cz = cosf(z);
    float sz = sinf(z);

    struct Vec3 m[3];

    m[0].x = cy*cx;
    m[0].y = cy*sx;
    m[0].z = -sy;

    float szsy = sz*sy;
    float czsy = cz*sy;

    m[1].x = szsy*cx-cz*sx;
    m[1].y = szsy*sx+cz*cx;
    m[1].z = sz*cy;

    m[2].x = czsy*cx+sz*sx;
    m[2].y = czsy*sx-sz*cx;
    m[2].z = cz*cy;

    //printf("m[0]: %f %f %f \n", m[0].x,m[0].y, m[0].z);
    //printf("m[1]: %f %f %f \n", m[2].x,m[1].y, m[1].z);
    //printf("m[2]: %f %f %f \n", m[2].x,m[2].y, m[2].z);

    struct Vec3 u;
    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x,
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y,
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}

ALWAYS_INLINE
struct Vec3 vec3_init_from_angles(float theta, float phi, float rho)
{
    Vec3 unit = vec3_init(1,0,0);
    unit = vec3_euler_rotation(unit, theta, phi, rho);
    return vec3_normalize(unit);
}

ALWAYS_INLINE
struct Vec3 vec3_bias_random(struct Vec3 v, const float bias)
{
    const float arc = bias / 360.0f;
    const float theta = ((randf() * PI * 2) - PI) * arc;
    const float phi = ((randf() * PI * 2) - PI) * arc;
    const float rho = ((randf() * PI * 2) - PI) * arc;
    return vec3_euler_rotation(v, theta, phi, rho);
}

ALWAYS_INLINE
struct Vec3 vec3_bias(struct Vec3 v, const float bias)
{
    const float arc = bias / 360.0f;
    const float factor = ((PI * 2) - PI) * arc;
    return vec3_euler_rotation(v, factor, factor, factor);
}

/*
    scalar return
*/

ALWAYS_INLINE
float vec3_dot(struct Vec3 a, struct Vec3 b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

ALWAYS_INLINE
float vec3_length_squared(struct Vec3 v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

ALWAYS_INLINE
float vec3_length(struct Vec3 v)
{
    return sqrtf(vec3_length_squared(v));
}

ALWAYS_INLINE
float vec3_cos2(struct Vec3 a, struct Vec3 b)
{
    float dot =  vec3_dot(a, b);
    return (dot*dot) / (vec3_dot(a, a) * vec3_dot(b, b));
}

ALWAYS_INLINE
float vec3_distance_squared(struct Vec3 a, struct Vec3 b)
{
    return (b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z);
}

ALWAYS_INLINE
float vec3_distance(struct Vec3 a, struct Vec3 b)
{
    return sqrtf(vec3_distance_squared(a, b));
}

// Angle from look vector vx,vy,vz to point x,y,z
ALWAYS_INLINE
float vec3_angle_to_point(Vec3 pt, Vec3 look, Vec3 pt2)
{
    look = vec3_normalize(look);
    pt2 = vec3_normalize(vec3_sub(pt2, pt));
    float x = vec3_dot(look, pt2);
    return acosf(x);
}

ALWAYS_INLINE
float vec3_to_theta(Vec3 direction)
{
    direction.z = 0;
    if (vec3_length_squared(direction) == 0.0f) return 0.0f;
    direction = vec3_normalize(direction);
    float t = acosf(direction.x) / PI;
    if (direction.y < 0) t = -t;
    return t;
}

//  x- and y-angle (theta,phi) rotation between two Vec3s
ALWAYS_INLINE
void vec3_to_angles(Vec3 direction, float* theta, float* phi)
{
    direction = vec3_normalize(direction);
    float z = direction.z;
    direction.z = 0;
    if (vec3_length_squared(direction) == 0.0f)
    {
        *theta = 0.0f;
        *phi = 0.0f;
        return;
    }
    direction = vec3_normalize(direction);

    float t = acosf(direction.x) / PI;
    if (direction.y < 0) t = -t;
    *theta = t;

    t = asinf(z) / PI;
    t += 0.5f;
    *phi = t;
}


ALWAYS_INLINE
struct Vec3 vec3_rand()
{
    return vec3_init(randf(), randf(), randf());
}

ALWAYS_INLINE
struct Vec3 vec3_rand_center()
{
    static const struct Vec3 half = vec3_init(0.5f);
    return vec3_sub(vec3_rand(), half);
}

ALWAYS_INLINE
bool vec3_equal(Vec3 a, Vec3 b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

ALWAYS_INLINE
bool vec3_equal_approximate(Vec3 a, Vec3 b)
{   // NOTE: This doesn't account for accuracy variation with large
    // or very small values.
    return (is_equal(a.x, b.x) && is_equal(a.y, b.y) && is_equal(a.z, b.z));
}

ALWAYS_INLINE
bool vec3_isnan(Vec3 v)
{
    #ifdef NAN
    return (isnan(v.x) || isnan(v.y) || isnan(v.z));
    #endif
    return false;
}

ALWAYS_INLINE
bool vec3_isfinite(Vec3 v)
{
    return (isfinite(v.x) || isfinite(v.y) || isfinite(v.z));
}

ALWAYS_INLINE
bool vec3_is_valid(Vec3 v)
{
    return (vec3_isfinite(v) && !vec3_isnan(v));
}

ALWAYS_INLINE
struct Vec3 vec3_lerp(struct Vec3 a, struct Vec3 b, float f)
{
    struct Vec3 v;
    const float g = 1.0 - f;
    v.x = g*a.x + f*b.x;
    v.y = g*a.y + f*b.y;
    v.z = g*a.z + f*b.z;
    return v;
}

/*
    diagnostic
*/
void vec3_print_dot(struct Vec3 a, struct Vec3 b);
void vec3_print_length(struct Vec3 v);

void vec3_print(struct Vec3 v)
{
    printf("vec3: %0.2f %0.2f %0.2f\n", v.x, v.y, v.z);
}

void vec3_print_dot(struct Vec3 a, struct Vec3 b)
{
    float d = a.x*b.x + a.y*b.y + a.z*+b.z;
    printf("vec3 dot: %f\n", d);
}

void vec3_print_length(struct Vec3 v)
{
    float l = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("vec3 length: %f\n", l);
}
