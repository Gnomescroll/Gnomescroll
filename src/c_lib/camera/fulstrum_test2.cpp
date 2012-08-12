#include "fulstrum_test2.hpp"

#include <math.h>

#include <physics/vec3.hpp>

//Hessian Normal Form
//http://mathworld.wolfram.com/HessianNormalForm.html


/* 
    d == 0, except for near and far plane
*/

/*
    If p is 0, d is zero
*/
class PlaneG  
{

public:

    struct Vec3 normal;
    struct Vec3 point;
    float p;    //hessian form

    //3 points define a plane
    void set3Points( Vec3 v1,  Vec3 v2,  Vec3 v3)
    {
        struct Vec3 aux1 = vec3_sub(v1, v2);
        struct Vec3 aux2 = vec3_sub(v3, v2);

        normal = vec3_normalize(vec3_cross(aux1, aux2));

        point = v2;
        p = -vec3_dot(normal,point);
    }

    void setNormalAndPoint(struct Vec3 _normal, struct Vec3 _point)
    {
        this->normal = vec3_normalize(_normal);
        this->point  = _point;
        p = -vec3_dot(normal, point);
    }

    float distance(struct Vec3 x)
    {
        return p + vec3_dot(normal,x);
    }

    float fast_distance(float x, float y, float z)
    {
        return x*normal.x + y*normal.y + z*normal.z;
    }
};

class FrustumG
{

public:

    enum {
        TOP = 0, BOTTOM, LEFT,
        RIGHT, NEARP, FARP
    };

    Vec3 c; //camera position
    PlaneG pl[6];
    Vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
    float nearD, farD, aspect, fov,tang;
    float nw,nh,fw,fh;

    void setCamInternals(float fov, float aspect, float nearD, float farD);
    
    void setCamDef(Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);

    bool pointInFrustum(struct Vec3 p) 
    {
        for(int i=0; i < 6; i++) 
        {
            if (pl[i].distance(p) < 0)
                return false;
        }
        return true;
    }

    bool pointInFulstum_fast(float x, float y, float z)
    {
        for(int i=0; i < 4; i++) 
        {
            if (pl[i].fast_distance(x,y,z) < 0) return false;
        }
        return true;
    }

    bool pointInFulstum_fast2(float x, float y, float z)
    {

        if (pl[TOP].fast_distance(x,y,z) < 0) return false;
        if (pl[BOTTOM].fast_distance(x,y,z) < 0) return false;

        return true;
    }
};


//#define ANG2RAD 3.14159265358979323846/180.0

void FrustumG::setCamInternals(float _fov, float _aspect, float _nearD, float _farD) 
{
    //printf("set camera\n");
    static const float ANG2RAD = 3.14159265358979323846f/180.0f;
    // store the information
    this->aspect = _aspect;
    this->fov = _fov;
    this->nearD = _nearD;
    this->farD  = _farD;

    // compute width and height of the near and far plane sections
    tang = (float)tan(ANG2RAD * fov * 0.5f) ;
    nh = nearD * tang;
    nw = nh * aspect;
    fh = farD  * tang;
    fw = fh * aspect;

    //printf("width= %f height= %f \n", fw, fh);
}

void FrustumG::setCamDef(Vec3 _c, Vec3 f, Vec3 r, Vec3 u)
{
    c = _c;

    struct Vec3 nc,fc;
    //struct Vec3 X,Y,Z;

    //nc = vec3_add(c, vec3_scalar_mult(f, nearD));
    //fc = vec3_add(c, vec3_scalar_mult(f, farD) );
    //ASSUME C = (0, 0, 0)
    nc = vec3_scalar_mult(f, nearD);
    fc = vec3_scalar_mult(f, farD);


    // compute the 4 corners of the frustum on the near plane
    
/*
    ntl = nc + Y * nh - X * nw;
    ntr = nc + Y * nh + X * nw;
    nbl = nc - Y * nh - X * nw;
    nbr = nc - Y * nh + X * nw;
*/

    ntl = vec3_add3( nc, vec3_scalar_mult(u,nh),  vec3_scalar_mult(r,-nw) );
    ntr = vec3_add3( nc, vec3_scalar_mult(u,nh),  vec3_scalar_mult(r,nw)  );
    nbl = vec3_add3( nc, vec3_scalar_mult(u,-nh), vec3_scalar_mult(r,-nw) );
    nbr = vec3_add3( nc, vec3_scalar_mult(u,-nh), vec3_scalar_mult(r,nw)  );
    // compute the 4 corners of the frustum on the far plane

/*
    ftl = fc + Y * fh - X * fw;
    ftr = fc + Y * fh + X * fw;
    fbl = fc - Y * fh - X * fw;
    fbr = fc - Y * fh + X * fw;
*/

    ftl = vec3_add3( fc, vec3_scalar_mult(u,fh), vec3_scalar_mult(r,-fw)  );
    ftr = vec3_add3( fc, vec3_scalar_mult(u,fh), vec3_scalar_mult(r,fw)   );
    fbl = vec3_add3( fc, vec3_scalar_mult(u,-fh), vec3_scalar_mult(r,-fw) );
    fbr = vec3_add3( fc, vec3_scalar_mult(u,-fh), vec3_scalar_mult(r,fw)  );

    // compute the six planes
    // the function set3Points assumes that the points
    // are given in counter clockwise order

    pl[TOP].set3Points(ntr,ntl,ftl);
    pl[BOTTOM].set3Points(nbl,nbr,fbr);
    pl[LEFT].set3Points(ntl,nbl,fbl);
    pl[RIGHT].set3Points(nbr,ntr,fbr);
    pl[NEARP].set3Points(ntl,ntr,nbr);
    pl[FARP].set3Points(ftr,ftl,fbl);


    if(u.z < 0.0) printf("u error: u= %f %f %f \n", u.x, u.y, u.z);
    
    //printf("B Normal= %f %f %f \n", pl[BOTTOM].normal.x, pl[BOTTOM].normal.y, pl[BOTTOM].normal.z);
    //Vec3 f2 = vec3_add(c, vec3_scalar_mult(f, 15)); //15 units in front of camera
    Vec3 f2 = vec3_scalar_mult(f, 15); //15 units in front of camera

    if( pl[TOP].distance(f2) < 0 ) printf("Top\n");
    if( pl[BOTTOM].distance(f2) < 0 ) printf("Bottom\n");
    if( pl[LEFT].distance(f2) < 0 ) printf("Left\n");
    if( pl[RIGHT].distance(f2) < 0 ) printf("Right\n");
    if( pl[NEARP].distance(f2) < 0 ) printf("NearP\n");
    if( pl[FARP].distance(f2) < 0 ) printf("FarP\n");

}


