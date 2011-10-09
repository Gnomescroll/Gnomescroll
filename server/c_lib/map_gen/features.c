#include "features.h"

// x,y are dimensions of map to apply to

void grass(int x, int y, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int base) {

    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f;

    int i,j,k,n;
    float d;
    int dd;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            k = _get_highest_solid_block(i,j);
            if (k >= 0) {
                _set(i,j,k, 4);
                if (k != 0) {
                    // dirt
                    d = perlin2((i+1)/fx,(j+1)/fy, octaves, persistence, amplitude, lacunarity, frequency, x, y, base);
                    dd = (int)(fabs(d) * 100);
                    dd %= (k < 3) ? k : 3;
                    dd += 1;
                    for (n=1; n <= dd; n++) {
                        _set(i,j,k-n, 5);
                    }
                }
            }
        }
    }
}
