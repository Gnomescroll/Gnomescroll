#include "ray_trace.h"

///ray casting stuff


//#define ssize 256
//#define bsize 65536

float dummy;

inline int collision_check(int x, int y, int z) {
    //printf("collision check: %i, %i, %i, %i \n", x,y,z,_get(x,y,z));
    return isActive(_get(x,y,z));
}

inline int collision_check2(int x, int y, int z) {
    //printf("collision check: %i, %i, %i, %i \n", x,y,z,_get(x,y,z));
    int tile = _get(x,y,z);
    return isActive(tile);
}

inline int collision_check3(int x, int y, int z) {
    return isActive(_get(x,y,z));
}

inline float dist(float x0,float y0,float z0, float x1,float y1,float z1) {
    return sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );
}

int ri3[4]; //return value
int ri4[3];

/*
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
    for(i =0; i < max_i; i++) {
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
    for(i =0; i < max_i; i++) {
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
    //printf("max_l= %f \n", len);
    //printf("max_i= %i \n", max_i);

    for(i =0; i < max_i; i++) {
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
*/
int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval) {
        float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    //printf("x0,y0,z0= %f, %f, %f \n", x0, y0, z0);
    //printf("cx,cy,cz= %i, %i, %i \n", cx, cy, cz);

    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    //printf("_dx,_dy,_dz= %i, %i, %i \n", _dx, _dy, _dz);
    double xf, yf, zf;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int end = 0;
    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    //printf("max_l= %f \n", len);
    //printf("max_i= %i \n", max_i);
    int side = -1;
    //collision_check(0,0,0);
    for(i =0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                if(collision_check(x,y,z)) {
                    ri4[0] = cdx;
                    break;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                if(collision_check(x,y,z)) {
                    ri4[1] = cdy;
                    break;
                }
            }
            if(cz >= bsize) {
                //printf("z decrease\n");
                cz -= bsize;
                z += cdz;
                if(collision_check(x,y,z)) {
                    ri4[2] = cdz;
                    break;
                }
            }
        }
    }
    //if( max_i - i != 0) {
    //printf("i, max_i= %i, %i, %i \n", i, max_i, max_i - i);
    //}
    *interval = (float)(i) / max_i;
    return ri4;
}


int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, int* tile) {
        float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    //printf("x0,y0,z0= %f, %f, %f \n", x0, y0, z0);
    //printf("cx,cy,cz= %i, %i, %i \n", cx, cy, cz);

    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    //printf("_dx,_dy,_dz= %i, %i, %i \n", _dx, _dy, _dz);
    double xf, yf, zf;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int end = 0;
    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    //printf("max_l= %f \n", len);
    //printf("max_i= %i \n", max_i);
    int side = -1;
    int _c = 0;
    //collision_check(0,0,0);
    for(i =0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                if(collision_check2(x,y,z)) {
                    ri4[0] = cdx;
                    //break;
                    _c = 1;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                if(_c || collision_check2(x,y,z)) {
                    ri4[1] = cdy;
                    //break;
                    _c = 1;
                }
            }
            if(cz >= bsize) {
                //printf("z decrease\n");
                cz -= bsize;
                z += cdz;
                if(_c || collision_check2(x,y,z)) {
                    ri4[2] = cdz;
                    //break;
                    _c = 1;
                }
            }
            if (_c) break;
        }
    }
    //if( max_i - i != 0) {
    //printf("i, max_i= %i, %i, %i \n", i, max_i, max_i - i);
    //}
    collision[0]=x;collision[1]=y;collision[2]=z;
    *tile = _get(x,y,z);
    *interval = (float)(i) / max_i;
    return ri4;
}

int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side) {
    float len2 = sqrt( _dfx*_dfx+_dfy*_dfy+_dfz*_dfz );
    _dfx /= len2;
    _dfy /= len2;
    _dfz /= len2;
    float x1,y1,z1;
    x1 = x0 + _dfx*max_l;
    y1 = y0 + _dfy*max_l;
    z1 = z0 + _dfz*max_l;
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    //int lx,ly,lz; //may or may not be used
    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int _x,_y,_z;
    _x=x;
    _y=y;
    _z=z;

    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    unsigned int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    //printf("_dx,_dy,_dz= %i, %i, %i \n", _dx, _dy, _dz);
    double xf, yf, zf;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project
    //printf("max_l= %f \n", len);
    //printf("max_i= %i \n", max_i);
    side[0]=0; side[1]=0; side[2]=0;
    int col=0;

    for(i =0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                _x = x;
                x += cdx;
                if(collision_check2(x,y,z)) {
                    side[0] = cdx;
                    col =1;
                    break;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                _y = y;
                y += cdy;
                if(collision_check2(x,y,z)) {
                    side[1] = cdy;
                    col=1;
                    break;
                }
            }
            if(cz >= bsize) {
                //printf("z decrease\n");
                cz -= bsize;
                _z = z;
                z += cdz;
                if(collision_check2(x,y,z)) {
                    side[2] = cdz;
                    col=1;
                    break;
                }
            }
        }
    }
    //if( max_i - i != 0) {
    //printf("i, max_i= %i, %i, %i \n", i, max_i, max_i - i);
    //}
    if(col == 1) {
    //side[0]=ri[0]; side[1]=ri[1]; side[2]=ri[2];
    collision[0]=x;collision[1]=y;collision[2]=z;
    pre_collision[0]=_x;pre_collision[1]=_y;pre_collision[2]=_z;
    *tile = _get(x,y,z);
    *distance = len*((float)(i) / max_i);
    return 1;
    } else {
        *tile = 0;
        distance = 0;
        return 0; //no collision
    }
}
