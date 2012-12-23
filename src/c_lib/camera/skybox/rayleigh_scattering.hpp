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
		float sy = radius*sin(stheta)*sin(sphi);
		float sz = radius*cos(stheta);

		//point position
		struct Vec3 u = vec3_init(0.0, 0.0, 1.0); 	//up
		struct Vec3 r = vec3_init(0.0, 1.0, 0.0);	//right

		const float size = 1.0;

		for(int i=0; i<dim; i++)
		{
			for(int j=0; i<dim; j++)
			{
				float x = size*(i*u.x + j*r.x);
				float y = size*(i*u.y + j*r.y);
				float z = size*(i*u.z + j*r.z);

				update_point(i,j, x,y,z, sx,sy,sz);
			}

		}
	}

	static float phase(float theta)
	{
		float c = cos(theta); //use inner product
		return 0.75*(1+c*c);
	}

	static float phase(struct Vec3 v1, struct Vec3 v2)
	{
		return 0.75*(1+vec3_dot(v1,v2));
	}

	//function of wavelength
	static float out_scatter(struct Vec3 v1, struct Vec3 v2, int samples)
	{
		float _f = 1.0f / ((float) samples);

		//H 0 is the scale height, which is the height at which the atmosphere's average density is found.
		//My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.
		static const flota H0 = 0.25f;

		struct Vec3 vi = vec3_sub(v2, v1);
		vi = Vec3 vec3_scalar_mult(vi, _f);

		float tmp = 0.0f;
		//the height is scaled so that 0 represents sea level and 1 is at the top of the atmosphere.
		//do over array of data?
		for(int i=0; i<samples; i++)
		{
			struct Vec3 tmp1 = vec3_add(v1, vec3_scalar_mult(vi, i*_f));
			struct Vec3 tmp2 = vec3_add(v1, vec3_scalar_mult(vi, (i+1)*_f));

			float _h = (tmp1.z + tmp2.z) / (-2.0*H0);

			tmp += exp(_h); //-height / H0 
		}

		return 4*3.14159*tmp;
	}

	static float in_scatter()
	{


	}

	void update_point(int i, int j, float x, float y, float z, float sx, float sy, float sz)
	{


	}

	void draw()
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