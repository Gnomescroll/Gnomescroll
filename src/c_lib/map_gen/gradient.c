#include "gradient.h"

static float _pb_falloff_size_sqrt = 0.0f;
void conf_pb_falloff(int size)
{
    _pb_falloff_size_sqrt = 0.5f * sqrtf((float)size);
}
float parabolic_falloff(int size, int n)
{
    float fsize = (float)size;
    float fn = (float)n;
    //float res = 0.5*(sqrt(fsize-fn)-0.5*sqrt(fsize));
    float res = 0.5f*(sqrtf(fsize-fn)- _pb_falloff_size_sqrt);
    // y = 0.5 * (sqrt(size - n) - 0.5 * sqrt(size));
    // y=0, y=3s/4
    // x=0, sqrt(s)/4
    return res;
}

void gradient1d_falloff(float fgrad[], int size)
{
    conf_pb_falloff(size);
    for (int k=0; k<size; k++)
        fgrad[k] = parabolic_falloff(size, k);
}

void gradient1d(float f[], int x_size, float x0, float x1)
{
    float samples[2] = {x0, x1};
    float fx_size = (float)x_size;

    for (int i=0; i<x_size; i++)
    {
        float px = i / fx_size;
        f[i] = linearInterpolate(samples, px);
    }
}

// only in x direction. more directions later if needed (this method is not used currently)
void apply_grad1(float* noisemap, int x, float x0, float x1)
{
    assert(x > 0 && x <= XMAX);
    
    float* fgrad = (float*) malloc(x*sizeof(float));

    gradient1d(fgrad, x, x0, x1);
    for (int i=0; i<x; i++) noisemap[i] += fgrad[i];
    free(fgrad);
}

void gradient2d(float f[], int x_size, int y_size, float x0, float x1, float y0, float y1)
{   // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis
    float samples[2][2] = { {(x0+y0)/2, (x0+y1)/2}, {(x1+y0)/2, (x1+y1)/2} };

    float fx_size = (float)x_size,
            fy_size = (float)y_size;
    
    for (int i=0; i<x_size; i++)
    {
        float px = i / fx_size;
        for (int j=0; j<y_size; j++)
        {
            float py = j / fy_size;
            f[i + x_size*j] = bilinearInterpolate(samples, px,py);
        }
    }
}

void apply_grad2(float* noisemap, int x, int y, float x0, float x1, float y0, float y1)
{
    assert(x > 0 && x <= XMAX && y > 0 && y <= YMAX);

    float* fgrad = (float*) malloc(x*y*sizeof(float));

    gradient2d(fgrad, x,y, x0, x1, y0, y1);

    // set map
    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    {
        int index = i + x*j;
        noisemap[index] += fgrad[index];    // apply gradient to map
    }
    free(fgrad);
}

inline void buildGradientSampleCube(float samples[2][2][2], float x0, float x1, float y0, float y1, float z0, float z1)
{
    samples[0][0][0] = (x0 + y0 + z0)/3; // 000
    samples[0][0][1] = (x0 + y0 + z1)/3; // 001
    samples[0][1][0] = (x0 + y1 + z0)/3; // 010
    samples[0][1][1] = (x0 + y1 + z1)/3; // 011
    samples[1][0][0] = (x1 + y0 + z0)/3; // 100
    samples[1][0][1] = (x1 + y0 + z1)/3; // 101
    samples[1][1][0] = (x1 + y1 + z0)/3; // 110
    samples[1][1][1] = (x1 + y1 + z1)/3; // 111
}

    
void gradient3d(float f[], int x_size, int y_size, int z_size, float x0, float x1, float y0, float y1, float z0, float z1)
{   // x,y,pos,neg should in [-1,1]. they indicate the gradient extremes along each axis
    float samples[2][2][2];
    buildGradientSampleCube(samples, x0, x1, y0, y1, z0, z1);

    float fx_size = (float)x_size,
            fy_size = (float)y_size,
            fz_size = (float)z_size;

    for (int i=0; i<x_size; i++)
    {
        float px = i / fx_size;
        for (int j=0; j<y_size; j++)
        {
            float py = j / fy_size;
            for (int k=0; k<z_size; k++)
            {
                float pz = k / fz_size;
                f[i + x_size*j + x_size*y_size*k] = trilinearInterpolate_condensed(samples, px,py,pz);
            }
        }
    }
}

void apply_grad3(float* noisemap, int x, int y, int z, float x0, float x1, float y0, float y1, float z0, float z1)
{
    assert(x > 0 && x <= XMAX && y > 0 && y <= YMAX && z > 0 && z <= ZMAX);

    // generate gradient
    float* fgrad = (float*) malloc(x*y*z*sizeof(float));
    
    gradient3d(fgrad, x,y,z, x0, x1, y0, y1, z0, z1);

    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
    for (int k=0; k<z; k++)
    {
        int index = i + x*j + x*y*k;
        noisemap[index] += fgrad[index];    // apply gradient to map
    }
    free(fgrad);
}

void apply_grad3_falloff(float* noisemap, int x, int y, int z, int x_dir, int y_dir, int z_dir)
{   // *_dir are boolean and indicate if gradient should be applied in that direction
    assert(x > 0 && x <= XMAX && y > 0 && y <= YMAX && z > 0 && z <= ZMAX);
    assert(x_dir != 0 || y_dir != 0 || z_dir != 0);

    // generate gradient
    if (x_dir)
    {
        float* fgrad = (float*) malloc(x*sizeof(float));
        gradient1d_falloff(fgrad, x);
        for (int i=0; i<x; i++)
        for (int j=0; j<y; j++)
        for (int k=0; k<z; k++)
        {
            int index = i + x*j + x*y*k;
            if (fgrad[i] < 0.0f) noisemap[index] += fgrad[i];    // apply gradient to map
        }
        free(fgrad);
    }
    if (y_dir)
    {
        float* fgrad = (float*) malloc(y*sizeof(float));
        gradient1d_falloff(fgrad, y);
        for (int i=0; i<x; i++)
        for (int j=0; j<y; j++)
        for (int k=0; k<z; k++)
        {
            int index = i + x*j + x*y*k;
            if (fgrad[j] < 0.0f) noisemap[index] += fgrad[j];    // apply gradient to map
        }
        free(fgrad);
    }
    if (z_dir)
    {
        float* fgrad = (float*) malloc(z*sizeof(float));
        gradient1d_falloff(fgrad, z);
        for (int i=0; i<x; i++)
        for (int j=0; j<y; j++)
        for (int k=0; k<z; k++)
        {
            int index = i + x*j + x*y*k;
            if (fgrad[k] < 0.0f) noisemap[index] += fgrad[k];    // apply gradient to map
        }
        free(fgrad);
    }

}
