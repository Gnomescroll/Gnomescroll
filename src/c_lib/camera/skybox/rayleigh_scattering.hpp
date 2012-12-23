#pragma once

namespace Skybox
{


class Skyplane
{
	static const int dim = 64*64;

	unsigned char color[4*dim*dim];
	public:

	struct Vec3 vert[4];

	Skyplane()
	{
		for(int i=0; i<dim*dim; i++)
		{
			color[4*i+0] = 255;
			color[4*i+1] = 255;
			color[4*i+2] = 255;
			color[4*i+3] = 255;
		}

		//vertices of the quad
		vert[0] = vec3_init(-1.0, 0.0, 1.0);
		vert[1] = vec3_init(-1.0, 0.0, -1.0);
		vert[2] = vec3_init(-1.0, 0.0, 1.0);
		vert[3] = vec3_init(1.0, 0.0, 1.0);
	}


	void update(float stheta, float sphi)
	{
		const float radius = 128;

		//sun position
		float sx = radius*sin(stheta)*cos(sphi);
		float sy = radius*sin(stheta)*sin(sphi)
		float sz = radius*cos(stheta);

		for(int i=0; i<dim; i++)
		{
			for(int j=0; i<dim; j++)
			{

				update_point(i,j, s,y,z);
			}

		}
	}

	void update_point(int i, int j, float sx, float sy, float sz)
	{
		struct Vec3 u = vec3_init(0.0, 0.0, 1.0); 	//up
		struct Vec3 r = vec3_init(0.0, 1.0, 0.0);	//right

		const size = 
	}

	draw()
	{


	}
};

void init_rayleigh_scattering()
{



}


void draw_rayleigh_scattering()
{


}


}