#include "ray_trace.hpp"

#include <stdio.h>
#include <math.h>

#include <common/defines.h>

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <physics/common.hpp>

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

#ifdef __MSVC__
#define fmin(x,y) x<y ? x : y
#define fmax(x,y) x>y ? x : y
#endif

static inline int collision_check(int x, int y, int z)
{
    return t_map::isSolid(x,y,z);
}

// called only by ray_cast_simple interfaces
static bool _ray_cast_simple(float x, float y, float z, float a, float b, float c, float len)
{
    int sx,sy,sz;
    sx = (int)x; //truncating conversion
    sy = (int)y;
    sz = (int)z;

    int _dx,_dy,_dz;
    _dx = ((a-x)/len) *ssize;
    _dy = ((b-y)/len) *ssize;
    _dz = ((c-z)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x, &dummy)*bsize : bsize - modff(x, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y, &dummy)*bsize : bsize - modff(y, &dummy)*bsize;
    cz = cdz >=0 ? modff(z, &dummy)*bsize : bsize - modff(z, &dummy)*bsize;

    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall

    for(int i=0; i < max_i; i++)
    {
        cx += dx;
        cy += dy;
        cz += dz;
        if (cx >= bsize || cy >= bsize || cz >= bsize)
        {
            if (cx >= bsize)
            {
                cx -= bsize;
                sx += cdx;
                if (collision_check(sx,sy,sz))
                    return false;
            }
            if (cy >= bsize)
            {
                cy -= bsize;
                sy += cdy;
                if (collision_check(sx,sy,sz))
                    return false;
            }
            if (cz >= bsize)
            {
                cz -= bsize;
                sz += cdz;
                if (collision_check(sx,sy,sz))
                    return false;
            }
        }
    }
    return true;
}

// is there a line between these 2 points, or does it hit the terrain?
bool ray_cast_simple(float x, float y, float z, float a, float b, float c)
{
    float len = distancef(x,y,z,a,b,c);
    return _ray_cast_simple(x,y,z, a,b,c, len);
}

bool ray_cast_simple(float x, float y, float z, float a, float b, float c, float max_distance)
{
    float len = distancef(x,y,z,a,b,c);
    if (len > max_distance)
        return false;
    return _ray_cast_simple(x,y,z, a,b,c, len);
}


static int ri4[3];

//note used
#if 0
int* _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval) {
    float len = sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    max_i = fmin(raycast_tick_max, max_i);

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
                cz -= bsize;
                z += cdz;
                if(collision_check(x,y,z)) {
                    ri4[2] = cdz;
                    break;
                }
            }
        }
    }

    *interval = (float)(i) / max_i;
    return ri4;
}
#endif

void _ray_cast4(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, struct Vec3* v_out)
{
    float len = (float)sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x,y,z;
    x = x0; //truncating conversion
    y = y0;
    z = z0;

    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    max_i = (max_i > raycast_tick_max) ? raycast_tick_max : max_i;

    for(i = 0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                if(collision_check(x,y,z)) {
                    if(cdx == 1)
                    {
                        *v_out = vec3_init(1.0f,0.0f,0.0f);
                    } else 
                    {
                        *v_out = vec3_init(-1.0f,0.0f,0.0f);
                    }
                    ri4[0] = cdx;
                    break;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                if(collision_check(x,y,z)) {
                    if(cdy == 1)
                    {
                        *v_out = vec3_init(0.0f,1.0f,0.0f);
                    } else 
                    {
                        *v_out = vec3_init(0.0f,-1.0f,0.0f);
                    }
                    break;
                }
            }
            if(cz >= bsize) {
                cz -= bsize;
                z += cdz;
                if(collision_check(x,y,z)) {
                    if(cdz == 1)
                    {
                        *v_out = vec3_init(0.0f,0.0f,1.0f);
                    } else 
                    {
                        *v_out = vec3_init(0.0f,0.0f,-1.0f);
                    }
                    break;
                }
            }
        }
    }

    *interval = (float)(i) / max_i;
}

int* _ray_cast5(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile) {
    float len = (float)sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x = (int)x0, //truncating conversion
        y = (int)y0,
        z = (int)z0;

    /* orient the delta step */
    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall

    int _c = 0;
    for(i=0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                x += cdx;
                if(collision_check(x,y,z)) {
                    ri4[0] = cdx;
                    _c = 1;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                y += cdy;
                if(_c || collision_check(x,y,z)) {
                    ri4[1] = cdy;
                    _c = 1;
                }
            }
            if(cz >= bsize) {
                cz -= bsize;
                z += cdz;
                if(_c || collision_check(x,y,z)) {
                    ri4[2] = cdz;
                    _c = 1;
                }
            }
            if (_c) break;
        }
    }

    collision[0]=x;collision[1]=y;collision[2]=z;
    *tile = t_map::get(x,y,z);
    *interval = (float)(i) / max_i;
    return ri4;
}


