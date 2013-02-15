#include "png_save.hpp"

typedef void (*pngDataWrite)(char*, float*, int, int, bool);

static unsigned char _png_gamma_correction[256];

void _init_png_gamma_correction()
{
    for (int i=0; i<256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        _png_gamma_correction[i] = (unsigned char)((int)intensity);
    }
}

const char* _png_write_full_filename(const char* fn)
{
    static char filename[GS_FN_MAX+1];
    size_t wrote = snprintf(filename, GS_FN_MAX+1, SCREENSHOT_PATH "%ss.png", fn);
    filename[GS_FN_MAX] = '\0';
    IF_ASSERT(wrote >= GS_FN_MAX+1)
        printf("Warning: truncating png filename to %s", filename);
    return filename;
}

static void _save_png(pngDataWrite writer, const char* fn, float* in, int xres, int yres, bool gamma_correction=true)
{
    const char* filename = _png_write_full_filename(fn);
    char* pbuffer = (char*)calloc(4*xres*yres, sizeof(char));

    _init_png_gamma_correction();

    writer(pbuffer, in, xres, yres, gamma_correction);

    int index;
    void* temp_row;
    int height_div_2;

    temp_row = (void *)calloc(4*xres, sizeof(char));
    IF_ASSERT(temp_row == NULL)
    {
        printf("Error: Not enough memory for surface inversion\n");
        return;
    }
    int pitch = xres * 4;
    int h = yres;

    height_div_2 = (int) (yres * 0.5f);
    for (index = 0; index < height_div_2; index++)
    {
        memcpy((Uint8 *)temp_row, (Uint8 *)(pbuffer) + pitch * index, pitch);
        memcpy((Uint8 *)(pbuffer) + pitch * index, (Uint8 *)pbuffer + pitch * (h - index-1), pitch);
        memcpy((Uint8 *)(pbuffer) + pitch * (h - index-1), temp_row, pitch);
    }
    free(temp_row);

    size_t png_size = 0;
    char* png_image = (char*)tdefl_write_image_to_png_file_in_memory(
        (const char*)pbuffer, xres, yres, 4, &png_size);

    FILE* pfile = fopen(filename , "wb");
    IF_ASSERT(pfile == NULL)
    {
        printf("Error: save_png, could not save image. Check that screenshot folder exists\n");
        return;
    }
    fwrite(png_image, 1, png_size, pfile);
    fclose(pfile);

    free(pbuffer);
    free(png_image);
}

static void _write_png_data(char* pbuffer, float* in, int xres, int yres, bool gamma_correction)
{
    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        int index = j*xres + i;
        float _v = in[j*xres+i];

        if (_v < 0.0) _v = 0.0f;
        if (_v > 1.0) _v = 1.0f;
        unsigned char v = (int)(255.0f*_v);

        for (int k=0; k<3; k++)
            pbuffer[4*index+k] = _png_gamma_correction[v];
        pbuffer[4*index+3] = 255;
    }
}

static void _write_png_rgb_data(char* pbuffer, float* in, int xres, int yres, bool gamma_correction)
{
    for (int i=0; i < xres; i++)
    for (int j=0; j < yres; j++)
    {
        int index = j*xres + i;
        for (int k=0; k<3;k++)
        {
            float _v = in[3*(j*xres+i)+k];

            if (_v < 0.0) _v = 0.0f;
            if (_v > 1.0) _v = 1.0f;
            unsigned char v = ((int)255.0f*_v);

            if (gamma_correction)
                pbuffer[4*index+k] = _png_gamma_correction[v];
            else
                pbuffer[4*index+k] = v;
        }
        pbuffer[4*index+3] = 255;
    }
}

void save_png(const char* fn, float* in, int xres, int yres, bool gamma_correction=true)
{
    _save_png(&_write_png_data, fn, in, xres, yres, gamma_correction);
}

void save_png_RGB(const char* fn, float* in, int xres, int yres, bool gamma_correction=true)
{
    _save_png(&_write_png_rgb_data, fn, in, xres, yres, gamma_correction);
}
