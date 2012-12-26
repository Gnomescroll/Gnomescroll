#pragma once

#include <SDL/png_save.hpp>

namespace Skybox
{

class Skyplane
{


	//unsigned char color[4*dim*dim];
	public:
	static const int dim = 16;
	const static int samples = 5;


	static const float radius = 256.0; //sun distance from surface
	static const float size = 128.0;	//skybox plane size

	int sun_i, sun_j, sun_side; //used to find pixel for tracking sun
	float sun_cos_max;

	struct Vec3 vert[4];

	Skyplane()
	{
	/*
		for(int i=0; i<dim*dim; i++)
		{
			color[4*i+0] = 255;
			color[4*i+1] = 255;
			color[4*i+2] = 255;
			color[4*i+3] = 255;
		}
	*/
		//vertices of the quad
		vert[0] = vec3_init(-1.0, 0.0, 1.0);
		vert[1] = vec3_init(-1.0, 0.0, -1.0);
		vert[2] = vec3_init(-1.0, 0.0, 1.0);
		vert[3] = vec3_init(1.0, 0.0, 1.0);
	}


	float farray[6][dim*dim]; //intensity array


	void draw_sun(float theta, float phi, float x,float y,float z)
	{
		const float radius = 64.0;
		const float plane_depth = size/2.0;

		float stheta = 2*3.14159*theta;
		float sphi   = 2*3.14159*phi;

		struct Vec3 s;
		s.x = radius*sinf(stheta)*cosf(sphi);
		s.y = radius*sinf(stheta)*sinf(sphi);
		s.z = radius*cosf(stheta);

		s.z += plane_depth;

    	glDisable(GL_DEPTH_TEST);

	    glPointSize(5.0);

	    glColor3ub(0, 0, 255);

	    glBegin(GL_POINTS);

	    glVertex3f(x+s.x,y+s.y,z+s.z);

	    glEnd();
	    
	    glColor3ub(255, 255, 255);
    	glEnable(GL_DEPTH_TEST);
	    glPointSize(1.0);


	}
	void update(float theta, float phi)
	{
		const float plane_depth = size/2.0;

		const float _df = 1.0/ ((float) dim);

		float stheta = 2*3.14159*theta;
		float sphi   = 2*3.14159*phi;

		//sun position

		struct Vec3 s;
		s.x = radius*sinf(stheta)*cosf(sphi);
		s.y = radius*sinf(stheta)*sinf(sphi);
		s.z = radius*cosf(stheta);

		s.z += plane_depth;

		//point position
		//struct Vec3 u = vec3_init(0.0, 0.0, 1.0); 	//up
		//struct Vec3 r = vec3_init(0.0, 1.0, 0.0);	//right

		//center of plane

		//struct Vec3 f[6];

		const float _f[6*3] =
		{
			 0, 0, 1, //top
			 0, 0,-1,
			 1, 0, 0, //north
			-1, 0, 0,
			 0, 1, 0, //west
			 0,-1, 0
		};

		const float _r[6*3] =
		{
			 0,-1, 0, //top
			 0,-1, 0,
			 0, -1,0, //north
			 0, 1, 0,
			 1, 0, 0, //west
			 -1, 0, 0
		};

		const float _u[6*3] =
		{
			-1, 0, 0, //top
			 1, 0, 0,
			 0, 0,1, //north
			 0, 0, 1,
			 0, 0, 1, //west
			 0, 0, 1
		};

		struct Vec3 f[6];
		struct Vec3 r[6];
		struct Vec3 u[6];

		for(int i=0;i<6;i++)
		{
			f[i] = vec3_init(_f[3*i+0],_f[3*i+1],_f[3*i+2] );
			r[i] = vec3_init(_r[3*i+0],_r[3*i+1],_r[3*i+2] );
			u[i] = vec3_init(_u[3*i+0],_u[3*i+1],_u[3*i+2] );
		}
	/*
		for(int i=0; i<6 ;i++)
		{
			struct Vec3 f = vec3_init(_f[3*i+0],_f[3*i+1],_f[3*i+2] );
			struct Vec3 r = vec3_init(_r[3*i+0],_r[3*i+1],_r[3*i+2] );
			struct Vec3 u = vec3_init(_u[3*i+0],_u[3*i+1],_u[3*i+2] );

			struct Vec3 cu = vec3_cross(f,r);

			printf("%i: dot= %.2f fr, fu, ru= %.2f %.2f %.2f \n", i,
				vec3_dot(cu, u),
				vec3_dot(f, r),
				vec3_dot(f, u),
				vec3_dot(r, u)
				);
		}
	*/

		//center of plane
		struct Vec3 center[6];

		for(int i=0;i<6;i++)
		{
			center[i] = vec3_scalar_mult(f[i], plane_depth);
			center[i].z += plane_depth;

			if(i == 1)
				center[i].z -= -1.0;	//to prevent zero
		}

	/*
		struct Vec3 sun = center[2];
		sun = vec3_normalize(sun);
		s = vec3_scalar_mult(sun, 256.0);

		printf("!!! sx,sy,sz= %.2f %.2f %.2f \n" ,s.x,s.y,s.z);
	*/

	/*
		printf("Sun Position: theta,phi= %.2f %.2f 	sx,sy,sz= %.2f %.2f %.2f \n", 
			theta, phi,
			s.x,s.y,s.z);
	*/


		//int sun_i, sun_j, sun_side; //used to find pixel for tracking sun
		//float sun_cos_max;
		sun_cos_max = -10.0;

		const float camera_z = 64.0f;
		struct Vec3 c = vec3_init(0.0, 0.0, camera_z);

		for(int side=0; side<6; side++)
		{
			for(int i=0; i<dim; i++)
			{
				for(int j=0; j<dim; j++)
				{
					int _i = i -dim/2;
					int _j = j -dim/2;

					struct Vec3 b;
					b.x = center[side].x + _df*size*(_i*r[side].x + _j*u[side].x);
					b.y = center[side].y + _df*size*(_i*r[side].y + _j*u[side].y);
					b.z = center[side].z + _df*size*(_i*r[side].z + _j*u[side].z);

					//can inline this even
					
					float light = update_point(c, b, s);
					//float light = 0;
					farray[side][j*dim+i] = light;

					//debug stuff
					struct Vec3 tb = vec3_normalize(vec3_sub(b, c));
					struct Vec3 ts = vec3_normalize(vec3_sub(s, c));

					float v = vec3_dot(tb, ts);

					if(v > sun_cos_max)
					{
						sun_cos_max = v;
						sun_side = side;
						sun_i = i;
						sun_j = j;
					}


					//printf("light: %i %i = %.2f \n", i,j,light);
				}

			}
		}

/*
		int m_side;
		int m_i;
		int m_j;
		float m_cos = -1000;

		for(int side=0; side<6; side++)
		{
			for(int i=0; i<dim; i++)
			{
				for(int j=0; j<dim; j++)
				{
					int _i = i -dim/2;
					int _j = j -dim/2;

					struct Vec3 b;
					b.x = center[side].x + _df*size*(_i*r[side].x + _j*u[side].x);
					b.y = center[side].y + _df*size*(_i*r[side].y + _j*u[side].y);
					b.z = center[side].z + _df*size*(_i*r[side].z + _j*u[side].z);


					struct Vec3 tb = vec3_sub(b, c);
					//struct Vec3 tb = vec3_sub(_b, c);
					struct Vec3 ts = vec3_sub(s, c);

					tb = vec3_normalize(tb);
					ts = vec3_normalize(ts);

					float v = vec3_dot(tb, ts);

					if(v > m_cos)
					{
						m_cos = v;
						m_side = side;
						m_i = i;
						m_j = j;
					}


					//can inline this even
					
					//float light = update_point(c, b, s);
					//float light = 0;
					//farray[side][j*dim+i] = light;

					//printf("light: %i %i = %.2f \n", i,j,light);
				}

			}
		}
		farray[m_side][m_j*dim+m_i] = 1.0;
*/

/*
		printf("!!! sx,sy,sz= %.2f %.2f %.2f \n" ,sx,sy,sz);
		printf("theta,phi= %.2f %.2f ", theta, phi);s
		printf("Sun Position: x,y,z= %.2f %.2f %.2f \n", sx,sy,sz);
		printf("plane_center: x,y,z= %.2f %.2f %.2f \n", center.x, center.y, center.z);
*/

	/*
		printf("!!! DOT= %.2f cos2= %.2f \n", 
			vec3_dot(s, center[2]), 
			vec3_cos2(s, center[2])
			);

		for(int i=0; i<6; i++)
		{
			printf("%i: center= %.2f %.2f %.2f \n", i,center[i].x,center[i].y,center[i].z );
		}
	*/
	}

