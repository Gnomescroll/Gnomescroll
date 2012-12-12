#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void save_targa(char *file_name, float *data, unsigned int x, unsigned int y)
{
	FILE *image;
	char *foot = "TRUEVISION-XFILES.";
	unsigned int i, j;

	if((image = fopen(file_name, "wb")) == NULL)
	{
		printf("Could not create file: %s\n", file_name);
		return;
	}
	fputc(0, image);
	fputc(0, image);
	fputc(2, image); /* uncompressed */

	for(i = 3; i < 12; i++)
		fputc(0, image); /* ignore some stuff */
	fputc(x % 256, image);  /* size */
	fputc(x / 256, image);
	fputc(y % 256, image);
	fputc(y / 256, image);

/* Clamping nod needed since clut is not HDRI */
/*	for(i = 0; i < 3 * x * y; i++) 
	{
		if(data[i] > 1)
			data[i] = 1;
		if(data[i] < 0)
			data[i] = 0;
	}
	*/

	fputc(24, image); /* 24 bit image */

	for(j = 0; j < y; j++)
	{
		for(i = 0; i < x; i++)
		{
			fputc((int)(data[((y - j - 1) * x + i) * 3 + 0] * 255.0), image);
			fputc((int)(data[((y - j - 1) * x + i) * 3 + 2] * 255.0), image);
			fputc((int)(data[((y - j - 1) * x + i) * 3 + 1] * 255.0), image);
		}
	}
	/* Footer not described in TARGA spec but for some reason needed by photoshop */
	for(i = 0; i < 9; i++)
		fputc(0, image); // ignore some stuff
	for(i = 0; foot[i] != 0; i++)
		fputc(foot[i], image); // ignore some stuff
	fputc(0, image);
	fclose(image);
}

int main(int argc, char **argv)
{
	char *file_name = "hald_clut.tga";
	unsigned int i, j, k, size, level = 16;
	float *data, *p;

	printf("Usage %s <FILENAME> <LEVEL>\n", argv[0]);

	if(argc > 1)
		file_name = argv[1];

	if(argc > 2)
	{
		if(sscanf(argv[2], "%u", &level) != 1 || level > 16)
		{
			printf("Not valid level: %s (level must be 16 or lower)\n", argv[2]);
			exit(0);
		}
	}		
	size = level * level;
	data = p = malloc((sizeof *data) * size * size * size * 3);
	for(i = 0; i < size; i++) /**/
	{
		for(j = 0; j < size; j++)
		{
			for(k = 0; k < size; k++)
			{
				*p++ = (float)k / (float)(size - 1);
				*p++ = (float)j / (float)(size - 1);
				*p++ = (float)i / (float)(size - 1);
			}
		}
	}
	save_targa(file_name, data, level * level * level, level * level * level);
	printf("Hald CLUT with level %u created, %u by %u pixels size, file name: %s\n", level, size * level, size * level, file_name);
	return 0;
}

