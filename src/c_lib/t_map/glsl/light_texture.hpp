#pragma once

namespace t_map
{ 

class LightTextureGenerator
{

	public:

	static const int dim = 16;

	float values[3*dim*dim];

	LightTextureGenerator()
	{

	}

	~LightTextureGenerator()
	{

	}


	inline static struct Vec3 lerp(struct Vec3 a, struct Vec3 b, float factor)
	{

	}

	void init()
	{

		struct Vec3 ul;
		struct Vec3 ur;
		struct Vec3 br;
		struct Vec3 bl;

		ul = vec3_init(0.0, 0.0, 0.0);	//darkness
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
				float ylerp = ((float) i) / ((float) (dim-1));

				struct Vec3 t3 = vec3_lerp(t1, t2, ylerp);

				
			}
		}	

	}

	void save(const char* filename)
	{
		

		//save_png_RGB(const char* filename, float* in, int xres, int yres)
		save_png_RGB("light_texture", values, dim, dim);
	}
};


void generate_light_texture()
{

	class LightTextureGenerator LTG;
	LTG.init();
	LTG.save("light_texture");
}


}
