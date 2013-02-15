
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void compute_hsv(float *output, float r, float g, float b)
{
    if (r < g)
    {
        if (r < b)
        {
            if (g < b)
            {
                output[0] = (4 - (g - r) / (b - r)) / 6.0;
                output[1] = 1 - r / b;
                output[2] = b;
            }else
            {
                output[0] = (2 + (b - r) / (g - r)) / 6.0;
                output[1] = 1 - r / g;
                output[2] = g;
            }
        }else
        {
            output[0] = (2 - (r - b) / (g - b)) / 6.0;
            output[1] = 1 - b / g;
            output[2] = g;
        }
    }else
    {
        if (r < b)
        {
            output[0] = (4 + (r - g) / (b - g)) / 6.0;
            output[1] = 1 - g / b;
            output[2] = b;
        }else
        {
            if (g < b)
            {
                output[0] = (6 - (b - g) / (r - g)) / 6.0;
                output[1] = 1 - g / r;
                output[2] = r;
            }else if (r == g && r == b)
            {
                output[0] = 0;
                output[1] = 0;
                output[2] = r;
            }else
            {

                output[0] = (0 + (g - b) / (r - b)) / 6.0;
                output[1] = 1 - b / r;
                output[2] = r;
            }
        }
    }
}

void compute_rgb(float *output, float h, float s, float v)
{
    if (h < 0)
        h = h + 5;
    if (h > 1)
        h = h - (float)((int)h);
    if (s < 0)
        s = 0;
    if (s > 1)
        s = 1;
    if (v < 0)
        v = 0;
    if (v > 1)
        v = 1;
    s = 1 - s;

    h *= 6;
    s *= v;
    switch ((int)h)
    {
        case 0 :
            output[0] = v;
            output[1] = s + (0 + h) * (v - s);
            output[2] = s;
        break;
        case 1 :
            output[0] = s + (2 - h) * (v - s);
            output[1] = v;
            output[2] = s;
        break;
        case 2 :
            output[0] = s;
            output[1] = v;
            output[2] = s + (h - 2) * (v - s);
        break;
        case 3 :
            output[0] = s;
            output[1] = s + (4 - h) * (v - s);
            output[2] = v;
        break;
        case 4 :
            output[0] = s + (h - 4) * (v - s);
            output[1] = s;
            output[2] = v;
        break;
        default :
            output[0] = v;
            output[1] = s;
            output[2] = s + (6 - h) * (v - s);
        break;
    }
}

void save_targa(char *file_name, float *data, unsigned int x, unsigned int y)
{
    FILE *image;
    char *foot = "TRUEVISION-XFILES.";
    unsigned int i, j;

    if ((image = fopen(file_name, "wb")) == NULL)
    {
        printf("Could not create file: %s\n", file_name);
        return;
    }
    fputc(0, image);
    fputc(0, image);
    fputc(2, image); /* uncompressed */

    for (i = 3; i < 12; i++)
        fputc(0, image); /* ignore some stuff */
    fputc(x % 256, image);  /* size */
    fputc(x / 256, image);
    fputc(y % 256, image);
    fputc(y / 256, image);

/* Clamping nod needed since clut is not HDRI */
/*  for (i = 0; i < 3 * x * y; i++)
    {
        if (data[i] > 1)
            data[i] = 1;
        if (data[i] < 0)
            data[i] = 0;
    }
    */

    fputc(24, image); /* 24 bit image */

    for (j = 0; j < y; j++)
    {
        for (i = 0; i < x; i++)
        {
            fputc((int)(data[((y - j - 1) * x + i) * 3 + 0] * 255.0), image);
            fputc((int)(data[((y - j - 1) * x + i) * 3 + 2] * 255.0), image);
            fputc((int)(data[((y - j - 1) * x + i) * 3 + 1] * 255.0), image);
        }
    }
    /* Footer not described in TARGA spec but for some reason needed by photoshop */
    for (i = 0; i < 9; i++)
        fputc(0, image); // ignore some stuff
    for (i = 0; foot[i] != 0; i++)
        fputc(foot[i], image); // ignore some stuff
    fputc(0, image);
    fclose(image);
}

