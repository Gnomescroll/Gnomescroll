#include "fulstrum_test.hpp"

#include <math.h>

#include <physics/vec3.hpp>
#include <camera/fulstrum_test2.hpp>

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
    const float pi = 3.14159265;
    fovy *= (pi/180);

    fulstrum.zfar = zfar;

    double angle = fovy / 2.0;
    double tang = tan(angle);
    float anglex = atan(tang*aspect);
    fulstrum.hy = tan(fovy/2);
    fulstrum.hx = fulstrum.hy*aspect;

    fulstrum.hy_sphere = 1.0/cos(angle);
    fulstrum.hx_sphere = 1.0/cos(anglex);
    
    fulstrum.c = camera;
    fulstrum.f = forward;
    fulstrum.u = up;
    fulstrum.r = right;

    fulstrum.f_2d = forward;
    fulstrum.f_2d.z = 0.0;
    fulstrum.f_2d = vec3_normalize(fulstrum.f_2d);

    fulstrum.r_2d = right;
    fulstrum.r_2d.z = 0.0;
    fulstrum.r_2d = vec3_normalize(fulstrum.r_2d);
}

#define SPHERE_FULSTRUM_DEBUG 0
#define SPHERE_FULSTRUM_DEBUG2 1

bool sphere_fulstrum_test(float x, float y, float  z, float r)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);
    
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
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    //printf("dx= %f, theshhold= %f \n", dx, dz*fulstrum.hy);
    if( dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    //printf("dy= %f, theshhold= %f \n", dy, dz*fulstrum.hy);
    if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
#else
    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz + r < 0 || dz > fulstrum.zfar - r ) return false;
    
    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    float rx = fulstrum.hx_sphere*r;
    if( dx < -dz*fulstrum.hx - rx|| dx > dz*fulstrum.hx + rx ) return false;
    
    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    float ry = fulstrum.hy_sphere*r;
    if( dy < -dz*fulstrum.hy - ry || dy > dz*fulstrum.hy + ry ) return false;

    return true;
#endif
}

bool point_fulstrum_test(float x, float y, float  z)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);

    //if (distancef_squared(fulstrum.c.x, fulstrum.c.y, fulstrum.c.z, x,y,z) > CAMERA_VIEW_DISTANCE_SQUARED)
    //    return false;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz < 0 || dz > CAMERA_VIEW_DISTANCE ) return false;
    //if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if( dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}

inline bool point_fulstrum_test2(float x, float y, float  z)
{
    // DOES NOT TRANSLATE POINT

    //if (distancef_squared(fulstrum.c.x, fulstrum.c.y, fulstrum.c.z, x,y,z) > CAMERA_VIEW_DISTANCE_SQUARED)
    //    return false;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    //vec3_normalize(vec3_init(x,y,z))
    //printf("dot 1= %f \n", vec3_dot( fulstrum.f, fulstrum.r) );
    //printf("dot 2= %f \n", vec3_dot( fulstrum.f, fulstrum.u) );
    //printf("dot 3= %f \n", vec3_dot( fulstrum.u, fulstrum.r) );

    // printf("dot 1= %f \n", vec3_dot( fulstrum.f, fulstrum.f) );
    //printf("forward: %f %f %f = %f \n", fulstrum.f.x, fulstrum.f.y, fulstrum.f.z );

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if( dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}

inline bool point_fulstrum_test_map(float x, float y, float  z)
{
    // DOES NOT TRANSLATE POINT

    //if (distancef_squared(fulstrum.c.x, fulstrum.c.y, fulstrum.c.z, x,y,z) > CAMERA_VIEW_DISTANCE_SQUARED)
    //    return false;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    //vec3_normalize(vec3_init(x,y,z))
    //printf("dot 1= %f \n", vec3_dot( fulstrum.f, fulstrum.r) );
    //printf("dot 2= %f \n", vec3_dot( fulstrum.f, fulstrum.u) );
    //printf("dot 3= %f \n", vec3_dot( fulstrum.u, fulstrum.r) );

    // printf("dot 1= %f \n", vec3_dot( fulstrum.f, fulstrum.f) );
    //printf("forward: %f %f %f = %f \n", fulstrum.f.x, fulstrum.f.y, fulstrum.f.z );

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if( dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}

bool xy_circle_fulstrum_test(float x, float y, float r)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);

    //float dz = x*fulstrum.f.x + y*fulstrum.f.y;
    //if( dz + r < 0 || dz > fulstrum.zfar - r ) return false;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    
    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;

    float dx = x*fulstrum.r_2d.x + y*fulstrum.r_2d.y;
    float rx = fulstrum.hy_sphere*r;
    if( dx < -dz*fulstrum.hx - rx|| dx > dz*fulstrum.hx + rx ) return false;
    
    //float dy = x*fulstrum.u.x + y*fulstrum.u.y;
    //float ry = fulstrum.hy_sphere*r;
    //if( dy < -dz*fulstrum.hy - ry || dy > dz*fulstrum.hy + ry ) return false;

    return true;
}


bool xy_point_fulstrum_test(float x, float y)
{
    x = quadrant_translate_f(fulstrum.c.x, x);
    y = quadrant_translate_f(fulstrum.c.y, y);

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;

    //if(x*x + y*y <= 16*16) return true;

    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y);
    if( dx < -dz*fulstrum.hx || dx > dz*fulstrum.hx ) return false;

    //float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    //if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}


void draw_fulstrum()
{
    //implement later
}
