#include "fulstrum_test.hpp"

#include <math.h>

#include <c_lib/physics/vec3.hpp>

static struct
{
    float zfar;

    float hy;
    float hy_sphere;

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
    fulstrum.hy = fulstrum.hy*aspect;;
    fulstrum.hy_sphere = 1.0/cos(angle);
    fulstrum.hy_sphere = 1.0/cos(anglex);
    
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

bool sphere_fulstrum_test(float x, float y, float  z, float r)
{
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
    if( dx < -dz*fulstrum.hy || dx > dz*fulstrum.hy ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    //printf("dy= %f, theshhold= %f \n", dy, dz*fulstrum.hy);
    if( dy < -dz*(fulstrum.hy+r/2) || dy > dz*(fulstrum.hy+r/2) ) return false;

    return true;
#else
    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz + r < 0 || dz > fulstrum.zfar - r ) return false;
    
    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    float rx = fulstrum.hy_sphere*r;
    if( dx < -dz*fulstrum.hy - rx|| dx > dz*fulstrum.hy + rx ) return false;
    
    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    float ry = fulstrum.hy_sphere*r;
    if( dy < -dz*fulstrum.hy - ry || dy > dz*fulstrum.hy + ry ) return false;

    return true;
#endif
}

bool point_fulstrum_test(float x, float y, float  z)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    z -= fulstrum.c.z;

    float dz = x*fulstrum.f.x + y*fulstrum.f.y + z*fulstrum.f.z;
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y + z*fulstrum.r.z);
    if( dx < -dz*fulstrum.hy || dx > dz*fulstrum.hy ) return false;

    float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}

bool xy_circle_fulstrum_test(float x, float y, float r)
{
    //float dz = x*fulstrum.f.x + y*fulstrum.f.y;
    //if( dz + r < 0 || dz > fulstrum.zfar - r ) return false;

    x -= fulstrum.c.x;
    y -= fulstrum.c.y;
    
    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;

    float dx = x*fulstrum.r_2d.x + y*fulstrum.r_2d.y;
    float rx = fulstrum.hy_sphere*r;
    if( dx < -dz*fulstrum.hy - rx|| dx > dz*fulstrum.hy + rx ) return false;
    
    //float dy = x*fulstrum.u.x + y*fulstrum.u.y;
    //float ry = fulstrum.hy_sphere*r;
    //if( dy < -dz*fulstrum.hy - ry || dy > dz*fulstrum.hy + ry ) return false;

    return true;
}


bool xy_point_fulstrum_test(float x, float y)
{
    x -= fulstrum.c.x;
    y -= fulstrum.c.y;

    //if(x*x + y*y <= 16*16) return true;

    float dz = x*fulstrum.f_2d.x + y*fulstrum.f_2d.y;
    if( dz < 0 || dz > fulstrum.zfar ) return false;

    float dx = (x*fulstrum.r.x + y*fulstrum.r.y);
    if( dx < -dz*fulstrum.hy || dx > dz*fulstrum.hy ) return false;

    //float dy = x*fulstrum.u.x + y*fulstrum.u.y + z*fulstrum.u.z;
    //if( dy < -dz*fulstrum.hy || dy > dz*fulstrum.hy ) return false;

    return true;
}


void draw_fulstrum()
{
    //implement later
}
