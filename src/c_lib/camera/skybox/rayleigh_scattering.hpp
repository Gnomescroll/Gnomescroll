#pragma once

namespace Skybox
{

void save_png(const char* filename, float* in, int xres, int yres);


class Skyplane
{
	static const int dim = 32;

	//unsigned char color[4*dim*dim];
	public:

	const static int samples = 5;

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

	void update(float theta, float phi)
	{
		const float radius = 256.0;
		const float size = 128.0; //plane size
		const float plane_depth = size/2.0;

		const float _df = 1.0/ ((float) dim);

		float stheta = 3.14159*theta;
		float sphi   = 3.14159*phi;

		//sun position

		struct Vec3 s;
		s.x = radius*sinf(stheta)*cosf(sphi);
		s.y = radius*sinf(stheta)*sinf(sphi);
		s.z = radius*cosf(stheta);

		//point position
		struct Vec3 u = vec3_init(0.0, 0.0, 1.0); 	//up
		struct Vec3 r = vec3_init(0.0, 1.0, 0.0);	//right

		//center of plane
		struct Vec3 center = vec3_init(
			plane_depth,
			0.0,
			size/2.0
			);

		struct Vec3 sun = center;
		sun = vec3_normalize(sun);
		sun = vec3_scalar_mult(sun, 256.0);

		//sx = sun.x;
		//sy = sun.y;
		//sz = sun.z;

		printf("!!! sx,sy,sz= %.2f %.2f %.2f \n" ,s.x,s.y,s.z);

		printf("Sun Position: theta,phi= %.2f %.2f 	sx,sy,sz= %.2f %.2f %.2f \n", 
			theta, phi,
			s.x,s.y,s.z);

		const float camera_z = 0.0f;
		struct Vec3 c = vec3_init(0.0, 0.0, camera_z);

		for(int i=0; i<dim; i++)
		{
			for(int j=0; j<dim; j++)
			{
				int _i = i -dim/2;
				int _j = j -dim/2;

				struct Vec3 b;
				b.x = center.x + _df*size*(_i*r.x + _j*u.x);
				b.y = center.y + _df*size*(_i*r.y + _j*u.y);
				b.z = center.z + _df*size*(_i*r.z + _j*u.z);

				//can inline this even
				float light = update_point(c, b, s);

				farray[0][j*dim+i] = light;

				printf("light: %i %i = %.2f \n", i,j,light);
			}

		}

		for(int i=0; i<dim; i++)
		{
			float _d = 1.0 / ((float) dim);
			farray[0][i*dim+0] = _d*i;
		}

/*
		printf("!!! sx,sy,sz= %.2f %.2f %.2f \n" ,sx,sy,sz);
		printf("theta,phi= %.2f %.2f ", theta, phi);
		printf("Sun Position: x,y,z= %.2f %.2f %.2f \n", sx,sy,sz);
		printf("plane_center: x,y,z= %.2f %.2f %.2f \n", center.x, center.y, center.z);

		printf("!!! DOT= %.2f cos2= %.2f \n", 
			vec3_dot(sun, center), 
			vec3_cos2(sun, center)
			);
*/
	}

	void save(const char* filename)
	{
		save_png(filename, farray[0], dim, dim);
	}

	static float phase(struct Vec3 v1, struct Vec3 v2)
	{
		float t0 = vec3_cos2(v1,v2);
		float t1 = vec3_cos2( vec3_normalize(v1),vec3_normalize(v2));

		if(abs(t0-t1) > 0.0001f)
		{
			printf("phase error: t0, t1= %.2f %.2f \n", t0,t1);
			GS_ASSERT(false);
		}
		//GS_ASSERT(vec3_cos2(v1,v2) == vec3_cos2( vec3_normalize(v1),vec3_normalize(v2)) );
		return 0.75*(1+vec3_cos2(v1,v2)); //*.75
	}

	//H 0 is the scale height, which is the height at which the atmosphere's average density is found.
	//My implementation uses 0.25, so the average density is found 25 percent of the way up from the ground to the sky dome.

	static const float H0 = 0.25f;
	static const float SKYBOX_HEIGHT = 128.0; //skybox height


	//function of wavelength

	static const float damp = 0.100;

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
		
			printf("%i: _t0,_t1,_t2= %.2f %.2f %.2f \n", i, _t0[i],_t1[i],_t2[i] );
		}

		float _r [samples+1];
		
		//debug
		//for(int i=0; i<=samples; i++)
		//	_r[i] = _t0[i]*expf( -_t1[i] -_t2[i] );
		for(int i=0; i<=samples; i++)
			_r[i] = expf( -1.0*(_t1[i] + _t2[i]) );

