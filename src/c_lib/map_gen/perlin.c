// Borrowed from:

// Copyright (c) 2008, Casey Duncan (casey dot duncan at gmail dot com)
// see LICENSE.txt for details
// $Id$

#include <math.h>
#include "perlin.h"

#ifdef _MSC_VER
#define inline __inline
#endif

// Body of a perlin noise wrapper
#define PERLINX_BODY(pnoise) do {\
    float amplitude = _amp, \
           frequency = _freq;   \
    int i;  \
    float max = 0.0f;   \
    float total = 0.0f; \
    for (i = 0; i < _oct; i++)  \
    {   \
        total += pnoise;    \
        max += amplitude;   \
        frequency *= _lac;  \
        amplitude *= _per;  \
    }   \
    return (total / max);   \
    }while(0)

#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

float inline
grad1(const int hash, const float x)
{
    float g = (hash & 7) + 1.0f;
    if (hash & 8)
        g = -1;
    return (g * x);
}

float
pnoise1(float x, const int repeat, const int base)
{
    float fx;
    int i = (int)floorf(x) % repeat;
    int ii = (i + 1) % repeat;
    i = (i & 255) + base;
    ii = (ii & 255) + base;

    x -= floorf(x);
    fx = x*x*x * (x * (x * 6 - 15) + 10);

    return lerp(fx, grad1(PERM[i], x), grad1(PERM[ii], x - 1)) * 0.4f;
}

float perlin1(float x, int repeat, int base)
{
    PERLINX_BODY(
        pnoise1(x * frequency, (const int)(repeat * frequency), base) * amplitude
    );
}

float inline
grad2(const int hash, const float x, const float y)
{
    const int h = hash & 15;
    return x * GRAD3[h][0] + y * GRAD3[h][1];
}

float
pnoise2(float x, float y, const float repeatx, const float repeaty, const int base)
{
    float fx, fy;
    int A, AA, AB, B, BA, BB;
    int i = (int)floorf(fmodf(x, repeatx));
    int j = (int)floorf(fmodf(y, repeaty));
    int ii = (int)fmodf(i + 1, repeatx);
    int jj = (int)fmodf(j + 1, repeaty);
    i += base; j += base;
    ii += base; jj += base;

    x -= floorf(x); y -= floorf(y);
    fx = x*x*x * (x * (x * 6 - 15) + 10);
    fy = y*y*y * (y * (y * 6 - 15) + 10);

    A = PERM[i];
    AA = PERM[A + j];
    AB = PERM[A + jj];
    B = PERM[ii];
    BA = PERM[B + j];
    BB = PERM[B + jj];

    return lerp(fy, lerp(fx, grad2(PERM[AA], x, y),
                             grad2(PERM[BA], x - 1, y)),
                    lerp(fx, grad2(PERM[AB], x, y - 1),
                             grad2(PERM[BB], x - 1, y - 1)));
}

float perlin2(float x, float y, int repeatx, int repeaty, int base)
{
    PERLINX_BODY(
        pnoise2(x * frequency, y * frequency, repeatx * frequency, repeaty * frequency, base) * amplitude
    );
}

//def noise(x, y, per):
    //def surflet(gridX, gridY):
        //distX, distY = abs(x-gridX), abs(y-gridY)
        //polyX = 1 - 6*distX**5 + 15*distX**4 - 10*distX**3
        //polyY = 1 - 6*distY**5 + 15*distY**4 - 10*distY**3
        //hashed = perm[perm[int(gridX)%per] + int(gridY)%per]
        //grad = (x-gridX)*dirs[hashed][0] + (y-gridY)*dirs[hashed][1]
        //return polyX * polyY * grad
    //intX, intY = int(x), int(y)
    //return (surflet(intX+0, intY+0) + surflet(intX+1, intY+0) +
            //surflet(intX+0, intY+1) + surflet(intX+1, intY+1))

float p2_surflet(float x, float y, int gx, int gy, int period)
{
    // distance from grid points
    float dx = abs(x-gx);
    float dy = abs(y-gy);
    // interpolated
    float px = 1.0f - 6.0f*(float)pow(dx,5.0f) + 15.0f*(float)pow(dx,4.0f) - 10.0f*(float)pow(dx,3.0f);
    float py = 1.0f - 6.0f*(float)pow(dy,5.0f) + 15.0f*(float)pow(dy,4.0f) - 10.0f*(float)pow(dy,3.0f);
    // gradient index
    int hash = tiling_indexes[tiling_indexes[gx%period] + gy%period];
    float g = (x-gx)*tiling_gradients[hash].x + (y-gy)*tiling_gradients[hash].y;
    return px * py * g;
}