	void save(const char* filename, int side)
	{
		save_png(filename, farray[side], dim, dim);
	}

	void blit_to_buffer(float* fbuffer, int row)
	{

		if(row == 0)
		{
			for(int i=0; i<dim; i++)
			{
				float _d = 1.0 / ((float) dim);
				farray[0][i*dim+0] = _d*i;
			}
		}

		const int row_size = 6*dim;

		for(int side=0; side<6; side++)
		{
			int xoff = dim*side;
			int yoff = row_size*dim*row;
			for(int j=0; j<dim; j++)
			{
				int _yoff = yoff + j*row_size;

				for(int i=0; i<dim; i++)
				{
					int _xoff = xoff + i;
					fbuffer[_xoff + _yoff] = farray[side][i + j*dim];
				}
			}
		}

	}

	static float phase(struct Vec3 v1, struct Vec3 v2)
	{
/*
		float t0 = vec3_cos2(v1,v2);
		float t1 = vec3_cos2( vec3_normalize(v1),vec3_normalize(v2));

		if(abs(t0-t1) > 0.0001f)
		{
			printf("phase error: t0, t1= %.2f %.2f \n", t0,t1);
			GS_ASSERT(false);
		}
*/
		//GS_ASSERT(vec3_cos2(v1,v2) == vec3_cos2( vec3_normalize(v1),vec3_normalize(v2)) );
		return 0.75*(1+vec3_cos2(v1,v2)); //*.75
	}

