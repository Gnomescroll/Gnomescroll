#include "_interface.hpp"



#if DC_CLIENT
#include <SDL/texture_loader.hpp>
#endif

#include <t_gen/noise_map2.hpp>

typedef unsigned char Uint8;

namespace t_gen
{

void populate_2d_noise_array(float* _2d_noise_array, unsigned long seed, float persistance, int octaves)
{
    //class PerlinOctave2D p2d(octaves);
    init_genrand(seed);
    //p2d.setup_octaves();
    //p2d.set_param(persistance, seed);

    class PerlinOctave2D p2d(octaves);
    //p2d.set_param(0.50, 1337);

    float sum = 0.0f;
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)  
    {
        float _x = i*(1.0f/512.0f); // + (0.5/512.0);
        float _y = j*(1.0f/512.0f); // + (0.5/512.0);

        _2d_noise_array[512*j + i] = p2d.sample(_x,_y,persistance);
        //printf("sample: %f \n", p2d.sample(_x,_y,persistance)) ;
        //printf("x,y= %f %f noise= %f \n", _x,_y, p2d.sample(_x,_y,persistance) );
        sum += p2d.sample(_x,_y, persistance);
    }

    printf("populate_2d_noise_array: average= %f \n", sum/ (512.0f*512.0f));
}

void save_png(const char* filename, float* in, int xres, int yres)
{
#if DC_CLIENT
    char FileName[128];
    sprintf(FileName,"./screenshot/%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for(int i=0; i < 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++)
    {
        int index = 4*(j*xres + i);

        float _v = in[j*xres+i];

        if( _v < 0.0) _v = 0.0f;
        if( _v > 1.0) _v = 1.0f;
        unsigned char v = ((int) 255.0f*_v );
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
            printf("save_screenshot: not enough memory for surface inversion \n");
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
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
#endif
}

__attribute((always_inline, optimize("-O3")))
void save_perlin(const char* filename, float* in, int xres, int yres)
{
//#if DC_CLIENT
    char FileName[128];
    sprintf(FileName,"./screenshot/%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for(int i=0; i< 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++)
    {
        int index = 4*(j*xres + i);

        float _v = in[j*xres+i];

        if( _v < 0.0f)
        {
            _v *= -1;
            if( _v > 1.0f) _v = 1.0f;
            unsigned char v = ((int) 255.0f*_v );
            unsigned char v2 = gamma_correction[v];

            PBUFFER[index+0] = 0;
            PBUFFER[index+1] = v2;
            PBUFFER[index+2] = 0;
            PBUFFER[index+3] = 255;
        }
        else
        {
            if( _v > 1.0f) _v = 1.0f;
            unsigned char v = ((int) 255.0f*_v );
            unsigned char v2 = gamma_correction[v];

            PBUFFER[index+0] = 0;
            PBUFFER[index+1] = 0;
            PBUFFER[index+2] = v2;
            PBUFFER[index+3] = 255;
        }
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
            printf("save_screenshot: not enough memory for surface inversion \n");
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
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
//#endif
}

}
