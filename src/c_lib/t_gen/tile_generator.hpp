#pragma once



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

void _save_png(const char* filename, float* in, int xres, int yres)
{
#if DC_CLIENT
    char FileName[128];
    sprintf(FileName,"./screenshot/%s.png", (char*) filename);

    char* PBUFFER = (char*) color_array;

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

    //free(PBUFFER);
    free(PNG_IMAGE);
#endif
}


	//void save_png(const char* filename, float* in, int xres, int yres)


void generate_random_tile()
{

	const int num_colors = 4;

	int colors[3*num_colors] =
	{
		0xBD, 0x56, 0x3F,
		0xBD, 0x37, 0x3F,
		0xBD, 0x48, 0x3F,
		0xBD, 0x69, 0x3F
	};



	for(int x=0; x<32; x++)
	for(int y=0; y<32; y++)
	{
		int index = rand() % num_colors;

		color_array[4*32*y+ 4*x + 0] = colors[3*index+0];
		color_array[4*32*y+ 4*x + 0] = colors[3*index+1];
		color_array[4*32*y+ 4*x + 0] = colors[3*index+2];
		color_array[4*32*y+ 4*x + 0] = 255;

	}

}



}