	//H 0 is the scale height, which is the height at which the atmosphere's average density is found.
	//My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.

	static const float H0 = 0.25f;
	static const float SKYBOX_HEIGHT = 128.0; //skybox height


	//function of wavelength

	static const float damp = 0.50;
	//static const float damp = 0.001;

	static float out_scatter(const struct Vec3 &v1, const struct Vec3 &v2)
	{
		float _f = 1.0f / ((float) samples);
		float _d = _f * vec3_distance(v1, v2);
		struct Vec3 vi = vec3_scalar_mult(vec3_sub(v2, v1), _f);

		//H 0 is the scale height, which is the height at which the atmosphere's average density is found.
		//My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.
		//static const float H0 = 0.25f;
		//static const float SKYBOX_HEIGHT = 128.0; //skybox height


		//vi = vec3_scalar_mult(vi, _f);
		//scale height to 0 to 1

		//the height is scaled so that 0 represents sea level and 1 is at the top of the atmosphere.
		//do over array of data?

		float _r[samples+1];

		for(int i=0; i<=samples; i++)
		{
			struct Vec3 tmp1 = vec3_add(v1, vec3_scalar_mult(vi, i));
			float _h = tmp1.z /(H0*SKYBOX_HEIGHT);

			_r[i] = exp(-_h); //intergrate over exp(-height / H0) 
		}

		float tmp = 0.0f;

		for(int i=0; i<samples; i++)
			tmp += _d*(_r[i] + _r[i+1]) / ( 2.0f);

		return damp*tmp;
		//return 4*3.14159*tmp;


	}

