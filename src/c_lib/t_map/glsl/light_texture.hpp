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


	void init()
	{

	}

	void save(const char* filename)
	{
		

	}
};


void generate_light_texture()
{

	class LightTextureGenerator LTG;
	LTG.init();
	LTG.save("light_texture");
}


}