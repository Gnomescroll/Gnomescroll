#include "features.h"

// x,y are dimensions of map to apply to

void _grass(int x, int y, int base) {

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
                    d = perlin2(((float)(i+1)/fx)*xnoise_scale,((float)(j+1)/fy)*ynoise_scale, x, y, base);
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

void _caves(int x, int y, int z, float threshold, int base) {


    float cutoff;
    cutoff = percentile_cutoff_calculation(threshold, noisemap, x*y*z);

    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;

    seed_noise(572);

    int i,j,k;
    float n;
    float ttl=0.0f;
    int c=0;
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                n = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x, y, z, base);
                c += 1;
                ttl += n;
                if (n > cutoff) {
                    _set(i,j,k, 2);
                }
            }
        }
    }

    float avg = ttl / (float)c;
    printf("ttl: %0.2f, count: %d, avg: %0.2f\n", ttl, c, avg);

    seed_noise(1001);
    
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                n = rmf_perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x, y, z, base);
                if (n > cutoff && isSolid(_get(i,j,k))) {
                    _set(i,j,k, 3);
                }
            }
        }
    }

    // invert
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                if (_get(i,j,k) != 3) {
                    _set(i,j,k, 2);
                } else {
                    _set(i,j,k, 0);
                }
            }
        }
    }

}


void _ceiling(int x, int y, int z, int height, int tile) {
    int i,j,k;
    z -= 1;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=z; k>z-height; k--) {
                _set(i,j,k, tile);
            }
        }
    }
}

void _floor(int x, int y, int z_start, int height, int tile) {
    int i,j,k;
    for (k=z_start; k<z_start+height; k++) {
        for (i=0; i<x; i++) {
            for (j=0; j<y; j++) {
                _set(i,j,k, tile);
            }
        }
    }
}
