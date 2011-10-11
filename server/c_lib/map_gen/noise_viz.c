#include "ridged_mf.h"

void rmf_dist(float* arr, int x, int y, int z) {
    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    float fz = (float)z + 2.0f;
    
    int i,j,k;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                arr[i] = rmf_perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x, y, z, 0);
            }
        }
    }
}
