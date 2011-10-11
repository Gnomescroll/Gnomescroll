#include "perturb.h"

void perturb_perlin2(int x, int y, int z, float turbulence) {
    printf("Perturbing with perlin2\n");

    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    //float fz = (float)z + 2.0f;

    int i,j,k;
    int a,b,c;
    float nx,ny;//,nz;
    int bval;

    int* transactions = (int*)malloc(sizeof(int)*x*y*z);
    int t_ctr = 0;

    int seed1 = next_seed();
    int seed2 = next_seed();

    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            seed_noise(seed1);
            nx = perlin2((i+1)/fx, (j+1)/fy, x,y,0);
            seed_noise(seed2);
            ny = perlin2((i+1)/fx, (j+1)/fy, x,y,0);
            
            for (k=0; k<z; k++) {
                bval = _get(i,j,k);
                if (isSolid(bval)) {
                    //printf("Block found at %d,%d,%d\n", i,j,k);
                    //_set(i,j,k,0);  // clear

                    // perturb
                    a = (int)((float)i + nx*turbulence);
                    b = (int)((float)j + ny*turbulence);
                    //c = (int)((float)k + nz*turbulence);
                    transactions[t_ctr++] = a;
                    transactions[t_ctr++] = b;
                    transactions[t_ctr++] = k;
                    //printf("Block moved to %d,%d,%d\n", a,b,k);
                    //_set(a,b,k, bval);  // move
                }
            }
        }
    }

    // apply transactions
    for (i=0; i<t_ctr; i++) {
        a = transactions[i];
        b = transactions[++i];
        c = transactions[++i];
        if (a < 0 || b < 0 || c < 0 || a > xmax || b > ymax || c > zmax) {
            continue;
        }
        _set(a,b,c, 3);
    }

}

void perturb_perlin3(int x, int y, int z, float turbulence) {
    printf("Perturbing with perlin3\n");

    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    float fz = (float)z + 2.0f;

    int i,j,k;
    int a,b,c;
    float nx,ny,nz;
    int bval;

    int* transactions = (int*)malloc(sizeof(int)*x*y*z);
    int t_ctr = 0;

    int seed1 = next_seed();
    int seed2 = next_seed();
    int seed3 = next_seed();

    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                bval = _get(i,j,k);
                if (isSolid(bval)) {

                    seed_noise(seed1);
                    nx = perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x,y,z, 0);
                    seed_noise(seed2);
                    ny = perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x,y,z, 0);
                    seed_noise(seed3);
                    nz = perlin3((i+1)/fx, (j+1)/fy, (k+1)/fz, x,y,z, 0);
                
                    //printf("Block found at %d,%d,%d\n", i,j,k);
                    //_set(i,j,k,0);  // clear

                    // perturb
                    a = (int)((float)i + nx*turbulence);
                    b = (int)((float)j + ny*turbulence);
                    c = (int)((float)k + nz*turbulence);

                    transactions[t_ctr++] = a;
                    transactions[t_ctr++] = b;
                    transactions[t_ctr++] = c;
                    //printf("Block moved to %d,%d,%d\n", a,b,k);
                    //_set(a,b,k, bval);  // move
                }
            }
        }
    }

    // apply transactions
    for (i=0; i<t_ctr; i++) {
        a = transactions[i];
        b = transactions[++i];
        c = transactions[++i];
        if (a < 0 || b < 0 || c < 0 || a > xmax || b > ymax || c > zmax) {
            continue;
        }
        _set(a,b,c, 3);
    }

}
