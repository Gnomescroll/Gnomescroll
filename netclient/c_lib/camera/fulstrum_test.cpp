#include "fulstrum_test.hpp"

#include <math.h>

#include <c_lib/physics/vector.hpp>

void setup_test_fulstrum(int camera)
{}

//static float fulstrum_fovy;
//static float fulstrum_fovx;
//static float fulstrum_aspect;
static float fulstrum_zfar;

static float fulstrum_hx;
static float fulstrum_hy;

static struct Vector fulstrum_c; //camera
static struct Vector fulstrum_f;   //forward
static struct Vector fulstrum_u;   //up
static struct Vector fulstrum_r;   //right

void setup_fulstrum(float fovy, float aspect, float zfar, Vector camera, Vector* forward, Vector *right, Vector* up)
{
    const float pi = 3.14159265;

    fovy *= (pi/180);

    fulstrum_hy = tan(fovy/2);
    fulstrum_hx = fulstrum_hy*aspect;

    fulstrum_zfar = zfar;

    fulstrum_c = camera;
    fulstrum_f = *forward;
    fulstrum_r = *right;
    fulstrum_u = *up;
}


bool sphere_fulstrum_test(float x, float y, float  z, float r)
{
    x -= fulstrum_c.x;
    y -= fulstrum_c.y;
    z -= fulstrum_c.z;

    float dz = x*fulstrum_f.x + y*fulstrum_f.y + z*fulstrum_f.z;
    //printf("dz= %f, radius= %f \n", dz,r);
    if( dz < 0 || dz > fulstrum_zfar ) return false;

    float dx = (x*fulstrum_r.x + y*fulstrum_r.y + z*fulstrum_r.z);
    //printf("dx= %f, theshhold= %f \n", dx, dz*fulstrum_hx);
    if( dx < -dz*fulstrum_hx || dx > dz*fulstrum_hx ) return false;

    float dy = x*fulstrum_u.x + y*fulstrum_u.y + z*fulstrum_u.z;
    //printf("dy= %f, theshhold= %f \n", dy, dz*fulstrum_hy);
    if( dy < -dz*fulstrum_hy || dy > dz*fulstrum_hy ) return false;

    return true;
}


void draw_fulstrum()
{
    //implement later
}
