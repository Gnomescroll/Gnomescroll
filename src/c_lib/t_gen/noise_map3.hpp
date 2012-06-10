#pragma once

#include <t_gen/twister.hpp>
#include <t_gen/noise_map2.hpp>

namespace t_gen
{

class PerlinField3D
{
    public:

    unsigned char* ga;  //gradient array
    float* grad; //gradient vector array
    //static const int ssize = 64*64*32;
    //static const int xsize = 64;

    int ssize;	//number of gradients

    int xsize; int xsize2;
    int zsize;

    float xscale;   //scale multiplier
    float zscale;

    static const int grad_max = 12;   //number of gradients

    //xsize is number of gradients
    PerlinField3D() {}

    void init(int _xsize, int _zsize)
    {
    	if(_xsize < 1) GS_ABORT();
        if(_zsize < 1) GS_ABORT();

    	xsize = _xsize; xsize2 = xsize*xsize;
        zsize = _zsize;
        ssize = xsize*xsize*zsize;

        init_genrand(rand());

        ga = new unsigned char[ssize];
        for(int i=0; i<ssize; i++)
        {
            ga[i] = genrand_int32() % grad_max; //gradient number
        }
    }

    ~PerlinField3D()
    {
        delete[] this->ga;
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
    return x>=0 ? (int)x : (int)x-1;
}

inline int get_gradient(int x, int y, int z)
{
    x = x % xsize; //replace with bitmask
    y = y % xsize;
    z = z % zsize;

    return ga[x + y*xsize + z*xsize2];
}

public:

// Classic Perlin noise, 3D version
float base(float x, float y, float z) 
{
    x *= xsize;
    y *= xsize;
    z *= zsize;

    //get grid point
    int X = fastfloor(x);
    int Y = fastfloor(y);
    int Z = fastfloor(z);

    x = x - X;
    y = y - Y;
    z = z - Z;

    int gi000 = get_gradient(X+0,Y+0,Z+0);
    int gi001 = get_gradient(X+0,Y+0,Z+1);
    int gi010 = get_gradient(X+0,Y+1,Z+0);
    int gi011 = get_gradient(X+0,Y+1,Z+1);

    int gi100 = get_gradient(X+1,Y+0,Z+0);
    int gi101 = get_gradient(X+1,Y+0,Z+1);
    int gi110 = get_gradient(X+1,Y+1,Z+0);
    int gi111 = get_gradient(X+1,Y+1,Z+1);
    
    // Calculate noise contributions from each of the eight corners
    float n000= dot(gi000, x, y, z);
    float n100= dot(gi100, x-1, y, z);
    float n010= dot(gi010, x, y-1, z);
    float n110= dot(gi110, x-1, y-1, z);
    float n001= dot(gi001, x, y, z-1);
    float n101= dot(gi101, x-1, y, z-1);
    float n011= dot(gi011, x, y-1, z-1);
    float n111= dot(gi111, x-1, y-1, z-1);
    // Compute the fade curve value for each of x, y, z
    
#if 1
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
#else
    float u = x;
    float v = y;
    float w = z;
#endif

    // Interpolate along x the contributions from each of the corners
    float nx00 = mix(n000, n100, u);
    float nx01 = mix(n001, n101, u);
    float nx10 = mix(n010, n110, u);
    float nx11 = mix(n011, n111, u);
    // Interpolate the four results along y
    float nxy0 = mix(nx00, nx10, v);
    float nxy1 = mix(nx01, nx11, v);
    // Interpolate the two last results along z
    float nxyz = mix(nxy0, nxy1, w);

    return nxyz * 0.707106781;   //-1 to 1 
}

};

class PerlinOctave3D
{
	public:
	int octaves;
	class PerlinField3D* octave_array;

	PerlinOctave3D(int _octaves)
	{
		octaves = _octaves;
		octave_array = new PerlinField3D[octaves];

		//for(int i=0; i<octaves; i++) octave_array[i].init(pow(2,i+2), 15);
		//for(int i=0; i<octaves; i++) octave_array[i].init(2*(i+1)+1, 4);
		//for(int i=0; i<octaves; i++) octave_array[i].init((i*(i+1))+1, 4);

        for(int i=0; i<octaves; i++) octave_array[i].init(
            primes[i+1], primes[i+1]);
	}

	~PerlinOctave3D()
	{
		delete[] octave_array;
	}

	void save_octaves(int _DEGREE)	//number of gradulations
	{}


	float sample(float x, float y, float z, float persistance)
	{	
		float p = 1.0;
		float tmp = 0.0;
		for(int i=0; i<octaves; i++)
		{
			tmp += octave_array[i].base(x,y,z);
			p *= persistance;
		}
		return tmp;
	}
};

class NoiseMap3D
{
    //for mixing

    
};


void test_octave_3d()
{
	return;
    PerlinOctave3D m(6);
    m.save_octaves(8);
}

void test_octave_3d_map_gen(int tile)
{
/*
	return;

    PerlinOctave2D m1(6);
    PerlinOctave2D m2(6);

	const int xres = 512;
	const int yres = 512;

	const float xresf = 1.0 / ((float) xres);
	const float yresf = 1.0 / ((float) yres);

    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {

        float x = i*xresf;
        float y = j*yresf;

        //value += 32*m.sample(x,y, 0.75);
        float roughness = m2.sample(x,y, 0.125);
        //if(roughness < 0) roughness = 0.0;
        if(roughness < 0) roughness *= -1.0/32.0;

        float value = 32 + 32*roughness*m1.sample(x,y, 0.450); //0.125);

        //value += 32*oct_0.sample(x, y, 32.5, PERSISTANCE);

        for (int k=0; k<value; k++) t_map::set(i,j,k,tile);


        //out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    }
*/
}

}