float pnoise2_tiling(float x, float y, int period)
{
    int ix = (int)x;
    int iy = (int)y;
    return   p2_surflet(x,y, ix+0,iy+0, period)
            + p2_surflet(x,y, ix+1,iy+0, period)
            + p2_surflet(x,y, ix+0,iy+1, period)
            + p2_surflet(x,y, ix+1,iy+1, period);
}

float perlin2_tiling(float x, float y, int period)
{
    //assert(_oct >= 1);
    assert(_oct == 1);
    float total = 0.0f;
    for (int i=0; i<_oct; i++)  
        total += (float)pow(0.5, i) * pnoise2_tiling(x * (float)pow(2,i), y * (float)pow(2,i), period*(float)pow(2,i));
    return total;
}

float inline
grad3(const int hash, const float x, const float y, const float z)
{
    const int h = hash & 15;
    return x * GRAD3[h][0] + y * GRAD3[h][1] + z * GRAD3[h][2];
}

float
pnoise3(float x, float y, float z, const int repeatx, const int repeaty, const int repeatz, 
    const int base)
{
    float fx, fy, fz;
    int A, AA, AB, B, BA, BB;
    int i = (int)floorf(fmodf(x, repeatx));
    int j = (int)floorf(fmodf(y, repeaty));
    int k = (int)floorf(fmodf(z, repeatz));
    int ii = (int)fmodf(i + 1,  repeatx);
    int jj = (int)fmodf(j + 1, repeaty);
    int kk = (int)fmodf(k + 1, repeatz);
    i += base; j += base; k += base;
    ii += base; jj += base; kk += base;

    x -= floorf(x); y -= floorf(y); z -= floorf(z);
    fx = x*x*x * (x * (x * 6 - 15) + 10);
    fy = y*y*y * (y * (y * 6 - 15) + 10);
    fz = z*z*z * (z * (z * 6 - 15) + 10);

    A = PERM[i];
    AA = PERM[A + j];
    AB = PERM[A + jj];
    B = PERM[ii];
    BA = PERM[B + j];
    BB = PERM[B + jj];
        
    return lerp(fz, lerp(fy, lerp(fx, grad3(PERM[AA + k], x, y, z),
                                      grad3(PERM[BA + k], x - 1, y, z)),
                             lerp(fx, grad3(PERM[AB + k], x, y - 1, z),
                                      grad3(PERM[BB + k], x - 1, y - 1, z))),
                    lerp(fy, lerp(fx, grad3(PERM[AA + kk], x, y, z - 1),
                                      grad3(PERM[BA + kk], x - 1, y, z - 1)),
                             lerp(fx, grad3(PERM[AB + kk], x, y - 1, z - 1),
                                      grad3(PERM[BB + kk], x - 1, y - 1, z - 1))));
}

float perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base)
{
    PERLINX_BODY(
        pnoise3(
            x * frequency, y * frequency, z * frequency,
            repeatx * frequency, repeaty * frequency, repeatz * frequency,
            base)
        * amplitude
    );
}

/* fill methods */
void perlin1_fill(float* noisemap, int x, int repeat, int base)
{
    float fx = (float)x + 2.0f; // padding
    for (int i=0; i<x; i++)
        noisemap[i] = perlin1(((float)(i+1)/fx)*xnoise_scale, repeat, base);
}

void perlin2_fill(float* noisemap, int x, int y, int repeatx, int repeaty, int base)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
        noisemap[i + x*j] = perlin2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale, repeatx, repeaty, base);

    //float ifreq = 512.0f;    // inverse freq
    //int size = 512;
    //int iters = 512 / size;
    //assert(iters == 1);
    //for (int m=0; m<iters; m++)
    //for (int n=0; n<iters; n++)
    //for (int i=0; i<size; i++)
    //for (int j=0; j<size; j++)
    //{
        //float fi = ((float)i) / ifreq;
        //float fj = ((float)j) / ifreq;
        //int period = ((float)size) / ifreq;
        //float val = perlin2_tiling(fi, fj, period);
        ////printf("val %0.2f\n", val);
        ////noisemap[(i+m*size) + size*(j+n*size)] = val;
        //noisemap[i + x*j] = val;
    //}
}

void perlin3_fill(float* noisemap, int x, int y, int z, int repeatx, int repeaty, int repeatz, int base)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    for (int k=0; k<z; k++)
        noisemap[i + x*j + x*y*k] = perlin3(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale,((float)(k+1)/fz)*znoise_scale, repeatx, repeaty, repeatz, base);
}