	//start position, end position, sun position
	static float in_scatter(const struct Vec3 &a, const struct Vec3 &b, const struct Vec3 &s)
	{
		struct Vec3 c = a;

		struct Vec3 bc = vec3_sub(b,c);
		struct Vec3 bs = vec3_sub(s,c);

		//For each point P along the ray from Pa to Pb , 
		//PPc is the ray from the point to the sun
		//PPa is the ray from the sample point to the camera.

		float _f = 1.0f / ((float) samples);
		float _d = _f * vec3_distance(b,a);	//for intergral

		GS_ASSERT(abs( vec3_distance(a,b) - vec3_length(vec3_sub(b,a))) < 0.0001 );


		struct Vec3 vi = vec3_scalar_mult(vec3_sub(b, a), _f);

		//compute sample points
		struct Vec3 tmp1[samples+1];	//should be constant for MSVC

		for(int i=0; i<=samples; i++)
			tmp1[i] = vec3_add(a, vec3_scalar_mult(vi, i));

		//direction of sun from sample point
		struct Vec3 _s[samples+1];
		for(int i=0; i<=samples; i++)
		{
			//do line sphere intersection to get length
			//vector which is 20 units in direction of sun from tmp1

			//_s[i] = vec3_add(tmp1[i], vec3_scalar_mult(vec3_normalize(vec3_sub(tmp1[i], s)), 20.0));

			_s[i] = s; //skip to sun without raise limiting

		}

		float _t0 [samples+1]; //exp(h/H0)
		float _t1 [samples+1]; //out_scatter PPc
		float _t2 [samples+1]; //out_scatter PPa

		for(int i=0; i<=samples; i++)
		{
			_t0[i] = expf( -tmp1[i].z / (H0*SKYBOX_HEIGHT));
			_t1[i] = out_scatter(tmp1[i], _s[i]); //out_scatter from current point to sun
			_t2[i] = out_scatter(tmp1[i], c); //out_scatter from current point to camera
		
			//printf("%i: _t0,_t1,_t2= %.2f %.2f %.2f \n", i, _t0[i],_t1[i],_t2[i] );
		}

		float _r [samples+1];
		
		//debug
		//for(int i=0; i<=samples; i++)
		//	_r[i] = _t0[i]*expf( -_t1[i] -_t2[i] );
		for(int i=0; i<=samples; i++)
			_r[i] = _t0[i]*expf( -1.0*(_t1[i] + _t2[i]) );

/*
		for(int i=0; i<=samples; i++)
		{
			printf("%i: _r[i] = %.2f, _t0,_t1,_t2= %.2f %.2f %.2f, -_t1[i] -_t2[i]= %.2f, exp= %.2f \n", 
				i, _r[i], _t0[i],_t1[i],_t2[i],
				-_t1[i] -_t2[i], exp(-_t1[i] -_t2[i]));
		}
*/

		float tmp = 0.0f;

		for(int i=0; i<samples; i++)
			tmp += _d*(_r[i] + _r[i+1]) / ( 2.0f);

		//struct Vec3 s2 = vec3_sub(v2, c);	

		//printf("phase: %.2f\n", phase(v2, s));
/*
		printf("a= %.2f %.2f %.2f  b= %.2f %.2f %.2f s= %.2f %.2f %.2f  phase: %.2f \n", 
			a.x,a.y,a.z, 
			b.x,b.y,b.z,
			s.x,s.y,s.z, 
			phase(b, s) );
*/
		//debug
		tmp *= phase(bc, bs);
		
		return tmp;
	}

	static float sun_position(const struct Vec3 &a, const struct Vec3 &b, const struct Vec3 &s)
	{
		//compute ray from camera to upper atmosphere (use sphere and radius)
		//intersection with upper atomsphere
		
		//const float ATMOSPHERE_DEPTH = 128.0;
		//struct Vec3 _b;
		//_b = vec3_scalar_mult(vec3_normalize(b), ATMOSPHERE_DEPTH);

		struct Vec3 c = a;

		struct Vec3 tb = vec3_sub(b, c);

		//struct Vec3 tb = vec3_sub(_b, c);
		struct Vec3 ts = vec3_sub(s, c);

		tb = vec3_normalize(tb);
		ts = vec3_normalize(ts);

		float v = vec3_dot(tb, ts);

		if(v <= 0.0)
			v = 0.0;
		v *= v;

		return v;
		//return vec3_cos2(tb,ts);


		//return vec3_cos2(b,s);	//cos2 between end ray and sun, from camera
	}

