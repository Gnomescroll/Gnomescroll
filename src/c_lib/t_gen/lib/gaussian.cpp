#include "gaussian.hpp"

#include <t_gen/twister.hpp>
#include <t_gen/lib/voronoi2.hpp>

namespace t_gen
{

float noise_map_2d_float[512*512  + 1];
unsigned char noise_map_2d_char[512*512  + 1];

//xdim/ydim might be odd
void init_gaussian_kernel(float* kernel, int xdim)
{
    const double k = 1.0; //normalization constant
    const double sigma = 1.0; //standard deviation

    for (int i=0; i<xdim; i++) kernel[i] = 0.0;

    const float m = (xdim-1)/2;

    double tmp[xdim];

    double _i = 0.0;
    for (int i=0; i < xdim; i++)
    {
        float a = (m-_i)*(m-_i);
        tmp[i] = exp(-1.0*a/sigma);
        _i += 1.0;
    }

    //for (int i=0; i < xdim; ++i) kernel[i] = 1.0;

    double sum = 0.0;
    for (int i=0; i < xdim; i++)
    {
        sum += tmp[i];
    }

    //double average = sum / ((float) xdim);

    for (int i=0; i < xdim; i++)
    {
        kernel[i] = float(k*tmp[i] / sum);
    }

    sum = 0.0;
    for (int i=0; i < xdim; ++i)
    {
        sum += kernel[i];
    }
    printf("kernel: sum= %f average= %f \n", sum, (sum /((float) xdim)));

}

void convolve(float* in, float* out, int xdim, int ydim)
{
    const int kdim = 5;
    const int kcen = (kdim-1) / 2;

    float kernel[kdim];

    init_gaussian_kernel((float*) kernel, kdim);

    int ii, jj;

    for (int i=0; i < xdim; i++)
    for (int j=0; j < ydim; j++)
    {
        float sum = 0.0;
        for (int n=0; n < kdim; n++)
        {
            ii = i + (n - kcen);
            if (ii >= 0 && ii < xdim) // ignore input samples which are out of bound
            {
                sum += in[xdim*j+ii] * kernel[n];
            }
        }
        out[xdim*j+i] = sum;
    }

    for (int i=0; i < xdim; i++)
    for (int j=0; j < ydim; j++)
    {
        float sum = 0.0;
        for (int n=0; n < kdim; n++)
        {
            jj = j + (n - kcen);
            if (jj >= 0 && jj < ydim) // ignore input samples which are out of bound
            {
                sum += out[xdim*jj+i] * kernel[n];
            }
        }
        out[xdim*j+i] = sum;
    }

// && jj >= 0 && jj < ydim


}



void test()
{
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);

    int xres = 512;
    int yres = 512;

    float* in = new float[xres*yres];
    float* out = new float[xres*yres];

    for (int i=0; i < xres*yres; i++) in[i] = 0.0;
    for (int i=0; i < xres*yres; i++) out[i] = 0.0;

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        //in[j*xres+i] = randf(); //genrand_real2();
        in[j*xres+i] = genrand_real2();

        out[j*xres+i] = 0.0;
        if ((j+i) % 2 == 0) out[j*xres+i] = 1.0;
    }

    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    save_png("001", out, xres, yres);

    delete[] in;
    delete[] out;

}

void gen_map()
{

    //genrand_real2()
    //genrand_int32());
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);

    int xres = 512;
    int yres = 512;

    float* in = new float[xres*yres];
    float* out = new float[xres*yres];

    for (int i=0; i < xres*yres; i++) in[i] = 0.0;
    for (int i=0; i < xres*yres; i++) out[i] = 0.0;

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        //in[j*xres+i] = randf(); //genrand_real2();
        in[j*xres+i] = genrand_real2();

        //out[j*xres+i] = 0.0;
        //if ((j+i) % 2 == 0) out[j*xres+i] = 1.0;

        out[j*xres+i] = genrand_real2();

    }

    convolve(in,out, xres,yres);
/*
    convolve(in, out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);
*/
    save_png("5001", out, xres, yres);


    double num = 0;
    double sum = 0;
    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        sum += out[yres*j+i];
        num += 1.0;
    }

    printf("noise map average= %f \n", (float) sum/num);



    //float scale = 0.7;
    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        noise_map_2d_float[512*j+i] = 0.5 + (1.0 - 0.5) *out[512*j + i];
    }

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {

        float _v = noise_map_2d_float[j*xres+i];

        if (_v < 0.0) _v = 0.0;
        if (_v > 1.0) _v = 1.0;

        unsigned char v = ((int) 255.0*_v);

        noise_map_2d_char[512*j+i] = v;
    }

    delete[] in;
    delete[] out;

}

}
