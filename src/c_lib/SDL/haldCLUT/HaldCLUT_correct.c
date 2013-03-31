
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float *load_targa(char *file_name, unsigned int *x, unsigned int *y)
{
    FILE *image;
    float *draw = NULL;
    unsigned int i, j, identfeald, type, x_size, y_size, alpha;

    if ((image = fopen(file_name, "rb")) == NULL)
    {
        printf("can not open file: %s\n", file_name);
        exit(0);
    }

    identfeald = fgetc(image);
    if (0 != fgetc(image))
    {
        printf("Error: File %s a non suported palet image\n", file_name);
        exit(0);
    }
    type = fgetc(image);
    if (2 != type) /* type must be 2 uncompressed RGB */
    {
        printf("Error: File %s is not a uncompressed RGB image\n", file_name);
        exit(0);
    }
    for (i = 3; i < 12; i++)
        fgetc(image); /* ignore some stuff */
    x_size = fgetc(image);
    x_size += 256 * fgetc(image);
    y_size = fgetc(image);
    y_size += 256 * fgetc(image);

    alpha = fgetc(image);

    if (alpha != 24 && alpha != 32) /* type must be 24 or 32 bits RGB */
    {
        printf("Error: File %s is not a 24 or 32 bit RGB image\n", file_name);
        exit(0);
    }

    for (i = 0; i < identfeald; i++)
        fgetc(image); /* ignore some stuff */
    *x = x_size;
    *y = y_size;

    draw = malloc((sizeof *draw) * x_size * y_size * 3);

    if (alpha == 32)
    {
        for (j = 0; j < y_size; j++)
        {
            for (i = 0; i < x_size; i++)
            {
                fgetc(image); /* ignore alpha */
                draw[((y_size - j - 1) * x_size + i) * 3 + 2] = (float)fgetc(image) / (float)255.0;
                draw[((y_size - j - 1) * x_size + i) * 3 + 1] = (float)fgetc(image) / (float)255.0;
                draw[((y_size - j - 1) * x_size + i) * 3 + 0] = (float)fgetc(image) / (float)255.0;
            }
        }
    }else for (j = 0; j < y_size; j++)
    {
        for (i = 0; i < x_size; i++)
        {
            draw[((y_size - j - 1) * x_size + i) * 3 + 0] = (float)fgetc(image) / (float)255.0;
            draw[((y_size - j - 1) * x_size + i) * 3 + 2] = (float)fgetc(image) / (float)255.0;
            draw[((y_size - j - 1) * x_size + i) * 3 + 1] = (float)fgetc(image) / (float)255.0;
        }
    }
    fclose(image);
    return draw;
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

void correct_pixel(float *input, float *output, float *clut, unsigned int level)
{
    int color, red, green, blue, i, j;
    float tmp[6], r, g, b;
    level *= level;

    red = input[0] * (float)(level - 1);
    if (red > level - 2)
        red = (float)level - 2;
    if (red < 0)
        red = 0;

    green = input[1] * (float)(level - 1);
    if (green > level - 2)
        green = (float)level - 2;
    if (green < 0)
        green = 0;

    blue = input[2] * (float)(level - 1);
    if (blue > level - 2)
        blue = (float)level - 2;
    if (blue < 0)
        blue = 0;

    r = input[0] * (float)(level - 1) - red;
    g = input[1] * (float)(level - 1) - green;
    b = input[2] * (float)(level - 1) - blue;

    color = red + green * level + blue * level * level;

    i = color * 3;
    j = (color + 1) * 3;

    tmp[0] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[1] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[2] = clut[i] * (1 - r) + clut[j] * r;

    i = (color + level) * 3;
    j = (color + level + 1) * 3;

    tmp[3] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[4] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[5] = clut[i] * (1 - r) + clut[j] * r;

    output[0] = tmp[0] * (1 - g) + tmp[3] * g;
    output[1] = tmp[1] * (1 - g) + tmp[4] * g;
    output[2] = tmp[2] * (1 - g) + tmp[5] * g;

    i = (color + level * level) * 3;
    j = (color + level * level + 1) * 3;

    tmp[0] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[1] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[2] = clut[i] * (1 - r) + clut[j] * r;

    i = (color + level + level * level) * 3;
    j = (color + level + level * level + 1) * 3;

    tmp[3] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[4] = clut[i++] * (1 - r) + clut[j++] * r;
    tmp[5] = clut[i] * (1 - r) + clut[j] * r;

    tmp[0] = tmp[0] * (1 - g) + tmp[3] * g;
    tmp[1] = tmp[1] * (1 - g) + tmp[4] * g;
    tmp[2] = tmp[2] * (1 - g) + tmp[5] * g;

    output[0] = output[0] * (1 - b) + tmp[0] * b;
    output[1] = output[1] * (1 - b) + tmp[1] * b;
    output[2] = output[2] * (1 - b) + tmp[2] * b;
}


int main(int argc, char **argv)
{
    unsigned int i, x, y, level;
    float *clut, *input, *output;

/*  char *fake_argv[4] = {"./executable.exe", "input.tga", "clut.tga", "output.tga"};
    argc = 4;
    argv = fake_argv;
*/
    if (argc != 4)
    {
        printf("Usage %s <INPUT FILE NAME> <CLUT FILE NAME> <OUTPUT FILE NAME>\n", argv[0]);
        exit(0);
    }
    printf("Loading CLUT...\n");
    clut = load_targa(argv[2], &x, &y);

    if (x != y)
    {
        printf("Error: CLUT image %u is not square\n", argv[2]);
        exit(0);
    }
    for (level = 1; level * level * level < x; level++);
    if (level * level * level > x)
    {
        printf("Error: CLUT image %u is a non valid level\n", argv[2]);
        exit(0);
    }
    printf("Loading image...\n");
    input = load_targa(argv[1], &x, &y);
    output = malloc((sizeof *output) * x * y * 3);
    printf("Correcting image...\n");
    for (i = 0; i < x * y * 3; i += 3)
        correct_pixel(&input[i], &output[i], clut, level);
    printf("Done\n");
    save_targa(argv[3], output, x, y);
}
