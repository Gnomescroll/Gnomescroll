/*
 * Ridged multifractal noise
 * -same as perlin or simplex noise except one thing
 * -each octave of noise is calculated as 1.0 - abs(noise(x,...))
 */

#include "ridged_mf.h"


// Body of a rmf noise wrapper
#define RMFX_BODY(pnoise) do {\
    float amplitude = _amp, \
           frequency = _freq;   \
    int i;  \
    float max = 0.0f;   \
    float total = 0.0f; \
    for (i = 0; i < _oct; i++)  \
    {   \
        total += 1.0f - fabs(pnoise);    \
        max += amplitude;   \
        frequency *= _lac;  \
        amplitude *= _per;  \
    }   \
    return (total / max);   \
    }while(0)


// Perlin

float rmf_perlin1(float x, int repeat, int base)
{
    RMFX_BODY(
        pnoise1(x * frequency, (const int)(repeat * frequency), base) * amplitude
    );
}

float rmf_perlin2(float x, float y, int repeatx, int repeaty, int base)
{
    RMFX_BODY(
        pnoise2(x * frequency, y * frequency, repeatx * frequency, repeaty * frequency, base) * amplitude
    );
}

float rmf_perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base)
{
    RMFX_BODY(
        pnoise3(
            x * frequency, y * frequency, z * frequency,
            repeatx * frequency, repeaty * frequency, repeatz * frequency,
            base)
        * amplitude
    );
}

// Simplex

float rmf_simplex2(float x, float y)
{
    RMFX_BODY(
        snoise2(x * frequency, y * frequency) * amplitude
    );
}

float rmf_simplex3(float x, float y, float z)
{
    RMFX_BODY(
        snoise3(x * frequency, y * frequency, z * frequency) * amplitude
    );
}

// Fill methods

void rmf_perlin1_fill(int x, int repeat, int base)
{
    float fx = (float)x + 2.0f;
    for (int i=0; i<x; i++)
        noisemap[i] = rmf_perlin1(((float)(i+1)/fx)*xnoise_scale, repeat, base);
}

void rmf_perlin2_fill(int x, int y, int repeatx, int repeaty, int base)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
        noisemap[i + x*j] = rmf_perlin2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale, repeatx, repeaty, base);
}

void rmf_perlin3_fill(int x, int y, int z, int repeatx, int repeaty, int repeatz, int base)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    for (int k=0; k<z; k++)
        noisemap[i + x*j + x*y*k] = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale,((float)(k+1)/fz)*znoise_scale, repeatx, repeaty, repeatz, base);
}

void rmf_simplex2_fill(int x, int y)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
        noisemap[i + x*j] = rmf_simplex2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale);
}

void rmf_simplex3_fill(int x, int y, int z)
{
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    for (int k=0; k<z; k++)
        noisemap[i + x*j + x*y*k] = rmf_simplex3(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale,((float)(k+1)/fz)*znoise_scale);
}
