#pragma once


//int texture_make(float *data, unsigned int dimensions, unsigned int x, unsigned int y, unsigned int z)
//{

void compute_hsv(float *output, float r, float g, float b)
{
	if(r < g)
	{
		if(r < b)
		{
			if(g < b)
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
		if(r < b)
		{
			output[0] = (4 + (r - g) / (b - g)) / 6.0;
			output[1] = 1 - g / b;
			output[2] = b;
		}else
		{
			if(g < b)
			{
				output[0] = (6 - (b - g) / (r - g)) / 6.0;
				output[1] = 1 - g / r;
				output[2] = r;
			}else if(r == g && r == b)
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

float *generate_identity(unsigned int level)
{
	unsigned int i, j, k, size;
	float *data, *p;	
	size = level * level;
	data = (float*) malloc( sizeof(float) * size * size * size * 3);
	p = data;

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
	return data;
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

int generate_clut_texture()
{

	static unsigned int texture_id = 0;
	if(texture_id != 0)
		return texture_id;

	//char *file_name = "custom_hald_clut.tga";
	//int i;
	int level = 8;
	float *data;

	data = generate_identity(level);

	for(int i = 0; i < level * level * level * level * level * level; i++)
	{
	//	correction_end_twist(&data[i * 3], 0.3);
	//	correction_mono_edge(&data[i * 3], 1);
	//	correction_desaturate_darks(&data[i * 3]);
	//	correction_dark_color(&data[i * 3], 0.6, 0.3, 1);
		correction_deep_dark_color(&data[i * 3], 0.6, 0.3, 1);
	}
	for(int i = 0; i < 3 * level * level * level * level * level * level; i++) 
	{
		if(data[i] > 1)
			data[i] = 1;
		if(data[i] < 0)
			data[i] = 0;
	}



	int w = level*level;
	int h = level*level;
	int d = level*level;

	glGenTextures(1, &texture_id);

	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, texture_id);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //GLuint internalFormat = GL_SRGB8_ALPHA8_EXT; //GL_RGBA;
    //GLuint internalFormat = GL_SRGB8_ALPHA8; //GL_RGBA;
    GLuint internalFormat = GL_RGB;
    GLuint format = GL_RGB;
    //glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, w, h, d, 0, format, GL_UNSIGNED_BYTE, data); //
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, w, h, d, 0, format, GL_FLOAT, data); //

	//p_glTexImage3DEXT(GL_TEXTURE_3D, 0, GL_RGB, x, y, z, 0, GL_RGB, GL_FLOAT, data);

	glDisable(GL_TEXTURE_3D);
	return texture_id;
}

