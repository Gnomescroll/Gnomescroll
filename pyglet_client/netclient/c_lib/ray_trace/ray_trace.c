#include "ray_trace.h"

///ray casting stuff


//#define ssize 256
//#define bsize 65536

float dummy;

inline int collision_check(int x, int y, int z) {
    return isActive(_get(x,y,z));
}

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion from float to int
    y = y0;
    z = z0;

    unsigned int cx,cy,cz;
    cx = modff(x0, &dummy)*bsize; //convert fractional part
    cy = modff(y0, &dummy)*bsize;
    cz = modff(z0, &dummy)*bsize;

    int _dx,_dy,_dz;
    _dx = (x1-x0)/len *bsize;
    _dy = (y1-y0)/len *bsize;
    _dz = (z1-z0)/len *bsize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    for(i =0; i <= max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cx >= bsize || cx >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                 ///do collision stuff
                collision_check(x,y,z);
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                 ///do collision stuff
                collision_check(x,y,z);
            }
            if(cz >= bsize) {
                cz -= bsize;
                z += cdz;
                 ///do collision stuff
                collision_check(x,y,z);
            }
        }
    }
}

//laxer version
int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modff(x0, &dummy)*bsize; //convert fractional part
    cy = modff(y0, &dummy)*bsize;
    cz = modff(z0, &dummy)*bsize;

    int _dx,_dy,_dz;
    _dx = (x1-x0)/len *bsize;
    _dy = (y1-y0)/len *bsize;
    _dz = (z1-z0)/len *bsize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    for(i =0; i <= max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cx >= bsize || cx >= bsize) {
            if(cx >= bsize) { cx -= bsize; x += cdx;}
            if(cy >= bsize) { cy -= bsize; y += cdy;}
            if(cz >= bsize) { cz -= bsize; z += cdz;}
            collision_check(x,y,z); ///do collision stuff
        }
    }
}

inline int collision_check3(int x, int y, int z) {
    return isActive(_get(x,y,z));
}

inline float dist(float x0,float y0,float z0, float x1,float y1,float z1) {
    return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );
}

int ri3[4]; //return value

int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modff(x0, &dummy)*bsize; //convert fractional part
    cy = modff(y0, &dummy)*bsize;
    cz = modff(z0, &dummy)*bsize;

    int _dx,_dy,_dz;
    _dx = (x1-x0)/len *bsize;
    _dy = (y1-y0)/len *bsize;
    _dz = (z1-z0)/len *bsize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    double xf, yf, zf;
    *distance =0;
    ri3[0]=0; ri3[1]=0; ri3[2]=0;

    int end = 0;
    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    //printf("length= %f \n", len);
    //printf("max_i= %i \n", max_i);

    for(i =0; i <= max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cx >= bsize || cx >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                 ///do collision stuff
                if(collision_check3(x,y,z)) {
                    end = 1;
                    ri3[0] = cdx;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                 ///do collision stuff
                if(collision_check3(x,y,z)) {
                    end = 1;
                    ri3[1] = cdy;
                }
            }
            if(cz >= bsize) {
                cz -= bsize;
                z += cdz;
                 ///do collision stuff
                if(collision_check3(x,y,z)) {
                    end = 1;
                    ri3[2] = cdz;
                }
            }
            if(end == 1) {
                xf =  (double) (cdx == 1 ? cx-dx : bsize-cx-dx) / bsize + x;
                yf =  (double) (cdy == 1 ? cy-dy : bsize-cy-dy) / bsize + y;
                zf =  (double) (cdz == 1 ? cz-dz : bsize-cz-dz) / bsize + z;
                *distance = dist(x0,y0,z0,xf,yf,zf);
                return ri3;
            }
        }
    }
    //printf("No collision \n");
    *distance = 0;
    return &ri3;
}

int ri4[3];

int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval) {
        float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modff(x0, &dummy)*bsize; //convert fractional part
    cy = modff(y0, &dummy)*bsize;
    cz = modff(z0, &dummy)*bsize;

    int _dx,_dy,_dz;
    _dx = (x1-x0)/len *bsize;
    _dy = (y1-y0)/len *bsize;
    _dz = (z1-z0)/len *bsize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    double xf, yf, zf;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int end = 0;
    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    //printf("length= %f \n", len);
    //printf("max_i= %i \n", max_i);
    int side = -1;
    for(i =0; i <= max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cx >= bsize || cx >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                if(collision_check3(x,y,z)) {
                    ri4[0] = cdx;
                    break;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                if(collision_check3(x,y,z)) {
                    ri4[1] = cdy;
                    break;
                }
            }
            if(cz >= bsize) {
                cz -= bsize;
                z += cdz;
                if(collision_check3(x,y,z)) {
                    ri4[2] = cdz;
                    break;
                }
            }
        }
    }
    *percentage = float(i) / max_i;
    return ri4;
}
