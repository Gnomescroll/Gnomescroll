#include "ray_trace.h"

///ray casting stuff


#define ssize 128
#define bsize 16384

float dummy;

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );
    int x,y,z;
    x = x0;
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modf(x0, &dummy)*bsize;
    cy = modf(y0, &dummy)*bsize;
    cz = modf(z0, &dummy)*bsize;

    int dx,dy,dz;
    dx = (x1-x0)/len *bsize;
    dy = (y1-y0)/len *bsize;
    dz = (z1-z0)/len *bsize;

    int i;


    }