float *generate_identity(unsigned int level)
{
    unsigned int i, j, k, size;
    float *data, *p;
    size = level * level;
    data = p = malloc((sizeof *data) * size * size * size * 3);
    for (i = 0; i < size; i++) /**/
    {
        for (j = 0; j < size; j++)
        {
            for (k = 0; k < size; k++)
            {
                *p++ = (float)k / (float)(size - 1);
                *p++ = (float)j / (float)(size - 1);
                *p++ = (float)i / (float)(size - 1);
            }
        }
    }
    return data;
}


void correction_end_twist(float *color, float twist)
{
    float tmp[3];
    compute_hsv(tmp, color[0], color[1], color[2]);
    compute_rgb(color, tmp[0] + twist * 0.3 * (tmp[2] * tmp[2]) - twist * ((1 - tmp[2]) * (1 - tmp[2])),
        tmp[1] * (1 - 0.5 * tmp[2] * tmp[2] * tmp[2]), tmp[2]);
}

void correction_mono_edge(float *color, float value)
{
    float tmp[3];
    compute_hsv(tmp, color[0], color[1], color[2]);
    compute_rgb(color, tmp[0], tmp[1] * (1 - value + tmp[2] * tmp[2] * 1.3 * 1.3 * value), tmp[2]);
}

void correction_desaturate_darks(float *color)
{
    float tmp[3];
    compute_hsv(tmp, color[0], color[1], color[2]);
    tmp[1] *= 1.2 - tmp[2];
    compute_rgb(color, tmp[0], tmp[1], tmp[2]);
}

void correction_dark_color(float *color, float red, float green, float blue)
{
    float tmp[3];
    compute_hsv(tmp, color[0], color[1], color[2]);
    color[0] = color[0] * tmp[2] + red * (1 - tmp[2]) * tmp[2];
    color[1] = color[1] * tmp[2] + green * (1 - tmp[2]) * tmp[2];
    color[2] = color[2] * tmp[2] + blue * (1 - tmp[2]) * tmp[2];
}

void correction_deep_dark_color(float *color, float red, float green, float blue)
{
    float tmp[3];
    compute_hsv(tmp, color[0], color[1], color[2]);

    tmp[2] = 1 - ((1 - tmp[2]) * (1 - tmp[2]));
    color[0] = color[0] * tmp[2] + red * (1 - tmp[2]) * tmp[2];
    color[1] = color[1] * tmp[2] + green * (1 - tmp[2]) * tmp[2];
    color[2] = color[2] * tmp[2] + blue * (1 - tmp[2]) * tmp[2];
}

int main(int argc, char **argv)
{
    char *file_name = "custom_hald_clut.tga";
    unsigned int i, j, k, size, level = 8;
    float *data;

    data = generate_identity(level);

    for (i = 0; i < level * level * level * level * level * level; i++)
    {
    //  correction_end_twist(&data[i * 3], 0.3);
    //  correction_mono_edge(&data[i * 3], 1);
    //  correction_desaturate_darks(&data[i * 3]);
    //  correction_dark_color(&data[i * 3], 0.6, 0.3, 1);
        correction_deep_dark_color(&data[i * 3], 0.6, 0.3, 1);
    }
    for (i = 0; i < 3 * level * level * level * level * level * level; i++)
    {
        if (data[i] > 1)
            data[i] = 1;
        if (data[i] < 0)
            data[i] = 0;
    }

    save_targa(file_name, data, level * level * level, level * level * level);
    printf("Custom Hald CLUT with level %u created, %u by %u pixels size, file name: %s\n", level, size * level, size * level, file_name);
    return 0;
}
