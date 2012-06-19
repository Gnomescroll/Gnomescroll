#include "fulstrum_test2.hpp"

#include <math.h>

#include <physics/vec3.hpp>


class PlaneG  
{

public:

    struct Vec3 normal;
    struct Vec3 point;
    float d;


    //Plane::Plane( Vec3 &v1,  Vec3 &v2,  Vec3 &v3);
    //Plane::Plane(void);
    //Plane::~Plane();

    //void set3Points( Vec3 &v1,  Vec3 &v2,  Vec3 &v3);
    //void setCoefficients(float a, float b, float c, float d);
    
    //3 points define a plane
    void set3Points( Vec3 v1,  Vec3 v2,  Vec3 v3)
    {
        struct Vec3 aux1 = vec3_sub(v1, v2);
        struct Vec3 aux2 = vec3_sub(v3, v2);

        struct Vec3 normal = vec3_cross(aux2, aux1);

        normal = vec3_normalize(normal);
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
    float nearD, farD, ratio, angle,tang;
    float nw,nh,fw,fh;

    void setCamInternals(float angle, float ratio, float nearD, float farD);
    void setCamDef(Vec3 p, Vec3 l, Vec3 u);
    bool pointInFrustum(Vec3 v);
    //int sphereInFrustum(Vec3 &p, float raio);
    //int boxInFrustum(AABox &b);
};


#define ANG2RAD 3.14159265358979323846/180.0

void FrustumG::setCamInternals(float angle, float ratio, float nearD, float farD) 
{
    // store the information
    this->ratio = ratio;
    this->angle = angle;
    this->nearD = nearD;
    this->farD = farD;

    // compute width and height of the near and far plane sections
    tang = (float)tan(ANG2RAD * angle * 0.5) ;
    nh = nearD * tang;
    nw = nh * ratio;
    fh = farD  * tang;
    fw = fh * ratio;
}

void FrustumG::setCamDef(Vec3 p, Vec3 l, Vec3 u) {

    struct Vec3 dir,nc,fc;
    struct Vec3 X,Y,Z;

    // compute the Z axis of camera
    // this axis points in the opposite direction from
    // the looking direction
   
    //Z = p - l;
    //Z.normalize();
    Z = vec3_sub(p, l);
    Z = vec3_normalize(Z);


    // X axis of camera with given "up" vector and Z axis
    //X = u * Z;
    //X.normalize();
    X = vec3_cross(u, Z);
    X = vec3_normalize(X);

    // the real "up" vector is the cross product of Z and X
    //Y = Z * X;

    Y = vec3_cross(Z,X);
    // compute the centers of the near and far planes
    //nc = p - Z * nearD;
    //fc = p - Z * farD;

    nc = vec3_sub(p, vec3_scalar_mult(Z, nearD) );
    // compute the 4 corners of the frustum on the near plane
    

    ntl = vec3_add( vec3_add(nc,vec3_scalar_mult(Y,nh)), vec3_scalar_mult(X,-nw) );
    ntr = vec3_add( vec3_add(nc,vec3_scalar_mult(Y,nh)), vec3_scalar_mult(X,nw) );
    nbl = vec3_add( vec3_add(nc,vec3_scalar_mult(Y,-nh)), vec3_scalar_mult(X,-nw) );
    nbr = vec3_add( vec3_add(nc,vec3_scalar_mult(Y,-nh)), vec3_scalar_mult(X,nw) );
/*
    ntl = nc + Y * nh - X * nw;
    ntr = nc + Y * nh + X * nw;
    nbl = nc - Y * nh - X * nw;
    nbr = nc - Y * nh + X * nw;
*/
    // compute the 4 corners of the frustum on the far plane

    ftl = vec3_add( vec3_add(fc,vec3_scalar_mult(Y,fh)), vec3_scalar_mult(X,-fw) );
    ftr = vec3_add( vec3_add(fc,vec3_scalar_mult(Y,fh)), vec3_scalar_mult(X,fw) );
    fbl = vec3_add( vec3_add(fc,vec3_scalar_mult(Y,-fh)), vec3_scalar_mult(X,-fw) );
    fbr = vec3_add( vec3_add(fc,vec3_scalar_mult(Y,-fh)), vec3_scalar_mult(X,fw) );
/*
    ftl = fc + Y * fh - X * fw;
    ftr = fc + Y * fh + X * fw;
    fbl = fc - Y * fh - X * fw;
    fbr = fc - Y * fh + X * fw;
*/
    // compute the six planes
    // the function set3Points assumes that the points
    // are given in counter clockwise order
/*
    pl[TOP].set3Points(ntr,ntl,ftl);
    pl[BOTTOM].set3Points(nbl,nbr,fbr);
    pl[LEFT].set3Points(ntl,nbl,fbl);
    pl[RIGHT].set3Points(nbr,ntr,fbr);
    pl[NEARP].set3Points(ntl,ntr,nbr);
    pl[FARP].set3Points(ftr,ftl,fbl);
*/
}

bool FrustumG::pointInFrustum(struct Vec3 p) 
{
    for(int i=0; i < 6; i++) 
    {
        if (pl[i].distance(p) < 0)
            return false;
    }
    return true;
}

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