#include "ray_trace.h"

///ray casting stuff


#define ssize 256
#define bsize 65536

float dummy;

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );
    int x,y,z;
    //int lx,ly,lz; //may or may not be used
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modf(x0, &dummy)*bsize; //convert fractional part
    cy = modf(y0, &dummy)*bsize;
    cz = modf(z0, &dummy)*bsize;

    int _dx,_dy,_dz;
    _dx = (x1-x0)/len *bsize;
    _dy = (y1-y0)/len *bsize;
    _dz = (z1-z0)/len *bsize;

    int cdx, cdy, xdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int i;
    int max_i = (bsize / ssize)*len + 1
    for(i =0; i <= max_i; i++) {
            cx += dx;
            cy += dy;
            cz += dz;

            if(cx >= bsize) {
                cx % bsize;
                //check
            }
            if(cx >= bsize) {
                cx % bsize;
                //check
            }
            if(cx >= bsize) {
                cx % bsize;
                //check
            }

        }
    }

