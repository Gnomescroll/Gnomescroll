#include "perturb.h"

// THESE ARE ALL TEST FUNCTIONS


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
            nx = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x,y,0);
            seed_noise(seed2);
            ny = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x,y,0);
            
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
    free(transactions);

}

void perturb_perlin3a(int x, int y, int z, float turbulence) {
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
                    nx = perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x,y,z, 0);
                    seed_noise(seed2);
                    ny = perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x,y,z, 0);
                    seed_noise(seed3);
                    nz = perlin3(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, ((float)(k+1)/fz)*znoise_scale, x,y,z, 0);
                
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
    free(transactions);

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
                    nx = perlin1(((float)(i+1)/fx)*xnoise_scale, x, 0);
                    seed_noise(seed2);
                    ny = perlin1(((float)(j+1)/fy)*ynoise_scale, y, 0);
                    seed_noise(seed3);
                    nz = perlin1(((float)(k+1)/fz)*znoise_scale, z, 0);
                
                    //printf("Block found at %d,%d,%d\n", i,j,k);
                    _set(i,j,k,0);  // clear

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
        if (!isSolid(_get(a,b,c))) {
            _set(a,b,c, 3);
        }
    }

    free(transactions);
}


void perturb_heightmap(int x, int y, float turbulence, int tile, int clamp) {
    printf("Perturbing heightmap\n");

    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;
    //float fz = (float)z + 2.0f;

    int i,j,k;
    float nz;
    int z;
    int n;
    int b;
    int offset;
    
    next_seed();

    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            z = _get_highest_solid_block(i,j);
            b = _get(i,j,z);
            if (! (b==4 || b==5)) continue;
            nz = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x, y, 0);

            // perturb
            k = (int)((float)z + nz*turbulence);
            offset = k-z;
            if (offset > clamp) k = z+clamp;
            if (k < 0 || k >= zmax) continue;
            for (n=z; n <= k; n++) {
                if (n==z) _set(i,j,z,5);
                else _set(i,j,n, tile);
            }
        }
    }
}


void perturb_2d_noisemapa(int x, int y, float turbulence, int blend_mode) {

    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;

    float nx,ny;
    int i,j;
    int ii,jj;

    int seed1 = next_seed();
    int seed2 = next_seed();

    int arr_size = x*y;
    int index1, index2;

    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            seed_noise(seed1);
            nx = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x, y, 0);
            seed_noise(seed2);
            ny = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x, y, 0);

            ii = (int)((float)(i) + nx*turbulence);
            jj = (int)((float)(j) + ny*turbulence);

            index1 = i + x*j;
            index2 = ii + x*jj;

            index2 %= arr_size;
            if (index2 < 0 || index2 >= arr_size) {
                printf("INDEX2 out of range; %d %d\n", index2, arr_size);
                continue;
            }

            switch (blend_mode) {
                case NO_BLEND:
                    noisemap[index2] = noisemap[index1];
                    break;
                case ADD_BLEND:
                    noisemap[index2] += noisemap[index1];
                    break;
                case SUB_BLEND:
                    noisemap[index2] -= noisemap[index1];
                    break;
                case MULT_BLEND:
                    noisemap[index2] *= noisemap[index1];
                    break;
                case DIV_BLEND:
                    noisemap[index2] /= noisemap[index1];
                    break;
            }
        }
    }
}

void perturb_2d_noisemap(int x, int y, float turbulence, int blend_mode) {

    float fx = (float)x + 2.0f;
    float fy = (float)y + 2.0f;

    float nx,ny;
    int i,j;
    int ii,jj;

    int seed1 = next_seed();
    int seed2 = next_seed();

    float *_noise = (float*) malloc(sizeof(float)*x*y);

    int index1,index2;
    int arr_size = x*y;

    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            seed_noise(seed1);
            nx = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x, y, 0);
            seed_noise(seed2);
            ny = perlin2(((float)(i+1)/fx)*xnoise_scale, ((float)(j+1)/fy)*ynoise_scale, x, y, 0);

            ii = (int)((float)(i) + nx*turbulence);
            jj = (int)((float)(j) + ny*turbulence);

            index1 = i + x*j;
            index2 = ii + x*jj;
            if (index2 < 0 || index2 >= arr_size) continue;
            _noise[index2] = noisemap[index1];
        }
    }

    // copy back to noisemap
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            switch (blend_mode) {
                case NO_BLEND:
                    noisemap[i + x*j] = _noise[i + x*j];
                    break;
                case ADD_BLEND:
                    noisemap[i + x*j] += _noise[i + x*j];
                    break;
                case SUB_BLEND:
                    noisemap[i + x*j] -= _noise[i + x*j];
                    break;
                case MULT_BLEND:
                    noisemap[i + x*j] *= _noise[i + x*j];
                    break;
                case DIV_BLEND:
                    noisemap[i + x*j] /= _noise[i + x*j];
                    break;
            }
        }
    }    
    free(_noise);
}