// loop ticks are capped to raycast_tick_max
//not used
#if 0
int* _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile) {
    float len = sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x = (int)x0, //truncating conversion
        y = (int)y0,
        z = (int)z0;

    /* orient the delta step */
    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    // this adjustment is necessary, but the step may need to be resized
    // if max_i > ray_cast_max.
    if (max_i > raycast_tick_max) {
        printf("WARNING: _ray_cast5_capped :: max_i ticks exceeded tick_max. max_i=%d\n", max_i);
        max_i = raycast_tick_max;
    }

    int _c = 0;
    for(i=0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;

        if(cx >= bsize) {
            cx -= bsize;
            x += cdx;
            if(collision_check(x,y,z)) {
                ri4[0] = cdx;
                _c = 1;
            }
        }
        if(cy >= bsize) {
            cy -= bsize;
            y += cdy;
            if(_c || collision_check(x,y,z)) {
                ri4[1] = cdy;
                _c = 1;
            }
        }
        if(cz >= bsize) {
            cz -= bsize;
            z += cdz;
            if(_c || collision_check(x,y,z)) {
                ri4[2] = cdz;
                _c = 1;
            }
        }
        if (_c) break;
    }

    collision[0]=x;collision[1]=y;collision[2]=z;
    *tile = t_map::get(x,y,z);
    *interval = (float)(i) / max_i;
    return ri4;
}
#endif

// loop ticks are capped to raycast_tick_max
#if 0
void _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile, struct Vec3* v_out) {
    float len = sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x = (int)x0, //truncating conversion
        y = (int)y0,
        z = (int)z0;

    /* orient the delta step */
    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    // this adjustment is necessary, but the step may need to be resized
    // if max_i > ray_cast_max.
    if (max_i > raycast_tick_max) {
        printf("WARNING: _ray_cast5_capped :: max_i ticks exceeded tick_max. max_i=%d\n", max_i);
        max_i = raycast_tick_max;
    }

    int _c = 0;
    for(i=0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;

        if(cx >= bsize) {
            cx -= bsize;
            x += cdx;
            if(collision_check(x,y,z)) {
                _c = 1;
                if(cdx == 1)
                {
                    *v_out = vec3_init(1.0,0.0,0.0);
                } else 
                {
                    *v_out = vec3_init(-1.0,0.0,0.0);
                }
            }
        }
        if(cy >= bsize) {
            cy -= bsize;
            y += cdy;
            if(_c || collision_check(x,y,z)) {
                _c = 1;
                if(cdy == 1)
                {
                    *v_out = vec3_init(0.0,1.0,0.0);
                } else 
                {
                    *v_out = vec3_init(0.0,-1.0,0.0);
                }
            }
        }
        if(cz >= bsize) {
            cz -= bsize;
            z += cdz;
            if(_c || collision_check(x,y,z)) {
                _c = 1;
                if(cdz == 1)
                {
                    *v_out = vec3_init(0.0,0.0,1.0);
                } else 
                {
                    *v_out = vec3_init(0.0,0.0,-1.0);
                }
            }
        }
        if (_c) break;
    }

    collision[0]=x;collision[1]=y;collision[2]=z;
    *tile = t_map::get(x,y,z);
    *interval = (float)(i) / max_i;
}
#endif

