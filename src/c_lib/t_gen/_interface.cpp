#include "_interface.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/noise_map2.hpp>
#include <t_gen/explosives.hpp>

typedef unsigned char Uint8;

namespace t_gen
{

void init()
{
    init_explosives();
}

void teardown()
{

}

void populate_2d_noise_array(float* _2d_noise_array, float persistence, int octaves)
{
    init_genrand(rand());
    class PerlinOctave2D* p2d = new class PerlinOctave2D(octaves);

    float sum = 0.0f;
    for (int i=0; i<XMAX; i++)
    for (int j=0; j<YMAX; j++)
    {
        float _x = i*(1.0f/(float)XMAX); // + (0.5/512.0);
        float _y = j*(1.0f/(float)YMAX); // + (0.5/512.0);

        float tmp = p2d->sample(_x,_y,persistence);
        _2d_noise_array[XMAX*j + i] = tmp;
        sum += tmp;
    }

    delete p2d;
}

// NOTE: caller must free() the return value if not NULL
float* create_2d_noise_array(const float persistence, const int octaves, const unsigned int x, const unsigned int y)
{
    int size = x*y;
    GS_ASSERT(size > 0);
    if (size <= 0) return NULL;

    float* noise = (float*)malloc(size * sizeof(float));
    GS_ASSERT(noise != NULL);
    if (noise == NULL) return NULL;

    init_genrand(rand());

    const float xscale = 1.0f/(float)x;
    const float yscale = 1.0f/(float)y;

    class PerlinOctave2D* pgen = new class PerlinOctave2D(octaves);

    for (size_t i=0; i<x; i++)
    for (unsigned int j=0; j<y; j++)
        noise[i + x*j] = pgen->sample((float)i * xscale, (float)j * yscale, persistence);

    delete pgen;

    return noise;
}

void save_png(const char* filename, float* in, int xres, int yres)
{
    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for (int i=0; i < 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        int index = 4*(j*xres + i);

        float _v = in[j*xres+i];

        if (_v < 0.0) _v = 0.0f;
        if (_v > 1.0) _v = 1.0f;
        unsigned char v = ((int) 255.0f*_v);
        unsigned char v2 = gamma_correction[v];

        PBUFFER[index+0] = v2;
        PBUFFER[index+1] = v2;
        PBUFFER[index+2] = v2;
        PBUFFER[index+3] = 255;
    }

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        PBUFFER[4*(xres*j + i) + 3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if (NULL == temp_row)
        {
            printf("save_screenshot: not enough memory for surface inversion \n");
            return;
        }
        int pitch = xres * 4;
        int h = yres;

        height_div_2 = (int) (yres * .5);
        for (index = 0; index < height_div_2; index++)
        {
            memcpy((Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row);
    }

    size_t png_size;
    char* PNG_IMAGE = (char*) tdefl_write_image_to_png_file_in_memory(
        (const char*) PBUFFER, xres, yres, 4, &png_size);

    FILE * pFile;
    pFile = fopen (FileName , "wb");
    if (!pFile)
    {
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile);
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
}

OPTIMIZED
inline void save_perlin(const char* filename, float* in, int xres, int yres)
{
//#if DC_CLIENT
    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for (int i=0; i< 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        int index = 4*(j*xres + i);

        float _v = in[j*xres+i];

        if (_v < 0.0f)
        {
            _v *= -1;
            if (_v > 1.0f) _v = 1.0f;
            unsigned char v = ((int) 255.0f*_v);
            unsigned char v2 = gamma_correction[v];

            PBUFFER[index+0] = 0;
            PBUFFER[index+1] = v2;
            PBUFFER[index+2] = 0;
            PBUFFER[index+3] = 255;
        }
        else
        {
            if (_v > 1.0f) _v = 1.0f;
            unsigned char v = ((int) 255.0f*_v);
            unsigned char v2 = gamma_correction[v];

            PBUFFER[index+0] = 0;
            PBUFFER[index+1] = 0;
            PBUFFER[index+2] = v2;
            PBUFFER[index+3] = 255;
        }
    }

    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        PBUFFER[4*(xres*j + i) + 3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if (NULL == temp_row)
        {
            printf("save_screenshot: not enough memory for surface inversion \n");
            return;
        }
        int pitch = xres * 4;
        int h = yres;

        height_div_2 = (int) (yres * .5);
        for (index = 0; index < height_div_2; index++)
        {
            memcpy((Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy((Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row);
    }

    size_t png_size;
    char* PNG_IMAGE = (char*) tdefl_write_image_to_png_file_in_memory(
        (const char*) PBUFFER, xres, yres, 4, &png_size);

    FILE * pFile;
    pFile = fopen (FileName , "wb");

    if (!pFile)
    {
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile);
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
}

}   // t_gen
