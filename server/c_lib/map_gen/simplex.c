// Copyright (c) 2008, Casey Duncan (casey dot duncan at gmail dot com)
// see LICENSE.txt for details
// $Id$

#include <math.h>

// 2D simplex skew factors
#define F2 0.3660254037844386f  // 0.5 * (sqrt(3.0) - 1.0)
#define G2 0.21132486540518713f // (3.0 - sqrt(3.0)) / 6.0

float 
snoise2(float x, float y) 
{
    int i1, j1, I, J, c;
    float s = (x + y) * F2;
    float i = floorf(x + s);
    float j = floorf(y + s);
    float t = (i + j) * G2;

    float xx[3], yy[3], f[3];
    float noise[3] = {0.0f, 0.0f, 0.0f};
    int g[3];

    xx[0] = x - (i - t);
    yy[0] = y - (j - t);

    i1 = xx[0] > yy[0];
    j1 = xx[0] <= yy[0];

    xx[2] = xx[0] + G2 * 2.0f - 1.0f;
    yy[2] = yy[0] + G2 * 2.0f - 1.0f;
    xx[1] = xx[0] - i1 + G2;
    yy[1] = yy[0] - j1 + G2;

    I = (int) i & 255;
    J = (int) j & 255;
    g[0] = PERM[I + PERM[J]] % 12;
    g[1] = PERM[I + i1 + PERM[J + j1]] % 12;
    g[2] = PERM[I + 1 + PERM[J + 1]] % 12;

    for (c = 0; c <= 2; c++)
        f[c] = 0.5f - xx[c]*xx[c] - yy[c]*yy[c];
    
    for (c = 0; c <= 2; c++)
        if (f[c] > 0)
            noise[c] = f[c]*f[c]*f[c]*f[c] * (GRAD3[g[c]][0]*xx[c] + GRAD3[g[c]][1]*yy[c]);
    
    return (noise[0] + noise[1] + noise[2]) * 70.0f;
}

//float simplex2(float x, float y, int _oct, float _per, float _freq, float _amp, int repeatx, int repeaty, int base)
float simplex2(float x, float y)
// snoise* methods do not use repeat intervals or base. unknown if this is an algorithmic constraint, or lack of implementation
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return snoise2(x,y);
    } else if (_oct > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += snoise2(x * _freq, y * _freq) * _amp;
            max += _amp;
            _freq *= _lac;  // constant?
            _amp *= _per;
        }
        //return (total/max); // why max
        return total;
    }
    return 0.0f;
}

#define dot3(v1, v2) ((v1)[0]*(v2)[0] + (v1)[1]*(v2)[1] + (v1)[2]*(v2)[2])

#define ASSIGN(a, v0, v1, v2) (a)[0] = v0; (a)[1] = v1; (a)[2] = v2;

#define F3 (1.0f / 3.0f)
#define G3 (1.0f / 6.0f)

float 
snoise3(float x, float y, float z) 
{
    int c, o1[3], o2[3], g[4], I, J, K;
    float f[4], noise[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    float s = (x + y + z) * F3;
    float i = floorf(x + s);
    float j = floorf(y + s);
    float k = floorf(z + s);
    float t = (i + j + k) * G3;

    float3 pos[3];

    pos[0].x = x - (i - t);
    pos[0].y = y - (j - t);
    pos[0].z = z - (k - t);

    if (pos[0].x >= pos[0].y) {
        if (pos[0].y >= pos[0].z) {
            ASSIGN(o1, 1, 0, 0);
            ASSIGN(o2, 1, 1, 0);
        } else if (pos[0].x >= pos[0].z) {
            ASSIGN(o1, 1, 0, 0);
            ASSIGN(o2, 1, 0, 1);
        } else {
            ASSIGN(o1, 0, 0, 1);
            ASSIGN(o2, 1, 0, 1);
        }
    } else {
        if (pos[0].x < pos[0].z) {
            ASSIGN(o1, 0, 0, 1);
            ASSIGN(o2, 0, 1, 1);
        } else if (pos[0].x < pos[0].z) {
            ASSIGN(o1, 0, 1, 0);
            ASSIGN(o2, 0, 1, 1);
        } else {
            ASSIGN(o1, 0, 1, 0);
            ASSIGN(o2, 1, 1, 0);
        }
    }
    
    for (c = 0; c <= 2; c++) {
        pos[3].xyz[c] = pos[0].xyz[c] - 1.0f + 3.0f * G3;
        pos[2].xyz[c] = pos[0].xyz[c] - o2[c] + 2.0f * G3;
        pos[1].xyz[c] = pos[0].xyz[c] - o1[c] + G3;
    }

    I = (int) i & 255; 
    J = (int) j & 255; 
    K = (int) k & 255;
    g[0] = PERM[I + PERM[J + PERM[K]]] % 12;
    g[1] = PERM[I + o1[0] + PERM[J + o1[1] + PERM[o1[2] + K]]] % 12;
    g[2] = PERM[I + o2[0] + PERM[J + o2[1] + PERM[o2[2] + K]]] % 12;
    g[3] = PERM[I + 1 + PERM[J + 1 + PERM[K + 1]]] % 12; 

    for (c = 0; c <= 3; c++)
        f[c] = 0.6f - pos[0].xyz[c]*pos[0].xyz[c] - pos[1].xyz[c]*pos[1].xyz[c] - pos[2].xyz[c]*pos[2].xyz[c];
    
    for (c = 0; c <= 3; c++)
        if (f[c] > 0)
            noise[c] = f[c]*f[c]*f[c]*f[c] * dot3(pos[c].xyz, GRAD3[g[c]]);
    
    return (noise[0] + noise[1] + noise[2] + noise[3]) * 32.0f;
}

//float simplex3(float x, float y, float z, int _oct, float _per, float _freq, float _amp, int repeatx, int repeaty, int repeatz, int base)
float simplex3(float x, float y, float z)
// snoise* methods do not use repeat intervals or base. unknown if this is an algorithmic constraint, or lack of implementation
{
    if (_oct == 1) {
        // Single octave, return simple noise
        return snoise3(x,y,z);
    } else if (_oct > 1) {
        int i;
        float total = 0.0f;
        /* max variable was not present in original code, its purpose is unknown. original code was incomplete, however */
        float max = 0.0f;

        for (i = 0; i < _oct; i++) {
            total += snoise3(x * _freq, y * _freq, z * _freq) * _amp;
            max += _amp;
            _freq *= _lac;
            _amp *= _per;
        }
        return (total/max);
    }
    return 0.0f;
}

/* fill methods */
void simplex2_fill(int x, int y) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = simplex2((i+1)/fx,(j+1)/fy);
            noisemap[i + x*j] = h;
        }
    }
}

void simplex3_fill(int x, int y, int z) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = simplex3((i+1)/fx,(j+1)/fy,(k+1)/fz);
                noisemap[i + x*j + x*y*k] = h;
            }
        }
    }
}