int* _ray_cast5_capped(float x0,float y0,float z0, float x1,float y1,float z1, float* interval, int* collision, CubeID* tile, struct Vec3* v_out) {
    float len = (float)sqrtf( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );

    int x = (int)x0, //truncating conversion
        y = (int)y0,
        z = (int)z0;

    /* orient the delta step */
    int _dx,_dy,_dz;
    _dx = ((x1-x0)/len) *ssize;
    _dy = ((y1-y0)/len) *ssize;
    _dz = ((z1-z0)/len) *ssize;

    int cdx, cdy, cdz;
    cdx = _dx >= 0 ? 1 : -1;
    cdy = _dy >= 0 ? 1 : -1;
    cdz = _dz >= 0 ? 1 : -1;

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    ri4[0]=0; ri4[1]=0; ri4[2]=0;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project so we dont end up in wall
    // this adjustment is necessary, but the step may need to be resized
    // if max_i > ray_cast_max.
    if (max_i > raycast_tick_max) {
        printf("WARNING: _ray_cast5_capped :: max_i ticks exceeded tick_max. max_i=%d\n", max_i);
        max_i = raycast_tick_max;
    }

    int _c = 0;
    for(i=0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;

        if(cx >= bsize) {
            cx -= bsize;
            x += cdx;
            if(collision_check(x,y,z)) {
                _c = 1;
                ri4[0] = cdx;
                if(cdx == 1)
                {
                    *v_out = vec3_init(1.0f,0.0f,0.0f);
                } else 
                {
                    *v_out = vec3_init(-1.0f,0.0f,0.0f);
                }
            }
        }
        if(cy >= bsize) {
            cy -= bsize;
            y += cdy;
            if(_c || collision_check(x,y,z)) {
                _c = 1;
                ri4[1] = cdy;
                if(cdy == 1)
                {
                    *v_out = vec3_init(0.0f,1.0f,0.0f);
                } else 
                {
                    *v_out = vec3_init(0.0f,-1.0f,0.0f);
                }
            }
        }
        if(cz >= bsize) {
            cz -= bsize;
            z += cdz;
            if(_c || collision_check(x,y,z)) {
                _c = 1;
                ri4[2] = cdz;
                if(cdz == 1)
                {
                    *v_out = vec3_init(0.0f,0.0f,1.0f);
                } else 
                {
                    *v_out = vec3_init(0.0f,0.0f,-1.0f);
                }
            }
        }
        if (_c) break;
    }

    collision[0]=x;collision[1]=y;collision[2]=z;
    *tile = t_map::get(x,y,z);
    *interval = (float)(i) / max_i;

    return ri4;
}


int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, CubeID* tile, int* side) {
    // normalize direction
    float len2 = (float)sqrtf( _dfx*_dfx+_dfy*_dfy+_dfz*_dfz );
    _dfx /= len2;
    _dfy /= len2;
    _dfz /= len2;

    // calculate endpoint
    float x1,y1,z1;
    x1 = x0 + _dfx*max_l;
    y1 = y0 + _dfy*max_l;
    z1 = z0 + _dfz*max_l;
    // redundant len calculation, will always be max_l
    float len = max_l;

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

    int dx,dy,dz;
    dx = _dx*cdx;
    dy = _dy*cdy;
    dz = _dz*cdz;

    int cx,cy,cz;
    float dummy;
    cx = cdx >=0 ? modff(x0, &dummy)*bsize : bsize - modff(x0, &dummy)*bsize; //convert fractional part
    cy = cdy >=0 ? modff(y0, &dummy)*bsize : bsize - modff(y0, &dummy)*bsize;
    cz = cdz >=0 ? modff(z0, &dummy)*bsize : bsize - modff(z0, &dummy)*bsize;

    int i;
    int max_i = (bsize / ssize)*len + 1; //over project

    side[0]=0; side[1]=0; side[2]=0;
    int col=0;

    for(i=0; i < max_i; i++) {
        cx += dx;
        cy += dy;
        cz += dz;
        if(cx >= bsize || cy >= bsize || cz >= bsize) {
            if(cx >= bsize) {
                cx -= bsize;
                _x = x;
                x += cdx;
                if(collision_check(x,y,z)) {
                    side[0] = cdx;
                    col =1;
                    break;
                }
            }
            if(cy >= bsize) {
                cy -= bsize;
                _y = y;
                y += cdy;
                if(collision_check(x,y,z)) {
                    side[1] = cdy;
                    col=1;
                    break;
                }
            }
            if(cz >= bsize) {
                cz -= bsize;
                _z = z;
                z += cdz;
                if(collision_check(x,y,z)) {
                    side[2] = cdz;
                    col=1;
                    break;
                }
            }
        }
    }
    if(col == 1) {
        side[0] *= -1;
        side[1] *= -1;
        side[2] *= -1;
        collision[0]=x;collision[1]=y;collision[2]=z;
        pre_collision[0]=_x;pre_collision[1]=_y;pre_collision[2]=_z;
        *tile = t_map::get(x,y,z);
        *distance = len * (((float)i) / ((float)max_i));
        return 1;
    } else {
        *tile = EMPTY_CUBE;
        *distance = 0;
        return 0; //no collision
    }
}


/* Formerly raycast_utils.py */
static int ray_cast_block[3];
int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high)
{
    int side[3];
    return _farthest_empty_block(x,y,z,vx,vy,vz,side,max_distance,z_low,z_high);
}

