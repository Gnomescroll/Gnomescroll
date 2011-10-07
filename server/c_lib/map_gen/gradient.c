#include "gradient.h"

void gradient1d(float f[], int x_size, float x0, float x1) {

    float samples[2] = {x0, x1};

    int i;
    float px;
    float fx_size = (float)x_size;

    for (i=0; i<x_size; i++) {
        px = i / fx_size;
        f[i] = linearInterpolate(samples, px);
    }

}

// only in x direction. more directions later if needed (this method is not used currently)
void apply_grad1(int x, float x0, float x1) {
    
    if (x > xmax || x < 0) {
        printf("WARNING: applying gradient beyond map size. Abort gradient.\n");
        return;
    }
    
    float * fgrad;
    fgrad = (float*) malloc(x*sizeof(float));

    gradient1d(fgrad, x, x0, x1);

    int i;
    for (i=0; i<x; i++) {
        noisemap[i] += fgrad[i];
    }

    free(fgrad);
}

void gradient2d(float f[], int x_size, int y_size, float x0, float x1, float y0, float y1) { // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis

    float samples[2][2] = { {(x0+y0)/2, (x0+y1)/2}, {(x1+y0)/2, (x1+y1)/2} };

    int i,j;
    float px,py;
    float fx_size = (float)x_size,
            fy_size = (float)y_size;
    
    for (i=0; i<x_size; i++) {
        px = i / fx_size;
        for (j=0; j<y_size; j++) {
            py = j / fy_size;
            f[i + x_size*j] = bilinearInterpolate(samples, px,py);
        }
    }
}

void apply_grad2(int x, int y, float x0, float x1, float y0, float y1) {

    if (x > xmax || y > ymax || x < 0 || y < 0) {
        printf("WARNING: applying gradient beyond map size. Abort gradient.\n");
        return;
    }

    float* fgrad;
    fgrad = (float*) malloc(x*y*sizeof(float));

    gradient2d(fgrad, x,y, x0, x1, y0, y1);

    // set map
    int i,j;
    int index;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            index = i + x*j;
            noisemap[index] += fgrad[index];    // apply gradient to map
        }
    }
    free(fgrad);
}

inline void buildGradientSampleCube(float samples[2][2][2], float x0, float x1, float y0, float y1, float z0, float z1) {
    samples[0][0][0] = (x0 + y0 + z0)/3; // 000
    samples[0][0][1] = (x0 + y0 + z1)/3; // 001
    samples[0][1][0] = (x0 + y1 + z0)/3; // 010
    samples[0][1][1] = (x0 + y1 + z1)/3; // 011
    samples[1][0][0] = (x1 + y0 + z0)/3; // 100
    samples[1][0][1] = (x1 + y0 + z1)/3; // 101
    samples[1][1][0] = (x1 + y1 + z0)/3; // 110
    samples[1][1][1] = (x1 + y1 + z1)/3; // 111
}

    
void gradient3d(float f[], int x_size, int y_size, int z_size, float x0, float x1, float y0, float y1, float z0, float z1) { // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis

    float samples[2][2][2];
    buildGradientSampleCube(samples, x0, x1, y0, y1, z0, z1);

    int i,j,k;
    float px,py,pz;
    float fx_size = (float)x_size,
            fy_size = (float)y_size,
            fz_size = (float)z_size;

    for (i=0; i<x_size; i++) {
        px = i / fx_size;
        for (j=0; j<y_size; j++) {
            py = j / fy_size;
            for (k=0; k<z_size; k++) {
                pz = k / fz_size;
                f[i + x_size*j + x_size*y_size*k] = trilinearInterpolate_condensed(samples, px,py,pz);
            }
        }
    }
}

void apply_grad3(int x, int y, int z, float x0, float x1, float y0, float y1, float z0, float z1) {

    if (x > xmax || y > ymax || z > zmax || x < 0 || y < 0 || z < 0) {
        printf("WARNING: applying gradient beyond map size. Abort gradient.\n");
        return;
    }

    // generate gradient
    float* fgrad;
    fgrad = (float*) malloc(x*y*z*sizeof(float));
    
    gradient3d(fgrad, x,y,z, x0, x1, y0, y1, z0, z1);

    int i,j,k;
    int index;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            for (k=0; k<z; k++) {
                index = i + x*j + x*y*k;
                noisemap[index] += fgrad[index];    // apply gradient to map
            }
        }
    }

    free(fgrad);
}
