#include "_interface.hpp"

#include <t_gen/twister.hpp>
//#include <t_map/t_map.hpp>

#if DC_CLIENT
#include <SDL/texture_loader.hpp>
#endif

namespace t_gen
{

//xdim/ydim might be odd
void init_gaussian_kernel(float* kernel, int xdim)
{
	const double k = 1.0; //normalization constant

	for(int i=0; i<xdim; i++) kernel[i] = 0.0;

	const float m = (xdim-1)/2;

	double tmp[xdim];

	double _i = 0.0;
	for(int i=0; i < xdim; i++)
	{
		float a = (m-_i)*(m-_i);
		tmp[i] = exp(-1.0*a);
		_i += 1.0;
	}

	//for(int i=0; i < xdim; ++i) kernel[i] = 1.0;

	double sum = 0.0;
	for(int i=0; i < xdim; i++)
	{
		sum += tmp[i];
	}

	//double average = sum / ((float) xdim);

	for(int i=0; i < xdim; i++)
	{
		kernel[i] = (float) ( k*tmp[i] / sum  );
	}

	sum = 0.0;
	for(int i=0; i < xdim; ++i)
	{
		sum += kernel[i];
	}
	printf("kernel: sum= %f average= %f \n", sum, (sum /((float) xdim) ));

}

void convolve(float* in, float* out, int xdim, int ydim)
{
	const int kdim = 5;
	const int kcen = (kdim-1) / 2;

	float kernel[kdim];

	init_gaussian_kernel( (float*) kernel, kdim);

	int ii, jj;

	for(int i=0; i < xdim; i++)
	for(int j=0; j < ydim; j++)
	{
    	float sum = 0.0;
		for(int n=0; n < kdim; n++)
        {
            ii = i + (n - kcen);
            if( ii >= 0 && ii < xdim) // ignore input samples which are out of bound
            {
            	sum += in[xdim*j+ii] * kernel[n];
            }
        }
        out[xdim*j+i] = sum;
	}

	for(int i=0; i < xdim; i++)
	for(int j=0; j < ydim; j++)
	{
    	float sum = 0.0;
		for(int n=0; n < kdim; n++)
        {
            jj = j + (n - kcen);
            if( jj >= 0 && jj < ydim) // ignore input samples which are out of bound
            {
            	sum += out[xdim*jj+i] * kernel[n];
            }
        }
        out[xdim*j+i] = sum;
	}

// && jj >= 0 && jj < ydim 


}


void save_png(const char* filename, float*in, int xres, int yres)
{
#if DC_CLIENT
    char FileName[128];
    sprintf(FileName,"./screenshot/%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for(int i=0; i< 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = pow(intensity, 1.0/2.2)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) {
    	int index = 4*(j*xres + i);

    	float _v = in[j*xres+i];

    	if( _v < 0.0) _v = 0.0;
    	if( _v > 1.0) _v = 1.0;
    	unsigned char v = ((int) 255.0*_v );
    	unsigned char v2 = gamma_correction[v];

    	PBUFFER[index+0] = v2;
    	PBUFFER[index+1] = v2;
    	PBUFFER[index+2] = v2;
    	PBUFFER[index+3] = 255;
    }

    for(int i=0; i < xres; i++)
    for(int j=0; j < yres; j++)
    {
        PBUFFER[4*(xres*j + i) + 3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if(NULL == temp_row)
        {
            SDL_SetError("save_screenshot: not enough memory for surface inversion");
        }
        int pitch = xres * 4;
        int h = yres;

        height_div_2 = (int) (yres * .5);
        for(index = 0; index < height_div_2; index++)    
        {
            memcpy( (Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row); 
    }

    size_t png_size;
    char* PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
        (const char*) PBUFFER, xres, yres, 4, &png_size);

    FILE * pFile;
    pFile = fopen ( FileName , "wb" );
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);
#endif
}


void test()
{
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);

    int xres = 512;
    int yres = 512;

    float* in = new float[xres*yres];
    float* out = new float[xres*yres];

    for(int i=0; i < xres*yres; i++) in[i] = 0.0;
    for(int i=0; i < xres*yres; i++) out[i] = 0.0;

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) 
    {
    	//in[j*xres+i] = randf(); //genrand_real2();
    	in[j*xres+i] = genrand_real2();

		out[j*xres+i] = 0.0;
    	if((j+i) % 2 == 0) out[j*xres+i] = 1.0;
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

}

void gen_map()
{
	//genrand_real2()
	//genrand_int32());
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);

    int xres = 256;
    int yres = 256;

    float* in = new float[xres*yres];
    float* out = new float[xres*yres];

    for(int i=0; i < xres*yres; i++) in[i] = 0.0;
    for(int i=0; i < xres*yres; i++) out[i] = 0.0;

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) 
    {
    	//in[j*xres+i] = randf(); //genrand_real2();
    	in[j*xres+i] = genrand_real2();

		out[j*xres+i] = 0.0;
    	if((j+i) % 2 == 0) out[j*xres+i] = 1.0;
    }

   	convolve(in,out, xres,yres);
/*
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


#if DC_SERVER

	int max_height_diff = 64;
	int baseline = 64;

	const int value = 1;
    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) 
    {
		int z = baseline + out[xres*j+i]*max_height_diff;
		t_map::set(i,j,z, value);
	}

#endif

}

}