#pragma once

namespace t_map
{ 

	//unsigned int light_texture_CLUT = 0;

class LightTextureGenerator
{

	public:

	static const int dim = 16;

	float values[3*dim*dim];

	unsigned int texture_array[1];

	LightTextureGenerator()
	{

	}

	~LightTextureGenerator()
	{

	}


	void init()
	{

		struct Vec3 ul;
		struct Vec3 ur;
		struct Vec3 br;
		struct Vec3 bl;

		ul = vec3_init(1.0, 0.0, 0.0);	//darkness
		br = vec3_init(1.0, 1.0, 1.0);	//light

		ur = vec3_init(0.0, 0.0, 1.0);
		bl = vec3_init(0.0, 1.0, 0.0);

		for(int i=0; i<dim; i++)
		{
			float xlerp = ((float) i) / ((float) (dim-1));

			struct Vec3 t1 = vec3_lerp(ul, ur, xlerp);
			struct Vec3 t2 = vec3_lerp(bl, br, xlerp);

			for(int j=0; j<dim; j++)
			{
				float ylerp = 1.0 - ((float) j) / ((float) (dim-1));

				struct Vec3 t3 = vec3_lerp(t1, t2, ylerp);

				values[3*(dim*j+i)+0] = t3.x;
				values[3*(dim*j+i)+1] = t3.y;
				values[3*(dim*j+i)+2] = t3.z;

			}
		}	

	}

	void save(const char* filename)
	{
		

		//save_png_RGB(const char* filename, float* in, int xres, int yres)
		save_png_RGB("light_texture", values, dim, dim);
	}

	void gen_textures()
	{
		glEnable(GL_TEXTURE_2D);
        glGenTextures(1, texture_array);

        for(int i=0; i<1; i++)
        {
            glBindTexture(GL_TEXTURE_2D, texture_array[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dim, dim, 0, GL_RGB, GL_FLOAT, values );
        
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_TEXTURE_2D);


	}
};

class LightTextureGenerator* LTG;

unsigned int generate_clut_light_texture()
{
	return LTG->texture_array[0];
}

void generate_light_texture()
{

	LTG = new LightTextureGenerator;
	LTG->init();
	LTG->save("light_texture");
	LTG->gen_textures();

	//light_texture_CLUT = LTG.texture_array[0];
}


}