class FrustumG _FrustrumG;

void setup_fulstrum2(float fovy, float aspect, float znear, float zfar, 
    Vec3 camera, Vec3 forward, Vec3 right, Vec3 up)
{
    //setCamInternals(float fov, float aspect, float nearD, float farD);
    //setCamDef(Vec3 p, Vec3 l, Vec3 u);
    _FrustrumG.setCamInternals(fovy, aspect, znear, zfar);

    //forward = vec3_add(camera, forward);
    //up = vec3_add(camera, up);
    //_FrustrumG.setCamDef(camera, forward, up);

    _FrustrumG.setCamDef(camera, forward, right, up);
}


bool point_fulstrum_test_2(float x, float y, float z)
{
    //printf("p= %f \n", _FrustrumG.pl[FrustumG::TOP].p );

    x -= _FrustrumG.c.x;
    y -= _FrustrumG.c.y;
    z -= _FrustrumG.c.z;

    Vec3 p = vec3_init(x,y,z);
    for(int i=0; i < 6; i++) 
    {
        if (_FrustrumG.pl[i].distance(p) < 0)
            return false;
    }
    return true;

/*
    for(int i=0; i < 6; i++) 
    {
        if (_FrustrumG.pl[i].distance(x,y,z) < 0)
            return false;
    }
    return true;
*/
}

bool point_fulstrum_test_2(struct Vec3 p)
{
    p = vec3_sub(p, _FrustrumG.c);
    //printf("d= %f \n", _FrustrumG.pl[FrustumG::TOP].d );

    for(int i=0; i < 6; i++) 
    {
        if (_FrustrumG.pl[i].distance(p) < 0)
            return false;
    }
    return true;
}


/*
    enum {
        TOP = 0, BOTTOM, LEFT,
        RIGHT, NEARP, FARP
    };
*/


//find the highest z level where all points are in fulstrum
float chunk_top_z_projection(float x, float y)
{
    //use binary search
    float z = 128.0f;

    while(_FrustrumG.pointInFulstum_fast(x-8.0,y-8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x-8.0,y+8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x+8.0,y+8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x+8.0,y+8.0,z)
        )
    {

        z-=1.0;

        if(z < 0.0) break;
    }

    return z +1.0;
}

//find the highest z level where all points are in fulstrum
float chunk_bottom_z_projection(float x, float y)
{
    //use binary search
    float z = 0.0f;

    while(_FrustrumG.pointInFulstum_fast(x-8.0,y-8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x-8.0,y+8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x+8.0,y+8.0,z) &&
        _FrustrumG.pointInFulstum_fast(x+8.0,y+8.0,z)
        )
    {

        z += 1.0;

        if(z < 0.0) break;
    }

    return z +1.0;
}


void chunk_top_z_projection(float x, float y, float* bottom, float *top)
{
    x -= _FrustrumG.c.x;
    y -= _FrustrumG.c.y;

    float _z = _FrustrumG.c.z;

    static const float zmax = 128.0;
    static const float zmin = 0.0f;
    float z = 0.0f;


    while(!_FrustrumG.pointInFulstum_fast2(x,y,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x,y+16.0,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x+16.0,y+16.0,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x+16.0,y,z-_z)

        )
    {
        z += 1.0;
        if(z > zmax) return;
    }

    float _bottom = z;


    z = 128.0f;

    while(!_FrustrumG.pointInFulstum_fast2(x,y,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x,y+16.0,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x+16.0,y+16.0,z-_z) &&
        !_FrustrumG.pointInFulstum_fast2(x+16.0,y,z-_z)
        )
    {
        z -= 1.0;
        //if(z < _bottom) break;
        if(z < zmin) break;
    }  


    *bottom = _bottom;
    *top = z;

}