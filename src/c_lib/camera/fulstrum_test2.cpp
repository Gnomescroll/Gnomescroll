#include "fulstrum_test2.hpp"

#include <math.h>

#include <physics/vec3.hpp>

class PlaneG  
{

public:

    struct Vec3 normal;
    struct Vec3 point;
    float d;

    //3 points define a plane
    void set3Points( Vec3 v1,  Vec3 v2,  Vec3 v3)
    {
        struct Vec3 aux1 = vec3_sub(v1, v2);
        struct Vec3 aux2 = vec3_sub(v3, v2);

        normal = vec3_normalize(vec3_cross(aux1, aux2));

        point = v2;
        d = -vec3_dot(normal,point);
    }

    void setNormalAndPoint(struct Vec3 _normal, struct Vec3 _point)
    {
        this->normal = vec3_normalize(_normal);
        this->point  = _point;
        d = -vec3_dot(normal, point);
    }

    float distance(struct Vec3 p)
    {
        return d + vec3_dot(normal,p);
    }

};

class FrustumG
{

private:

    enum {
        TOP = 0, BOTTOM, LEFT,
        RIGHT, NEARP, FARP
    };

public:

    //static enum {OUTSIDE, INTERSECT, INSIDE};
    //Vec3 pln[6]; //plane normal
    //Vec3 plp[6]; //plane position

    PlaneG pl[6];
    Vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
    float nearD, farD, aspect, fov,tang;
    float nw,nh,fw,fh;

    void setCamInternals(float fov, float aspect, float nearD, float farD);
    
    void setCamDef(Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);
    //void setCamDef(Vec3 p, Vec3 l, Vec3 u);
    
    //bool pointInFrustum(Vec3 v);
    //int sphereInFrustum(Vec3 &p, float raio);
    //int boxInFrustum(AABox &b);