		for(int i=0; i<=samples; i++)
		{
			printf("%i: _r[i] = %.2f, _t0,_t1,_t2= %.2f %.2f %.2f, -_t1[i] -_t2[i]= %.2f, exp= %.2f \n", 
				i, _r[i], _t0[i],_t1[i],_t2[i],
				-_t1[i] -_t2[i], exp(-_t1[i] -_t2[i]));
		}

		float tmp = 0.0f;

		for(int i=0; i<samples; i++)
			tmp += _d*(_r[i] + _r[i+1]) / ( 2.0f);

		//struct Vec3 s2 = vec3_sub(v2, c);	

		//printf("phase: %.2f\n", phase(v2, s));
		printf("a= %.2f %.2f %.2f  b= %.2f %.2f %.2f s= %.2f %.2f %.2f  phase: %.2f \n", 
			a.x,a.y,a.z, 
			b.x,b.y,b.z,
			s.x,s.y,s.z, 
			phase(b, s) );

		//debug
		//tmp *= phase(v2, s);
		
		return tmp;
	}

	//sun distance
	//atmosphere height
	//planet radius

	float update_point(const struct Vec3 &c, const struct Vec3 &b, const struct Vec3 &s)
	{
		//struct Vec3 s = vec3_init(sx,sy,sz);
		
		//compute ray from camera to upper atmosphere
		//intersection with upper atomsphere

		//float* farray = new float[dim*dim];

		const float ATMOSPHERE_DEPTH = 128.0;
		//struct Vec3 b = vec3_init(x,y,z);
		struct Vec3 _b;
		_b = vec3_scalar_mult(vec3_normalize(b), ATMOSPHERE_DEPTH);
		return in_scatter(c, _b, s);
	}

	void draw()
	{


	}
};

void init_rayleigh_scattering()
{
	return;
	class Skyplane S;

	if(true)
	{
		int max_div = 16;
		float _f = 0.5 / ((float) max_div);

		for(int i=0; i<max_div; i++)
		{
			char filename[128];

			snprintf(filename, 128,"light_%.2i", i);
			S.update(_f*((float) i), 0.0);	//height, then rotation
			S.save(filename);
		}
	}
	else
	{
		S.update(0.0, 0.0);	//height, then rotation
		S.save("sky");
	}
	abort();
}


void draw_rayleigh_scattering()
{


}

/*
	CRAP
*/

void save_png(const char* filename, float* in, int xres, int yres)
{
    char FileName[128];
    sprintf(FileName,SCREENSHOT_PATH "%s.png", (char*) filename);
    char* PBUFFER = (char*) malloc(4*xres*yres);

    static unsigned char gamma_correction[256];

    for(int i=0; i < 256; i++)
    {
        float intensity = ((float) i) / 255;
        intensity = powf(intensity, 1.0f/2.2f)*255;
        gamma_correction[i] = (unsigned char)((int) intensity);
    }

    for(int i=0; i < xres; i++) 
    for(int j=0; j < yres; j++)
    {
        int index = 4*(j*xres + i);

        float _v = in[j*xres+i];

        if( _v < 0.0) _v = 0.0f;
        if( _v > 1.0) _v = 1.0f;
        unsigned char v = ((int) 255.0f*_v );
        unsigned char v2 = gamma_correction[v];

        PBUFFER[index+0] = v2;
        PBUFFER[index+1] = v2;
        PBUFFER[index+2] = v2;
        PBUFFER[index+3] = 255;
    }

    for(int i=0; i < xres; i++)
    for(int j=0; j < yres; j++)
    {
        PBUFFER[4*(xres*j + i) + 3] = 255;
    }

    {
        int index;
        void* temp_row;
        int height_div_2;

        temp_row = (void *)malloc(4*xres);
        if(NULL == temp_row)
        {
            printf("save_screenshot: not enough memory for surface inversion \n");
            return;
        }
        int pitch = xres * 4;
        int h = yres;

        height_div_2 = (int) (yres * .5);
        for(index = 0; index < height_div_2; index++)    
        {
            memcpy( (Uint8 *)temp_row, (Uint8 *)(PBUFFER) + pitch * index, pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * index, (Uint8 *)PBUFFER + pitch * (h - index-1), pitch);
            memcpy( (Uint8 *)(PBUFFER) + pitch * (h - index-1), temp_row, pitch);
        }
        free(temp_row); 
    }

    size_t png_size;
    char* PNG_IMAGE = (char* ) tdefl_write_image_to_png_file_in_memory(
        (const char*) PBUFFER, xres, yres, 4, &png_size);

    FILE * pFile;
    pFile = fopen ( FileName , "wb" );
    if(!pFile)
    {
        printf("Error: could not save image.  Check that screenshot folder exists\n");
        return;
    }
    fwrite (PNG_IMAGE , 1 , png_size, pFile );
    fclose (pFile);

    free(PBUFFER);
    free(PNG_IMAGE);
}

}