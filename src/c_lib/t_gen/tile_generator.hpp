#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{

unsigned char color_array[32*32*4];

void set_rgb(int x, int y, int r, int g, int b)
{
    color_array[4*32*y+ 4*x + 0] = r;
    color_array[4*32*y+ 4*x + 0] = g;
    color_array[4*32*y+ 4*x + 0] = b;
    color_array[4*32*y+ 4*x + 0] = 255;
}

void _save_png(const char* filename, int xres, int yres)
{
#if DC_CLIENT

    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);

    char* PBUFFER = (char*) color_array;

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

    //free(PBUFFER);
    free(PNG_IMAGE);
#endif
}


    //void save_png(const char* filename, float* in, int xres, int yres)


void generate_random_tile()
{

#if 0
    const int num_colors = 13;
    int colors[3*num_colors] =
    {
        87, 118, 45,
        92, 124, 46,
        73, 99, 37,
        76, 103, 38,

        80, 107, 40,
        71, 96, 35,
        69, 94, 35,
        93, 126, 48,

        81, 110, 41,
        78, 106, 40,
        77, 105, 39,
        65, 87, 33,
        70, 96, 36
    };


    for (int x=0; x<32; x++)
    for (int y=0; y<32; y++)
    {
        int index = rand() % num_colors;

        color_array[4*32*y+ 4*x + 0] = colors[3*index+0];
        color_array[4*32*y+ 4*x + 1] = colors[3*index+1];
        color_array[4*32*y+ 4*x + 2] = colors[3*index+2];
        color_array[4*32*y+ 4*x + 3] = 255;

    }
#else

    const int max = 20;
    int r = 81 ;
    int g = 110;
    int b = 41;


    for (int x=0; x<32; x++)
    for (int y=0; y<32; y++)
    {
        int R = r + (rand() % max) - max/2;
        int G = g + (rand() % max) - max/2;
        int B = b + (rand() % max) - max/2;

        color_array[4*32*y+ 4*x + 0] = R;
        color_array[4*32*y+ 4*x + 1] = G;
        color_array[4*32*y+ 4*x + 2] = B;
        color_array[4*32*y+ 4*x + 3] = 255;

    }

#endif

    _save_png("tile_00", 32, 32);
}

}   // t_gen
