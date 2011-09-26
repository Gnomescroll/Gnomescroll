/*
 * Ridged multifractal noise
 * -same as perlin or simplex noise except one thing
 * -each octave of noise is calculated as 1.0 - abs(noise(x,...))
 */
 
#include "perlin.h"
#include "simplex.h"


float rmf_perlin1(float x, int octaves, float persistence, float frequency, float amplitude, int repeat, int base)
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
    //float x, y;
    //int octaves = 1;
    //float persistence = 0.5f;
    //float freq = 1.0f;
    //float amp = 1.0f;

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
    //float x, y, z;
    //int octaves = 1;
    //float persistence = 0.5f;
    //float freq = 1.0f;
    //float amp = 1.0f;

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