int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, int side[3], float max_distance, int z_low, int z_high)
{   // side si the normal of the solid block that it collided against

    const float inc = 1.0f / RAYCAST_SAMPLING_DENSITY;
    float xy_inc = (float)sqrtf(vx*vx + vy*vy);

    int z_max;
    float z_inc;
    float n = 0.0f;

    int x_, y_, z_;
    int x__, y__, z__;

    if (vz >= 0.0f)
    {
        z_max = z_high;
        z_inc = vz;
    }
    else
    {
        z_max = z_low;
        z_inc = -1.0f*vz;
    }

    while (!(n*xy_inc > max_distance || n*z_inc > z_max))
    {
        n += inc;

        x_ = (int)translate_point(x+vx*n);
        y_ = (int)translate_point(y+vy*n);
        z_ = (int)(z+vz*n);

        x__ = (int)translate_point(x+ vx*(n+inc));
        y__ = (int)translate_point(y+ vy*(n+inc));
        z__ = (int)(z+ vz*(n+inc));

        if (x_ != x__ || y_ != y__ || z_ != z__)
        {
            if (collision_check(x__, y__, z__))
            {
                if (z_ >= z-z_low && z_ <= z+z_high)
                {
                    if (z_ < 0 || z >= map_dim.z) return NULL;
                    ray_cast_block[0] = translate_point(x_);
                    ray_cast_block[1] = translate_point(y_);
                    ray_cast_block[2] = z_;

                    side[0] = x_ - x__;
                    side[1] = y_ - y__;
                    side[2] = z_ - z__;
                    return ray_cast_block;
                }
                break;
            }
        }
    }
    return NULL;
}

int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high) {

    const float inc = 1.0f / RAYCAST_SAMPLING_DENSITY;
    float xy_inc = (float)sqrtf(vx*vx + vy*vy);

    int z_max;
    float z_inc;
    float n = 0.0f;

    int x_, y_, z_;
    int x__, y__, z__;

    if (vz >= 0.0f) {
        z_max = z_high;
        z_inc = vz;
    } else {
        z_max = z_low;
        z_inc = -vz;
    }

    while (!(n*xy_inc > max_distance || n*z_inc > z_max)) {
        n += inc;

        x_ = (int)translate_point(x+vx*n);
        y_ = (int)translate_point(y+vy*n);
        z_ = (int)(z+vz*n);

        x__ = (int)translate_point(x+ vx*(n+inc));
        y__ = (int)translate_point(y+ vy*(n+inc));
        z__ = (int)(z+ vz*(n+inc));

        if (x_ != x__ || y_ != y__ || z_ != z__) {
            if (collision_check(x__, y__, z__)) {
                ray_cast_block[0] = translate_point(x__);
                ray_cast_block[1] = translate_point(y__);
                ray_cast_block[2] = translate_point(z__);
                return ray_cast_block;
            }
        }
    }
    return NULL;
}

inline float sphere_line_distance(float px, float py, float pz, float ox, float oy, float oz, float tx, float ty, float tz, float* pos, float* _rad2)
{
    float t;
    float d;
    float x,y,z;

    tx -= px;
    ty -= py;
    tz -= pz;

    t =  tx*ox + ty*oy + tz*oz; // <tx|ox>

    d = t/(ox*ox+oy*oy+oz*oz); //distance to collision

    x = t*ox - tx;
    y = t*oy - ty;
    z = t*oz - tz;
    *_rad2 = x*x+y*y+z*z; // minimum distance squared between target and line

    //x,y,z is closest point
    x = t*ox + px;
    y = t*oy + py;
    z = t*oz + pz;

    pos[0] = x;
    pos[1] = y;
    pos[2] = z;

    return d;
}

int get_cube_side_from_side_array(int* side)
{
/*
    int cube_side = -1;
    if (side[0] ==  1) cube_side = 2;
    else if (side[0] == -1) cube_side = 3;
    else if (side[1] ==  1) cube_side = 4;
    else if (side[1] == -1) cube_side = 5;
    else if (side[2] ==  1) cube_side = 0;
    else if (side[2] == -1) cube_side = 1;

    GS_ASSERT(cube_side != -1);
    return cube_side;
*/
    if (side[0] ==  1) return 2;
    if (side[0] == -1) return 3;
    if (side[1] ==  1) return 4;
    if (side[1] == -1) return 5;
    if (side[2] ==  1) return 0;
    if (side[2] == -1) return 1;

    GS_ASSERT(false);
    return 0;
}

void get_side_array_from_cube_side(int cube_id, int *side)
{
    side[0]=side[1]=0;
    side[2]=1;
    switch (cube_id)
    {
        case 2:
            side[0] = 1;
            break;
        case 3:
            side[0] = -1;
            break;
        case 4:
            side[1] = 1;
            break;
        case 5:
            side[1] = -1;
            break;
        case 0:
            side[2] = 1;
            break;
        case 1:
            side[2] = -1;
            break;
        default:
            GS_ASSERT(false);
            return;
    }
}
