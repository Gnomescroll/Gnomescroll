#pragma once

namespace Skybox
{


class Skyplane
{
	static const int dim = 4;

	unsigned char color[4*dim*dim];
	public:

	const static int samples = 5;

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


	void update(float _stheta, float _sphi)
	{
		const float radius = 256.0;

		float stheta = 2.0*3.14159*_stheta;
		float sphi   = 2.0*3.14159*_sphi;

		//sun position
		float sx = radius*sinf(stheta)*cosf(sphi);
		float sy = radius*sinf(stheta)*sinf(sphi);
		float sz = radius*cosf(stheta);

		printf("Sun Position: theta,phi= %.2f %.2f x,y,z= %2.f %2.f %2.f \n", _stheta, _sphi, sx,sy,sz);

		//sx = radius * 0.0;
		//sy = radius * 128;
		//sz = 64;

		//point position
		struct Vec3 u = vec3_init(0.0, 0.0, 1.0); 	//up
		struct Vec3 r = vec3_init(1.0, 0.0, 0.0);	//right

		const float size = 128.0;

		for(int i=0; i<dim; i++)
		{
			for(int j=0; j<dim; j++)
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
		float c = cosf(theta); //use inner product
		return 0.75*(1+c*c);
	}

	static float phase(struct Vec3 v1, struct Vec3 v2)
	{
		return 0.75*(1+vec3_cos2(v1,v2));
	}

	//function of wavelength
	static float out_scatter(struct Vec3 v1, struct Vec3 v2)
	{
		float _f = 1.0f / ((float) samples);
		float _d = _f * vec3_length(vec3_sub(v2,v1));
		struct Vec3 vi = vec3_scalar_mult(vec3_sub(v2, v1), _f);

		//H 0 is the scale height, which is the height at which the atmosphere's average density is found.
		//My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.
		static const float H0 = 0.25f;
		static const float SKYBOX_HEIGHT = 128.0; //skybox height


		//vi = vec3_scalar_mult(vi, _f);
		//scale height to 0 to 1
		float tmp = 0.0f;
		//the height is scaled so that 0 represents sea level and 1 is at the top of the atmosphere.
		//do over array of data?
		for(int i=0; i<samples; i++)
		{
			struct Vec3 tmp1 = vec3_add(v1, vec3_scalar_mult(vi, i));
			struct Vec3 tmp2 = vec3_add(v1, vec3_scalar_mult(vi, (i+1)));

			float _h = (tmp1.z + tmp2.z) / (-2.0*H0*SKYBOX_HEIGHT); //shortcut?
			//evaluate it at n points in array and then add them in

			tmp += _d*exp(_h); //intergrate over exp(-height / H0) 
		}

		return 4*3.14159*tmp;
	}

	//sun position, endpoint position
	static float in_scatter(struct Vec3 s, struct Vec3 v2)
	{

		const static float camera_z = 0.0f;
		static const float H0 = 0.25f;
		static const float SKYBOX_HEIGHT = 128.0; //skybox height

		struct Vec3 v1 = s; //sun position
		struct Vec3 c = vec3_init(0.0, 0.0, camera_z);
		float _f = 1.0f / ((float) samples);
		float _d = _f * vec3_length(vec3_sub(v2,v1));
		
		struct Vec3 vi = vec3_scalar_mult(vec3_sub(v2, c), _f);

		//compute sample points
		struct Vec3 tmp1[samples+1];	//should be constant for MSVC
		for(int i=0; i<=samples; i++)
			tmp1[i] = vec3_add(c, vec3_scalar_mult(vi, i));

		//compute rays from sample points to sun
		struct Vec3 _s[samples+1];
		for(int i=0; i<=samples; i++)
		{
			//do line sphere intersection to get length
			_s[i] = vec3_add(tmp1[i], vec3_scalar_mult(vec3_sub(s, tmp1[i]), 10.0));

		}

		float _t0 [samples+1]; //exp(h/H0)
		float _t1 [samples+1]; //out_scatter PPc
		float _t2 [samples+1]; //out_scatter PPa

		for(int i=0; i<=samples; i++)
		{
			_t0[i] = expf(tmp1[i].z / (H0*SKYBOX_HEIGHT));
			_t1[i] = out_scatter(tmp1[i], _s[i]); //out_scatter from current point to sun
			_t2[i] = out_scatter(tmp1[i], c); //out_scatter from current point to camera
		
			printf("%i: _t0,_t1,_t2= %.2f %.2f %.2f \n", i, _t0[i],_t1[i],_t2[i] );
		}

		float _r [samples+1];
		for(int i=0; i<=samples; i++)
			_r[i] = _t0[i]*expf( -_t1[i] -_t2[i] );


		float tmp = 0.0f;

		for(int i=0; i<samples; i++)
			tmp += _d*(_r[i] + _r[i+1]) / ( 2.0f);

		printf("phase: %.2f\n", phase(s, v2));
		printf("v2= %.2f %.2f %.2f  s= %.2f %.2f %.2f \n", v2.x,v2.y,v2.z, s.x,s.y,s.z);
		tmp *= phase(v2, s);
		return tmp;
	}

	//sun distance
	//atmosphere height
	//planet radius
	void update_point(int i, int j, float x, float y, float z, float sx, float sy, float sz)
	{
		struct Vec3 s = vec3_init(sx,sy,sz);
		
		//compute ray from camera to upper atmosphere
		//intersection with upper atomsphere
		const float ATMOSPHERE_DEPTH = 128.0;
		struct Vec3 b;
		float bl = ATMOSPHERE_DEPTH/sqrt(x*x+y*y+z*z);
		b.x = bl*x;
		b.y = bl*y;
		b.z = bl*z;

		float light = in_scatter(s, b);

		printf("light: %i %i = %.2f \n", i,j,light);

		if(light > 1.0)
			light = 1.0;
		if(light < 0.0)
			light = 0.0;

		color[i+dim*j] = (unsigned char) (light / 255.0);
	}

	void draw()
	{


	}
};

void init_rayleigh_scattering()
{
	class Skyplane S;

	S.update(0.0, 0.5);

	//abort();
}


void draw_rayleigh_scattering()
{


}


}