/*
 * Ridged multifractal noise
 * -same as perlin or simplex noise except one thing
 * -each octave of noise is calculated as 1.0 - abs(noise(x,...))
 */
 
#include "perlin.h"
#include "simplex.h"


float rmf_perlin1(float x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
{
    if (octaves == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise1(x, repeat, base));
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += 1.0f - fabs(pnoise1(x * frequency, (const int)(repeat * frequency), base) * amplitude);
            max += amplitude;
            frequency *= 2.0f;  // is this supposed to be constant?
            amplitude *= persistence;
        }
        return (total / max);
    } 
    return 0.0f;
}

float rmf_perlin2(float x, float y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base)
{
    if (octaves == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise2(x,y, repeatx, repeaty, base));
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += 1.0f - fabs(pnoise2(x * frequency, y * frequency, repeatx * frequency, repeaty * frequency, base) * amplitude);
            max += amplitude;
            frequency *= 2.0f;  // constant?
            amplitude *= persistence;
        }
        return (total / max);
    }
    return 0.0f;
}

float rmf_perlin3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base)
{
    if (octaves == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(pnoise3(x, y, z, repeatx, repeaty, repeatz, base));
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += 1.0f - fabs(pnoise3(x * frequency, y * frequency, z * frequency, 
                (const int)(repeatx*frequency), (const int)(repeaty*frequency), (const int)(repeatz*frequency), base) * amplitude);
            max += amplitude;
            frequency *= 2.0f;  // constant?
            amplitude *= persistence;
        }
        return (total / max);
    }
    return 0.0f;
}


float rmf_simplex2(float x, float y, int octaves, float persistence, float frequency, float amplitude)
{
    if (octaves == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(snoise2(x,y));
    } else if (octaves > 1) {
        int i;
        float max = 0.0f;
        float total = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += 1.0f - fabs(snoise2(x * frequency, y * frequency) * amplitude);
            max += amplitude;
            frequency *= 2.0f;  // constant?
            amplitude *= persistence;
        }
        return (total/max); // why max
    }
    return 0.0f;
}

float rmf_simplex3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude)
{
    if (octaves == 1) {
        // Single octave, return simple noise
        return 1.0f - fabs(snoise3(x,y,z));
    } else if (octaves > 1) {
        int i;
        float total = 0.0f;
        float max = 0.0f;

        for (i = 0; i < octaves; i++) {
            total += 1.0f - fabs(snoise3(x * frequency, y * frequency, z * frequency) * amplitude);
            max += amplitude;
            frequency *= 2.0f;
            amplitude *= persistence;
        }
        return (total/max);
    }
    return 0.0f;
}

/* fill methods */
void rmf_perlin1_fill(int x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base) {
    float fx = (float)x + 2.0f;
    int i;
    float h;
    for (i=0; i<x; i++) {
        h = rmf_perlin1((i+1)/fx, octaves, persistence, frequency, amplitude, repeat, base);
        noisemap[i] = h;
    }
}

void rmf_perlin2_fill(int x, int y, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int base) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = rmf_perlin2((i+1)/fx,(j+1)/fy, octaves, persistence, frequency, amplitude, repeatx, repeaty, base);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_perlin3_fill(int x, int y, int z, int octaves, float persistence, float frequency, float amplitude, int repeatx, int repeaty, int repeatz, int base) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = rmf_perlin3((i+1)/fz,(j+1)/fz,(k+1)/fz, octaves, persistence, frequency, amplitude, repeatx, repeaty, repeatz, base);
                noisemap[i + x*j + x*y*k] = h;
            }
        }
    }
}

void rmf_simplex2_fill(int x, int y, int octaves, float persistence, float frequency, float amplitude) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;
    int i,j;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            h = simplex2((i+1)/fx,(j+1)/fy, octaves, persistence, frequency, amplitude);
            noisemap[i + x*j] = h;
        }
    }
}

void rmf_simplex3_fill(int x, int y, int z, int octaves, float persistence, float frequency, float amplitude) {
    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;
    int i,j,k;
    float h;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                h = simplex3((i+1)/fx,(j+1)/fy,(k+1)/fz, octaves, persistence, frequency, amplitude);
                noisemap[i + x*j + x*y*k] = h;
            }
        }
    }
}
