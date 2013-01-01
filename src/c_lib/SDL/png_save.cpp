#include "png_save.hpp"


static unsigned char _png_gamma_correction[256];

void _init_png__png_gamma_correction()
{

    for(int i=0; i < 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        _png_gamma_correction[i] = (unsigned char)((int) intensity);
    }
}

void save_png(const char* filename, float* in, int xres, int yres)
{
    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    _init_png__png_gamma_correction();

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++)
    {
        int index = j*xres + i;

        float _v = in[j*xres+i];

        if( _v < 0.0) _v = 0.0f;
        if( _v > 1.0) _v = 1.0f;
        unsigned char v = ((int) 255.0f*_v );
        unsigned char v2 = _png_gamma_correction[v];

        PBUFFER[4*index+0] = v2;
        PBUFFER[4*index+1] = v2;
        PBUFFER[4*index+2] = v2;
        PBUFFER[4*index+3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if(NULL == temp_row)
        {
            printf("Error: save_png, not enough memory for surface inversion \n");
            return;
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
    if(!pFile)
    {
        printf("Error: save_png, could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
}


void save_png_RGB(const char* filename, float* in, int xres, int yres)
{
    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    _init_png__png_gamma_correction();

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++)
    {
        int index = j*xres + i;
        for(int k=0; k<3;k++)
        {
            float _v = in[3*(j*xres+i)];

            if( _v < 0.0) _v = 0.0f;
            if( _v > 1.0) _v = 1.0f;
            unsigned char v = ((int) 255.0f*_v );
            unsigned char v2 = _png_gamma_correction[v];

            PBUFFER[4*index+k] = v2;
        }
        PBUFFER[index+3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if(NULL == temp_row)
        {
            printf("Error: save_png, not enough memory for surface inversion \n");
            return;
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
    if(!pFile)
    {
        printf("Error: save_png, could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
}


}