    bool pointInFrustum(struct Vec3 p) 
    {
        for(int i=0; i < 6; i++) 
        {
            if (pl[i].distance(p) < 0)
                return false;
        }
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

void FrustumG::setCamDef(Vec3 c, Vec3 f, Vec3 r, Vec3 u)
//void FrustumG::setCamDef(Vec3 p, Vec3 l, Vec3 u) 
{
/*
    float f1,f2, f3;

    f1 = vec3_dot(f,r);
    f2 = vec3_dot(f,u);
    f3 = vec3_dot(r,u);   

    printf("0: %f %f %f \n", f1,f2,f3);

    f1 = vec3_dot(f,f);
    f2 = vec3_dot(r,r);
    f3 = vec3_dot(u,u);   

    printf("1: %f %f %f \n", f1,f2,f3);
*/
#if 1

    struct Vec3 nc,fc;
    //struct Vec3 X,Y,Z;

    nc = vec3_add(c, vec3_scalar_mult(f, nearD));
    fc = vec3_add(c, vec3_scalar_mult(f, farD) );
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

//debugging
#if 1

    if(u.z < 0.0) printf("u error: u= %f %f %f \n", u.x, u.y, u.z);
    
    //printf("B Normal= %f %f %f \n", pl[BOTTOM].normal.x, pl[BOTTOM].normal.y, pl[BOTTOM].normal.z);

    Vec3 f2 = vec3_add(c, vec3_scalar_mult(f, 15)); //15 units in front of camera

    if( pl[TOP].distance(f2) < 0 ) printf("Top\n");
    if( pl[BOTTOM].distance(f2) < 0 ) printf("Bottom\n");
    if( pl[LEFT].distance(f2) < 0 ) printf("Left\n");
    if( pl[RIGHT].distance(f2) < 0 ) printf("Right\n");
    if( pl[NEARP].distance(f2) < 0 ) printf("NearP\n");
    if( pl[FARP].distance(f2) < 0 ) printf("FarP\n");
#endif

#else
    nc = vec3_add(c, vec3_scalar_mult(f, nearD));
    fc = vec3_add(c, vec3_scalar_mult(f, farD) );

    pl[NEARP].setNormalAndPoint(vec3_scalar_mult(f, -1), nc);
    pl[FARP].setNormalAndPoint(f,fc);

    Vec3 aux,normal;
/*
    aux = (nc + Y*nh) - p;
    aux.normalize();
    normal = aux * X;
    pl[TOP].setNormalAndPoint(normal,nc+Y*nh);

    aux = (nc - Y*nh) - p;
    aux.normalize();
    normal = X * aux;
    pl[BOTTOM].setNormalAndPoint(normal,nc-Y*nh);

    aux = (nc - X*nw) - p;
    aux.normalize();
    normal = aux * Y;
    pl[LEFT].setNormalAndPoint(normal,nc-X*nw);

    aux = (nc + X*nw) - p;
    aux.normalize();
    normal = Y * aux;
    pl[RIGHT].setNormalAndPoint(normal,nc+X*nw);
*/

    aux = vec3_sub(vec3_add(nc, vec3_scalar_mult(u, nh)), c);
    aux = vec3_normalize(aux);
    normal = vec3_cross(aux, r);
    pl[TOP].setNormalAndPoint(normal, vec3_add(nc, vec3_scalar_mult(u, nh)));

    aux = vec3_sub(vec3_add(nc, vec3_scalar_mult(u, nh)), c);
    aux = vec3_normalize(aux);
    normal = vec3_cross(aux, r);
    pl[TOP].setNormalAndPoint(normal, vec3_add(nc, vec3_scalar_mult(u, nh)));

    aux = vec3_sub(vec3_add(nc, vec3_scalar_mult(u, nh)), c);
    aux = vec3_normalize(aux);
    normal = vec3_cross(aux, r);
    pl[TOP].setNormalAndPoint(normal, vec3_add(nc, vec3_scalar_mult(u, nh)));

    aux = vec3_sub(vec3_add(nc, vec3_scalar_mult(u, nh)), c);
    aux = vec3_normalize(aux);
    normal = vec3_cross(aux, r);
    pl[TOP].setNormalAndPoint(normal, vec3_add(nc, vec3_scalar_mult(u, nh)));

#endif
}

#if 0
    struct Vec3 nc,fc;
    struct Vec3 X,Y,Z;

    // compute the Z axis of camera
    // this axis points in the opposite direction from
    // the looking direction
   
    //Z = p - l;
    //Z.normalize();
    Z = vec3_normalize(vec3_sub(p, l));


    X = vec3_normalize(vec3_cross(u, Z));

    Y = vec3_cross(Z,X);

    nc = vec3_sub(p, vec3_scalar_mult(Z, nearD));
    fc = vec3_sub(p, vec3_scalar_mult(Z, farD) );
    // compute the 4 corners of the frustum on the near plane
    
/*
    ntl = nc + Y * nh - X * nw;
    ntr = nc + Y * nh + X * nw;
    nbl = nc - Y * nh - X * nw;
    nbr = nc - Y * nh + X * nw;
*/

    ntl = vec3_add3( nc, vec3_scalar_mult(Y,nh),  vec3_scalar_mult(X,-nw) );
    ntr = vec3_add3( nc, vec3_scalar_mult(Y,nh),  vec3_scalar_mult(X,nw)  );
    nbl = vec3_add3( nc, vec3_scalar_mult(Y,-nh), vec3_scalar_mult(X,-nw) );
    nbr = vec3_add3( nc, vec3_scalar_mult(Y,-nh), vec3_scalar_mult(X,nw)  );
    // compute the 4 corners of the frustum on the far plane

/*
    ftl = fc + Y * fh - X * fw;
    ftr = fc + Y * fh + X * fw;
    fbl = fc - Y * fh - X * fw;
    fbr = fc - Y * fh + X * fw;
*/
    ftl = vec3_add3( fc, vec3_scalar_mult(Y,fh), vec3_scalar_mult(X,-fw)  );
    ftr = vec3_add3( fc, vec3_scalar_mult(Y,fh), vec3_scalar_mult(X,fw)   );
    fbl = vec3_add3( fc, vec3_scalar_mult(Y,-fh), vec3_scalar_mult(X,-fw) );
    fbr = vec3_add3( fc, vec3_scalar_mult(Y,-fh), vec3_scalar_mult(X,fw)  );

    // compute the six planes
    // the function set3Points assumes that the points
    // are given in counter clockwise order

    pl[TOP].set3Points(ntr,ntl,ftl);
    pl[BOTTOM].set3Points(nbl,nbr,fbr);
    pl[LEFT].set3Points(ntl,nbl,fbl);
    pl[RIGHT].set3Points(nbr,ntr,fbr);
    pl[NEARP].set3Points(ntl,ntr,nbr);
    pl[FARP].set3Points(ftr,ftl,fbl);
#endif

/*
int FrustumG::sphereInFrustum(Vec3 &p, float radius) {

    float distance;
    int result = INSIDE;

    for(int i=0; i < 6; i++) {
        distance = pl[i].distance(p);
        if (distance < -radius)
            return OUTSIDE;
        else if (distance < radius)
            result =  INTERSECT;
    }
    return(result);
}
*/

/*
pl[NEARP].setNormalAndPoint(-Z,nc);
    pl[FARP].setNormalAndPoint(Z,fc);

    Vec3 aux,normal;

    aux = (nc + Y*nh) - p;
    aux.normalize();
    normal = aux * X;
    pl[TOP].setNormalAndPoint(normal,nc+Y*nh);

    aux = (nc - Y*nh) - p;
    aux.normalize();
    normal = X * aux;
    pl[BOTTOM].setNormalAndPoint(normal,nc-Y*nh);

    aux = (nc - X*nw) - p;
    aux.normalize();
    normal = aux * Y;
    pl[LEFT].setNormalAndPoint(normal,nc-X*nw);

    aux = (nc + X*nw) - p;
    aux.normalize();
    normal = Y * aux;
    pl[RIGHT].setNormalAndPoint(normal,nc+X*nw);
*/

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

float top_z_projection(float x, float y)
{
    struct Vec3 n = _FrustrumG.pl[0].normal;
    float d = _FrustrumG.pl[0].d;
    d = 0;
    return (x*n.x + y*n.y + d)/(-n.z);
}

float bottom_z_projection(float x, float y)
{
    struct Vec3 n = _FrustrumG.pl[1].normal;
    float d = _FrustrumG.pl[1].d;
    d = 0;
    return (x*n.x + y*n.y + d)/(-n.z);
}