	//sun distance
	//atmosphere height
	//planet radius

	float update_point(const struct Vec3 &c, const struct Vec3 &b, const struct Vec3 &s)
	{
		const float ATMOSPHERE_DEPTH = 128.0;
		//compute ray from camera to upper atmosphere (use sphere and radius)
		//intersection with upper atomsphere
		struct Vec3 _b;
		_b = vec3_scalar_mult(vec3_normalize(b), ATMOSPHERE_DEPTH);
		
		//return sun_position(c,_b,s);
		return in_scatter(c, _b, s);

	}

};

class SkyboxRender
{
	public:
	int time_count;

	class Skyplane sun;

	unsigned char* sun_rgba[6];

	unsigned int texture_array[6];

	SkyboxRender()
	{
		time_count = 0;



		glEnable(GL_TEXTURE_2D);

	    glGenTextures(6, texture_array);

	    for(int i=0; i<6; i++)
	    {
		    glBindTexture(GL_TEXTURE_2D, texture_array[i]);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glDisable(GL_TEXTURE_2D);

	    for(int i=0; i<6; i++)
	    {
			sun_rgba[i] = new unsigned char[sun.dim*sun.dim*6];
		}
	}

	~SkyboxRender()
	{

	}


	void increment_time()
	{

		const int tspeed = 1; //normally 1
		time_count = (time_count+tspeed) % 6750;


	}

	void update_skybox()
	{
		float sun_theta = time_count / 6750.0; //day length
		//float sun_phi = time_count / 3000.0;
		float sun_phi = 0;
		//sun.draw_sun(sun_theta, sun_phi, x,y,z);

		sun.update(sun_theta, sun_phi); //update float array

		//gamma
	    static unsigned char gamma_correction[256];
	    for(int i=0; i < 256; i++)
	    {
	        float intensity = ((float) i) / 255;
	        intensity = powf(intensity, 1.0f/2.2f)*255;
	        gamma_correction[i] = (unsigned char)((int) intensity);
	    }

	    //load RGB from array
		for(int side = 0; side<6; side++)
		{
			const int dim = sun.dim;
			const float* farray = sun.farray[side];

			for(int i=0; i<sun.dim; i++)
			for(int j=0; j<sun.dim; j++)
			{
				float _v = farray[j*sun.dim+i];
        		if( _v < 0.0) _v = 0.0f;
        		if( _v > 1.0) _v = 1.0f;
        		unsigned char v = ((int) 255.0f*_v );
        		unsigned char v2 = gamma_correction[v];

				sun_rgba[side][4*(i+j*dim)+0] = v2;
				sun_rgba[side][4*(i+j*dim)+1] = v2;
				sun_rgba[side][4*(i+j*dim)+2] = v2;
				sun_rgba[side][4*(i+j*dim)+3] = 255;

        		if(i==0 && j == 0)
        		{
        			sun_rgba[side][4*(i+j*dim)+0] = 255;
        		}

			}

		    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sun.dim, sun.dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		}

		glEnable(GL_TEXTURE_2D);

		for(int side = 0; side<6; side++)
		{
			glBindTexture(GL_TEXTURE_2D, texture_array[side]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sun.dim, sun.dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, sun_rgba[side] );
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_TEXTURE_2D);


		CHECK_GL_ERROR();

		//printf("update \n");

		//textures

		
	}


	void draw(float x, float y, float z)
	{
		x = 256;
		y = 256;
		z = 128;

		const float _f[6*3] =
		{
			 0, 0, 1, //top
			 0, 0,-1,
			 1, 0, 0, //north
			-1, 0, 0,
			 0, 1, 0, //west
			 0,-1, 0
		};

		const float _r[6*3] =
		{
			 0,-1, 0, //top
			 0,-1, 0,
			 0, -1,0, //north
			 0, 1, 0,
			 1, 0, 0, //west
			 -1, 0, 0
		};

		const float _u[6*3] =
		{
			-1, 0, 0, //top
			 1, 0, 0,
			 0, 0,1, //north
			 0, 0, 1,
			 0, 0, 1, //west
			 0, 0, 1
		};

		struct Vec3 f[6];
		struct Vec3 r[6];
		struct Vec3 u[6];

		for(int i=0;i<6;i++)
		{
			f[i] = vec3_init(_f[3*i+0],_f[3*i+1],_f[3*i+2] );
			r[i] = vec3_init(_r[3*i+0],_r[3*i+1],_r[3*i+2] );
			u[i] = vec3_init(_u[3*i+0],_u[3*i+1],_u[3*i+2] );
		}

		//center of plane
		struct Vec3 center[6];

		for(int i=0;i<6;i++)
		{
			const float plane_depth = sun.size/2.0;
			center[i] = vec3_scalar_mult(f[i], plane_depth);
			center[i].z += plane_depth;

			center[i].x += x;
			center[i].y += y;
			center[i].z += z;
		}


		glEnable(GL_TEXTURE_2D);

		for(int i=0; i<6; i++)
		{
			struct Vec3 rv, uv, lv, dv;
			//f = vec3_scalar_mult(_f[i], sun.size/2);
			rv = vec3_scalar_mult(r[i], sun.size/2);	//right
			uv = vec3_scalar_mult(u[i], sun.size/2);	//up

			//nf = vec3_scalar_mult(_f[i], -sun.size/2);
			lv = vec3_scalar_mult(r[i], -1.0*sun.size/2);	//left
			dv = vec3_scalar_mult(u[i], -1.0*sun.size/2);	//down

			struct Vec3 ul,bl,br,ur; 
			ul = vec3_add3(center[i], uv, lv);
			bl = vec3_add3(center[i], dv, lv);
			br = vec3_add3(center[i], dv, rv );
			ur = vec3_add3(center[i], uv, rv);

			glBindTexture(GL_TEXTURE_2D, texture_array[i]);

			//(0,0), 0,1 1,1 1,0

			glBegin(GL_QUADS);

			glTexCoord2f(0.0,1.0);

			glVertex3f(ul.x, ul.y, ul.z);

			glTexCoord2f(0.0,0.0);

			glVertex3f(bl.x, bl.y, bl.z);

			glTexCoord2f(1.0,0.0);

			glVertex3f(br.x, br.y, br.z);

			glTexCoord2f(1.0,1.0);
			glVertex3f(ur.x, ur.y, ur.z);

			glEnd();
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_TEXTURE_2D);


		float sun_theta = time_count / 6750.0; //day length
		//float sun_phi = time_count / 3000.0;
		float sun_phi = 0;
		sun.draw_sun(sun_theta, sun_phi, x,y,z);

		CHECK_GL_ERROR();

	//printf("draw\n");
	}
};


static class SkyboxRender* SR;
static class ConfigFileLoader CFL;
float test_float = 0.0f;

void init_rayleigh_scattering()
{
	class Skyplane S;

	if(true)
	{
		int max_div = 16;
		float _f = 1.0 / ((float) max_div);

		int dim = S.dim;
		float* fbuffer = new float[6*dim*dim*max_div];
		for(int i=0; i<max_div; i++)
		{
			char filename[128];

			snprintf(filename, 128,"light_%.2i", i);
			S.update(_f*((float) i) , 0.0);	//height, then rotation
			//S.save(filename);
			S.blit_to_buffer(fbuffer, i);
		}

		save_png("sky", fbuffer, 6*dim, dim*max_div);

	}
	else
	{
		S.update(0.0, 0.0);	//height, then rotation
		S.save("sky", 2);
	}
	//abort();


	SR = new SkyboxRender;
	//SR->update_skybox(); //wait for time to do this?

}


void draw_rayleigh_scattering()
{
/*
	CFL.set_float("test", &test_float);
	CFL.load_file("./settings/skybox");
	abort();
*/
	
	SR->increment_time();

	SR->draw(current_camera_position.x, current_camera_position.y, current_camera_position.z);

	static int update_count = 0;
	update_count++;
	if(update_count % 15 ==0 )
		SR->update_skybox();

}

/*
	CRAP
*/


}