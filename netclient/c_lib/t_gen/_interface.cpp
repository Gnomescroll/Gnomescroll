#include "_interface.hpp"

#include <t_gen/twister.hpp>

#include <SDL/texture_loader.hpp>

namespace t_gen
{

//xdim/ydim might be odd
void init_gaussian_kernel(float* kernel, int dim)
{
	const double k = 1.0; //normalization constant

	for(int i=0; i<xdim; i++) kernel[i] = 0.0;

	const float m = ((float) xdim) / 2.0;

	double tmp[dim];

	double _i = 0.0;
	for(int i=0; i < xdim; i++)
	{

		float a = (m-_i)*(m-_i);
		tmp[i][j] = exp(-1*a+b);
		_i += 1.0;
	}

	//for(int i=0; i < xdim; ++i) kernel[i] = 1.0;


	double sum = 0.0;
	for(int i=0; i < xdim; i++)
	{
		sum += tmp[i][j];
	}

	double average = sum / ((float) xdim*ydim);

	for(int i=0; i < xdim; i++)
	{
		kernel[i] = (float) ( k*tmp[i] / average );
	}


	sum = 0.0;
	for(int i=0; i < xdim; ++i)
	{
		sum += kernel[xdim*j + i];
	}
	//printf("kernel: sum= %f average= %f \n", sum, (sum /((float) xdim*ydim) ));

}

void convolve(float* in, float* out, int xdim, int ydim)
{
	const int kdim = 5;
	const int kCenter = (kdim-1) / 2;

	float kernel[kdim];

	init_gaussian_kernel( (float*) kernel, kdim);

	int mm, nn;
	int ii, jj;

	for(int i=0; i < xdim; i++)
	for(int j=0; j < ydim; j++)
	{
    	out[xdim*j+i] = 0.0;
        for(int m=0; m < kRows; m++)
		for(int n=0; n < kCols; n++)
            {
                ii = i + (m - kCenter);


                // ignore input samples which are out of bound
                if( ii >= 0 && ii < xdim && jj >= 0 && jj < ydim )
                {
                	out[xdim*j+i] += in[xdim*jj+ii] * kernel[mm][nn];
                }

            }
        }
	}

}

void test()
{

    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);
/*
	int i;
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    init_by_array(init, length);
    printf("1000 outputs of genrand_int32()\n");
    for (i=0; i<1000; i++) {
      printf("%10lu ", genrand_int32());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of genrand_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", genrand_real2());
      if (i%5==4) printf("\n");
    }
*/
    int xres = 256;
    int yres = 256;

    float* in = new float[xres*yres];
    float* out = new float[xres*yres];

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) 
    {
    	in[j*xres+i] = genrand_real2();
    }

    convolve(in,out, xres,yres);
    convolve(out,in, xres,yres);
    convolve(in,out, xres,yres);

    char FileName[128];

    sprintf(FileName,"./screenshot/%d.png",  (int) 500 );

    char* PBUFFER = (char*) malloc(4*xres*yres);


    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) {
    	int index = 4*(j*xres + i);

    	//int v = genrand_int32() % 256;
    	int v = 255*out[j*xres+i];
    	PBUFFER[index+0] = v;
    	PBUFFER[index+1] = v;
    	PBUFFER[index+2] = v;
    	PBUFFER[index+3] = 255;
    }
    //char* PBUFFER = (char*) surface->pixels;

    //memcpy()
    //glReadPixels(0, 0, xres, yres, GL_RGBA, GL_UNSIGNED_BYTE, (void*) PBUFFER);

    for(int i=0; i < xres; i++) {
    for(int j=0; j < yres; j++) {

        //if( (PBUFFER)[4*(xres*j + i) + 3] == 0) ((unsigned int*)PBUFFER)[xres*j + i] = 0xffffffff;
        PBUFFER[4*(xres*j + i) + 3] = 255;

    }}

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
        //int w = xres;
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

    //void *tdefl_write_image_to_png_file_in_memory(
    //    const void *pImage, int w, int h, int num_chans, size_t *pLen_out);



    //printf("PNG SIZE= %i \n", png_size);

    FILE * pFile;
    pFile = fopen ( FileName , "wb" );
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);
}
	
}