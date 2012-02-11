#include "fulstrum_test.hpp"

#include <math.h>

#include <c_lib/physics/vec3.hpp>

void setup_test_fulstrum(int camera)
{}

//static float fulstrum_fovy;
//static float fulstrum_fovx;
//static float fulstrum_aspect;
static float fulstrum_zfar;

static float fulstrum_hx;
static float fulstrum_hy;

static float fulstrum_hy_sphere;
static float fulstrum_hx_sphere;

/*
    Use vec3, Vec3 deprecated
*/
static struct Vec3 fulstrum_c; //camera
static struct Vec3 fulstrum_f;   //forward
static struct Vec3 fulstrum_u;   //up
static struct Vec3 fulstrum_r;   //right

static struct Vec3 fulstrum_2d_r;   //right

void setup_fulstrum(float fovy, float aspect, float zfar, Vec3 camera, Vec3* forward, Vec3 *right, Vec3* up)
{
    const float pi = 3.14159265;

    fovy *= (pi/180);

    fulstrum_hy = tan(fovy/2);
    fulstrum_hx = fulstrum_hy*aspect;;

    //WTF COED
    double angle = fovy / 2.0;
    double tang = tan(angle);
    float anglex = atan(tang*aspect);

    fulstrum_hy_sphere = 1.0/cos(angle);
    fulstrum_hx_sphere = 1.0/cos(anglex);

    fulstrum_zfar = zfar;

    fulstrum_c = camera;
    fulstrum_f = *forward;
    fulstrum_r = *right;
    fulstrum_u = *up;

    fulstrum_2d_r = *right;
    fulstrum_2d_r.z = 0.0;
    fulstrum_2d_r = vec3_normalize(fulstrum_2d_r);
    //fulstrum_2d_r.x

}

#define SPHERE_FULSTRUM_DEBUG 0

bool sphere_fulstrum_test(float x, float y, float  z, float r)
{
    x -= fulstrum_c.x;
    y -= fulstrum_c.y;
    z -= fulstrum_c.z;

#if SPHERE_FULSTRUM_DEBUG
    float dz = x*fulstrum_f.x + y*fulstrum_f.y + z*fulstrum_f.z;
    //printf("dz= %f, radius= %f \n", dz,r);
    if( dz < 0 || dz > fulstrum_zfar ) return false;

    float dx = (x*fulstrum_r.x + y*fulstrum_r.y + z*fulstrum_r.z);
    //printf("dx= %f, theshhold= %f \n", dx, dz*fulstrum_hx);
    if( dx < -dz*fulstrum_hx || dx > dz*fulstrum_hx ) return false;

    float dy = x*fulstrum_u.x + y*fulstrum_u.y + z*fulstrum_u.z;
    //printf("dy= %f, theshhold= %f \n", dy, dz*fulstrum_hy);
    if( dy < -dz*(fulstrum_hy+r/2) || dy > dz*(fulstrum_hy+r/2) ) return false;

    return true;
#else
    float dz = x*fulstrum_f.x + y*fulstrum_f.y + z*fulstrum_f.z;
    if( dz + r < 0 || dz > fulstrum_zfar - r ) return false;
    
    float dx = (x*fulstrum_r.x + y*fulstrum_r.y + z*fulstrum_r.z);
    float rx = fulstrum_hx_sphere*r;
    if( dx < -dz*fulstrum_hx - rx|| dx > dz*fulstrum_hx + rx ) return false;
    
    float dy = x*fulstrum_u.x + y*fulstrum_u.y + z*fulstrum_u.z;
    float ry = fulstrum_hx_sphere*r;
    if( dy < -dz*fulstrum_hy - ry || dy > dz*fulstrum_hy + ry ) return false;

    return true;
#endif
}

bool point_fulstrum_test(float x, float y, float  z)
{
    x -= fulstrum_c.x;
    y -= fulstrum_c.y;
    z -= fulstrum_c.z;

    float dz = x*fulstrum_f.x + y*fulstrum_f.y + z*fulstrum_f.z;
    if( dz < 0 || dz > fulstrum_zfar ) return false;

    float dx = (x*fulstrum_r.x + y*fulstrum_r.y + z*fulstrum_r.z);
    if( dx < -dz*fulstrum_hx || dx > dz*fulstrum_hx ) return false;

    float dy = x*fulstrum_u.x + y*fulstrum_u.y + z*fulstrum_u.z;
    if( dy < -dz*fulstrum_hy || dy > dz*fulstrum_hy ) return false;

    return true;
}

bool xy_circle_fulstrum_test(float x, float y, float r)
{
    //float dz = x*fulstrum_f.x + y*fulstrum_f.y;
    //if( dz + r < 0 || dz > fulstrum_zfar - r ) return false;

    x -= fulstrum_c.x;
    y -= fulstrum_c.y;
    
    float dz = x*fulstrum_f.x + y*fulstrum_f.y;

    float dx = x*fulstrum_r.x + y*fulstrum_r.y;
    float rx = fulstrum_hx_sphere*r;
    if( dx < -dz*fulstrum_hx - rx|| dx > dz*fulstrum_hx + rx ) return false;
    
    //float dy = x*fulstrum_u.x + y*fulstrum_u.y;
    //float ry = fulstrum_hx_sphere*r;
    //if( dy < -dz*fulstrum_hy - ry || dy > dz*fulstrum_hy + ry ) return false;

    return true;
}


bool xy_point_fulstrum_test(float x, float y)
{
    x -= fulstrum_c.x;
    y -= fulstrum_c.y;

    if(x*x + y*y <= 16*16) return true;

    float dz = x*fulstrum_f.x + y*fulstrum_f.y;
    if( dz < 0 || dz > fulstrum_zfar ) return false;

    float dx = (x*fulstrum_r.x + y*fulstrum_r.y);
    if( dx < -dz*fulstrum_hx || dx > dz*fulstrum_hx ) return false;

    //float dy = x*fulstrum_u.x + y*fulstrum_u.y + z*fulstrum_u.z;
    //if( dy < -dz*fulstrum_hy || dy > dz*fulstrum_hy ) return false;

    return true;
}


void draw_fulstrum()
{
    //implement later
}
