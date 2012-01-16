#include "gradient.h"

float _pb_falloff_size_sqrt = 0.0f;
void conf_pb_falloff(int size) {
    _pb_falloff_size_sqrt = 0.5 * sqrt((float)size);
}
float parabolic_falloff(int size, int n) {
    float fsize = (float)size;
    float fn = (float)n;
    //float res = 0.5*(sqrt(fsize-fn)-0.5*sqrt(fsize));
    float res = 0.5*(sqrt(fsize-fn)- _pb_falloff_size_sqrt);
    // y = 0.5 * (sqrt(size - n) - 0.5 * sqrt(size));
    // y=0, y=3s/4
    // x=0, sqrt(s)/4
    return res;
}

void gradient1d_falloff(float fgrad[], int size) {
    int k;
    conf_pb_falloff(size);
    for (k=0; k<size; k++) {
        fgrad[k] = parabolic_falloff(size, k);
    }
}

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

void apply_grad3_falloff(int x, int y, int z, int x_dir, int y_dir, int z_dir) {    // *_dir are boolean and indicate if gradient should be applied in that direction

    if (x > xmax || y > ymax || z > zmax || x < 0 || y < 0 || z < 0) {
        printf("WARNING: applying gradient beyond map size. Abort gradient.\n");
        return;
    }

    if (x_dir == 0 && y_dir == 0 && z_dir == 0) {
        printf("apply_grad3_falloff: no dimensions specified to apply gradient in. no effect.\n");
        return;
    }

    // generate gradient
    float* fgrad;
    int i,j,k;
    int index;

    if (x_dir) {
        fgrad = (float*) malloc(x*sizeof(float));
        gradient1d_falloff(fgrad, x);
        for (i=0; i<x; i++) {
            for (j=0; j<y; j++) {
                for (k=0; k<z; k++) {
                    index = i + x*j + x*y*k;
                    if (fgrad[i] < 0.0f) {
                        noisemap[index] += fgrad[i];    // apply gradient to map
                    }
                }
            }
        }
        free(fgrad);
    }
    if (y_dir) {
        fgrad = (float*) malloc(y*sizeof(float));
        gradient1d_falloff(fgrad, y);
        for (i=0; i<x; i++) {
            for (j=0; j<y; j++) {
                for (k=0; k<z; k++) {
                    index = i + x*j + x*y*k;
                    if (fgrad[j] < 0.0f) {
                        noisemap[index] += fgrad[j];    // apply gradient to map
                    }
                }
            }
        }
        free(fgrad);
    }
    if (z_dir) {
        fgrad = (float*) malloc(z*sizeof(float));
        gradient1d_falloff(fgrad, z);
        for (i=0; i<x; i++) {
            for (j=0; j<y; j++) {
                for (k=0; k<z; k++) {
                    index = i + x*j + x*y*k;
                    if (fgrad[k] < 0.0f) {
                        noisemap[index] += fgrad[k];    // apply gradient to map
                    }
                }
            }
        }
        free(fgrad);
    }

}
