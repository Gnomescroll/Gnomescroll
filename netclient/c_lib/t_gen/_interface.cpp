#include "_interface.hpp"

#include <t_gen/twister.hpp>

#include <SDL/texture_loader.hpp>

namespace t_gen
{

void init_gaussian_kernel(float* kernel, int xdim, int ydim)
{
	kernel = new float[xdim*ydim];

	for(int i=0; i<xdim*ydim; i++) kernel[i] = 0;

	//populate
}

void convolve(int* in, int* out, int kCols, int kRows)
{
	const int kCols = 16;
	const int kRows = 16;

	float* kernel;

	init_gaussian_kernel(kernel, kCols, kRows);

	int kCenterX = kCols / 2;
	int kCenterY = kRows / 2;

	for(i=0; i < rows; ++i)              // rows
	{
	    for(j=0; j < cols; ++j)          // columns
	    {
	        sum = 0;                     // init to 0 before sum

	        for(m=0; m < kRows; ++m)     // kernel rows
	        {
	            mm = kRows - 1 - m;      // row index of flipped kernel

	            for(n=0; n < kCols; ++n) // kernel columns
	            {
	                nn = kCols - 1 - n;  // column index of flipped kernel

	                // index of input signal, used for checking boundary
	                ii = i + (m - kCenterY);
	                jj = j + (n - kCenterX);

	                // ignore input samples which are out of bound
	                if( ii >= 0 && ii < rows && jj >= 0 && jj < cols )
	                out[i][j] += in[ii][jj] * kernel[mm][nn];
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

    //SDL_Surface* surface = create_surface_from_nothing(256,256);

    char FileName[128];

    sprintf(FileName,"./screenshot/%d.png",  (int) 500 );

    //printf("Screenshot: %s \n", (char*) );

    //SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, xres, yres,
    //                                               32, 0x0000ff, 0x00ff00, 0xff0000, 0x000000);

    char* PBUFFER = (char*) malloc(4*xres*yres);


    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++) {
    	int index = 4*(j*xres + i);

    	int v = genrand_int32() % 256;
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