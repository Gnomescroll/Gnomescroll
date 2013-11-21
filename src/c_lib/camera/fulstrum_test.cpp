/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "fulstrum_test.hpp"

#include <math.h>

#include <physics/vec3.hpp>
#include <camera/fulstrum_test2.hpp>
#include <input/handlers.hpp>

static struct
{
    float zfar;

    float hy;
    float hx;

    float hy_sphere;
    float hx_sphere;

    struct Vec3 c; //camera
    struct Vec3 f;   //forward
    struct Vec3 u;   //up
    struct Vec3 r;   //right

    struct Vec3 r_2d;   //right
    struct Vec3 f_2d;
} fulstrum;

void setup_fulstrum(float fovy, float aspect, float zfar, Vec3 camera, Vec3 forward, Vec3 right, Vec3 up)
{
    if (!input_state.frustum) return;

    const float pi = 3.14159265f;
    fovy *= (pi/180.0f);

    fulstrum.zfar = zfar;

    double angle = fovy / 2.0f;
    double tang = tan(angle);
    double anglex = atan(tang*aspect);
    fulstrum.hy = (float)tan(angle);
    fulstrum.hx = (float)fulstrum.hy*aspect;

    fulstrum.hy_sphere = (float)(1.0/cos(angle));
    fulstrum.hx_sphere = (float)(1.0/cos(anglex));

    fulstrum.c = camera;
    fulstrum.f = forward;
    fulstrum.u = up;
    fulstrum.r = right;

    fulstrum.f_2d = forward;
    fulstrum.f_2d.z = 0.0f;
    fulstrum.f_2d = vec3_normalize(fulstrum.f_2d);

    fulstrum.r_2d = right;
    fulstrum.r_2d.z = 0.0f;
    fulstrum.r_2d = vec3_normalize(fulstrum.r_2d);
}

#define FULSTRUM_DEBUG_VERIFY 0 //verify results against fulstrum_test_2

#define SPHERE_FULSTRUM_DEBUG 0
#define SPHERE_FULSTRUM_DEBUG2 0 //use planes based point fulstrum test

bool sphere_fulstrum_test_translate(float x, float y, float z, float r)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);
    return sphere_fulstrum_test(x,y,z,r);
}

bool sphere_fulstrum_test(float x, float y, float z, float r)
{

#if FULSTRUM_DEBUG_VERIFY
    return point_fulstrum_test_debug(x,y,z);
#endif

    if (distancef_squared(fulstrum.c.x, fulstrum.c.y, fulstrum.c.z, x,y,z) > CAMERA_VIEW_DISTANCE_SQUARED)
        return false;

#if SPHERE_FULSTRUM_DEBUG2
    return point_fulstrum_test_2(x,y,z);
#endif

    //r = 1.0f;
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

#if SPHERE_FULSTRUM_DEBUG
    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    //printf("dz= %f, radius= %f \n", dz,r);
    if (dz < 0 || dz > fulstrum.zfar) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    //printf("dx= %f, theshhold= %f \n", dx, dz*fulstrum.hy);
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    //printf("dy= %f, theshhold= %f \n", dy, dz*fulstrum.hy);
    if (dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy) return false;

    return true;
#else
    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if (dz + r < 0 || dz > fulstrum.zfar - r) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    float rx = fulstrum.hx_sphere*r;
    if (dx < -dz*fulstrum.hx - rx|| dx > dz*fulstrum.hx + rx) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    float ry = fulstrum.hy_sphere*r;
    if (dy < -dz*fulstrum.hy - ry || dy > dz*fulstrum.hy + ry) return false;

    return true;
#endif
}

bool point_fulstrum_test_translate(float x, float y, float z)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);
    return point_fulstrum_test(x,y,z);
}

bool point_fulstrum_test(float x, float y, float z)
{
#if FULSTRUM_DEBUG_VERIFY
    return point_fulstrum_test_debug(x,y,z);
#endif

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if (dz < 0 || dz > CAMERA_VIEW_DISTANCE) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if (dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy) return false;

    return true;
}

//for skybox?
bool point_fulstrum_test_no_view_distance(float x, float y, float z)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if (dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy) return false;

    return true;
}

inline bool point_fulstrum_test_debug(float x, float y, float z)
{

    if (distancef_squared(fulstrum.c.x, fulstrum.c.y, fulstrum.c.z, x,y,z) > CAMERA_VIEW_DISTANCE_SQUARED)
        return false;

    //get return value from the other fulstrum method
    bool tmp = point_fulstrum_test_2(x,y,z);

    bool tmp1 = true;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if (dz < 0 || dz > fulstrum.zfar) tmp1 = false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) tmp1 = false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if (dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy) tmp1 = false;

    GS_ASSERT(tmp == tmp1);
    return tmp;
}

inline bool point_fulstrum_test_map(float x, float y, float z)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if (dz < 0 || dz > fulstrum.zfar) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if (dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy) return false;

    return true;
}

bool xy_circle_fulstrum_test(float x, float y, float r)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;

    float rx = fulstrum.hx_sphere*r;

    float dx = x*fulstrum.r_2d.x + y*fulstrum.r_2d.y;
    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;
    float view = dz*fulstrum.hx + rx;
    if (dx < -view || dx > view) return false;

    return true;
}

bool xy_point_fulstrum_test(float x, float y)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;

    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;
    float dx = x*fulstrum.r_2d.x + y*fulstrum.r_2d.y;
    if (dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx) return false;

    return true;
}

void draw_fulstrum()
{
    //implement later
}
