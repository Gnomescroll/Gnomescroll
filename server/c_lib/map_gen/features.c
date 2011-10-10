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
                    d = perlin2((i+1)/fx,(j+1)/fy, x, y, base);
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

void _caves(int x, int y, int z, int base) {

    float fx = (float)x + 2.0f,
           fy = (float)y + 2.0f,
           fz = (float)z + 2.0f;

    seed_noise(571);
    
    int i,j,k;
    float n;
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                n = rmf_perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, base);
                if (n > 2.45) {
                    _set(i,j,k, 2);
                }
            }
        }
    }

    seed_noise(1001);
    
    for (i=0; i < x; i++) {
        for (j=0; j < y; j++) {
            for (k=0; k < z; k++) {
                n = rmf_perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, base);
                if (n > 2.45 && isSolid(_get(i,j,k))) {
                    _set(i,j,k, 3);
                }
            }
        }
    }
    
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
