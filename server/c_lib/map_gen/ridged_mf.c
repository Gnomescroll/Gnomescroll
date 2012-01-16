/*
 * Ridged multifractal noise
 * -same as perlin or simplex noise except one thing
 * -each octave of noise is calculated as 1.0 - abs(noise(x,...))
 */

#include "ridged_mf.h"


float rmf_perlin1(float x, int repeat, int base)
{
    float amplitude = _amp,
           frequency = _freq;

    int i;
    float max = 0.0f;
    float total = 0.0f;

    for (i = 0; i < _oct; i++) {
        total += 1.0f - fabs(pnoise1(x * frequency, (const int)(repeat * frequency), base) * amplitude);
        max += amplitude;
        frequency *= _lac;
        amplitude *= _per;
    }
    return (total / max);
}

float rmf_perlin2(float x, float y, int repeatx, int repeaty, int base)
{
    float amplitude = _amp,
           frequency = _freq;

    int i;
    float max = 0.0f;
    float total = 0.0f;

    for (i = 0; i < _oct; i++) {
        total += 1.0f - fabs(pnoise2(x * frequency, y * frequency, repeatx * frequency, repeaty * frequency, base) * amplitude);
        max += amplitude;
        frequency *= _lac;
        amplitude *= _per;
    }
    return (total / max);
}

float rmf_perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base)
{
    float amplitude = _amp,
           frequency = _freq;

    int i;
    float max = 0.0f;
    float total = 0.0f;

    for (i = 0; i < _oct; i++) {
        total += 1.0f - fabs(pnoise3(x * frequency, y * frequency, z * frequency, 
            (const int)(repeatx*frequency), (const int)(repeaty*frequency), (const int)(repeatz*frequency), base) * amplitude);
        max += amplitude;
        frequency *= _lac;
        amplitude *= _per;
    }
    return (total / max);
}


float rmf_simplex2(float x, float y)
{
    float amplitude = _amp,
           frequency = _freq;

    int i;
    float max = 0.0f;
    float total = 0.0f;

    for (i = 0; i < _oct; i++) {
        total += 1.0f - fabs(snoise2(x * frequency, y * frequency) * amplitude);
        max += amplitude;
        frequency *= _lac;
        amplitude *= _per;
    }
    return (total/max); // why max
}

float rmf_simplex3(float x, float y, float z)
{
    float amplitude = _amp,
           frequency = _freq;

    int i;
    float total = 0.0f;
    float max = 0.0f;

    for (i = 0; i < _oct; i++) {
        total += 1.0f - fabs(snoise3(x * frequency, y * frequency, z * frequency) * amplitude);
        max += amplitude;
        frequency *= _lac;
        amplitude *= _per;
    }
    return (total/max);
}

/* fill methods */
void rmf_perlin1_fill(int x, int repeat, int base) {
    float fx = (float)x + 2.0f;
    int i;
    float h;
    for (i=0; i<x; i++) {
        h = rmf_perlin1(((float)(i+1)/fx)*xnoise_scale, repeat, base);
        noisemap[i] = h;
    }
}

void rmf_perlin2_fill(int x, int y, int repeatx, int repeaty, int base) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = rmf_perlin2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale, repeatx, repeaty, base);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_perlin3_fill(int x, int y, int z, int repeatx, int repeaty, int repeatz, int base) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale,((float)(k+1)/fz)*znoise_scale, repeatx, repeaty, repeatz, base);
                noisemap[i + x*j + x*y*k] = h;
                printf("%0.2f\n",h);
            }
        }
    }
}

void rmf_simplex2_fill(int x, int y) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = rmf_simplex2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_simplex3_fill(int x, int y, int z) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = rmf_simplex3(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale,((float)(k+1)/fz)*znoise_scale);
                noisemap[i + x*j + x*y*k] = h;
            }
        }
    }
}
