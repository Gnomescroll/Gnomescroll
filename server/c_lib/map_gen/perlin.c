// Borrowed from:

// Copyright (c) 2008, Casey Duncan (casey dot duncan at gmail dot com)
// see LICENSE.txt for details
// $Id$

//#include "Python.h"
#include <math.h>
//#include <stdio.h>
#include "perlin.h"

#ifdef _MSC_VER
#define inline __inline
#endif

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

//static PyObject *
//py_pnoise1(PyObject *self, PyObject *args, PyObject *kwargs)

float perlin1(float x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
{
    //float x;
    //int octaves = 1;
    //float persistence = 0.5f;
    //int repeat = 1024; // arbitrary
    //int base = 0;
    //float freq = 1.0f;
    //float amp = 1.0f;

    if (octaves == 1) {
        // Single octave, return simple noise
        return pnoise1(x, repeat, base);
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += pnoise1(x * frequency, (const int)(repeat * frequency), base) * amplitude;
            max += amplitude;
            frequency *= 2.0f;  // is this supposed to be constant?
            amplitude *= persistence;
        }
        return (total / max);
    } else {    // octaves <1, invalid
        return 0.0f;
    }
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

float perlin2(float x, float y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base)
{
    //float x, y;
    //int octaves = 1;
    //float persistence = 0.5f;
    //float repeatx = 1024; // arbitrary
    //float repeaty = 1024; // arbitrary
    //int base = 0;
    //float freq = 1.0f;
    //float amp = 1.0f;

    if (octaves == 1) {
        // Single octave, return simple noise
        return pnoise2(x,y, repeatx, repeaty, base);
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += pnoise2(x * frequency, y * frequency, repeatx * frequency, repeaty * frequency, base) * amplitude;
            max += amplitude;
            frequency *= 2.0f;  // constant?
            amplitude *= persistence;
        }
        return (total / max);
    } else {
        return 0.0f;
    }
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

float perlin3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base)
{
    //float x, y, z;
    //int octaves = 1;
    //float persistence = 0.5f;
    //int repeatx = 1024; // arbitrary
    //int repeaty = 1024; // arbitrary
    //int repeatz = 1024; // arbitrary
    //int base = 0;
    //float freq = 1.0f;
    //float amp = 1.0f;

    if (octaves == 1) {
        // Single octave, return simple noise
        return  pnoise3(x, y, z, repeatx, repeaty, repeatz, base);
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += pnoise3(x * frequency, y * frequency, z * frequency, 
                (const int)(repeatx*frequency), (const int)(repeaty*frequency), (const int)(repeatz*frequency), base) * amplitude;
            max += amplitude;
            frequency *= 2.0f;  // constant?
            amplitude *= persistence;
        }
        return (total / max);
    } else {
        return 0.0f;
    }
}

//static PyMethodDef perlin_functions[] = {
    //{"pnoise1", (PyCFunction) py_pnoise1, METH_VARARGS | METH_KEYWORDS, 
        //"pnoise1(x, octaves=1, persistence=0.5, repeat=1024, base=0.0)\n\n"
        //"1 dimensional perlin improved noise function (see pnoise3 for more info)"},
    //{"pnoise2", (PyCFunction) py_pnoise2, METH_VARARGS | METH_KEYWORDS, 
        //"pnoise2(x, y, octaves=1, persistence=0.5, repeatx=1024, repeaty=1024, base=0.0)\n\n"
        //"2 dimensional perlin improved noise function (see pnoise3 for more info)"},
    //{"pnoise3", (PyCFunction) py_pnoise3, METH_VARARGS | METH_KEYWORDS, 
        //"pnoise3(x, y, z, octaves=1, persistence=0.5, "
            //"repeatx=1024, repeaty=1024, repeatz=1024, base=0.0)\n\n"
        //"return perlin \"improved\" noise value for specified coordinate\n\n"
        //"octaves -- specifies the number of passes for generating fBm noise,\n"
        //"defaults to 1 (simple noise).\n\n"
        //"persistence -- specifies the amplitude of each successive octave relative\n"
        //"to the one below it. Defaults to 0.5 (each higher octave's amplitude\n"
        //"is halved). Note the amplitude of the first pass is always 1.0.\n\n"
        //"repeatx, repeaty, repeatz -- specifies the interval along each axis when \n"
        //"the noise values repeat. This can be used as the tile size for creating \n"
        //"tileable textures\n\n"
        //"base -- specifies a fixed offset for the input coordinates. Useful for\n"
        //"generating different noise textures with the same repeat interval"},
    //{NULL}
//};

//void
//init_perlin(void)
//{
    //PyObject *m;
    //m = Py_InitModule3("_perlin", perlin_functions, 
        //"Native-code tileable Perlin \"improved\" noise functions");
//}
