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

    float* cache;
    float cache_persistance;

	PerlinOctave3D(int _octaves)
    {
        cache = NULL;
        cache_persistance = 0.0;

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
        if(cache != NULL) delete[] cache;
	}

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

    void save_octaves(float _DEIN) {}

    void set_persistance(float persistance)
    {
        if(cache_persistance != persistance)
        {
            cache_persistance = persistance;
            populate_cache(persistance);
        }

    }
    
    void populate_cache(float persistance)
    {
        if(cache == NULL) cache = new float[(512/4)*(512/4)*(128/8)];

        const int XMAX = 512/4;
        const int YMAX = 512/4;
        const int ZMAX = 128/8;

        float x,y,z;

        for(int k=0; k<ZMAX; k++)
        for(int i=0; i<XMAX; i++)
        for(int j=0; j<YMAX; j++)
        {
            x = i*(4.0/512.0);
            y = j*(4.0/512.0);
            z = k*(8.0/512.0);

            cache[k*XMAX*YMAX + j*XMAX + i] = sample(x,y,z, persistance);
        }
    }


};

class MapGenerator1
{
    public:
    
    PerlinOctave3D* erosion3D;
    PerlinOctave2D* erosion2D;

    PerlinOctave2D* height2D;
    PerlinOctave2D* roughness2D;

    static const int XMAX = 512/4;
    static const int YMAX = 512/4;
    static const int ZMAX = 128/8;
    static const int XYMAX = 128*128;

/*
    Multiply by 3, subtract 2 and then clamp to -1 to 1
*/
  
    float* cache;
    MapGenerator1()
    {
        cache = NULL;

        erosion2D = new PerlinOctave2D(4);
        erosion3D = new PerlinOctave3D(4);

        height2D = new PerlinOctave2D(4);
        roughness2D = new PerlinOctave2D(4);
    }


    void set_persistance(float p1, float p2, float p3, float p4)
    {
        erosion2D->set_persistance(p2);
        erosion3D->set_persistance(p1);

        height2D->set_persistance(p3);
        roughness2D->set_persistance(p4);
    }

    void save_noisemaps()
    {
        height2D->save_octaves2(8,"n_height");
    }

    void populate_cache()
    {
        if(cache == NULL) cache = new float[XMAX*YMAX*ZMAX];

        for(int k=0; k<ZMAX; k++)
        for(int i=0; i<XMAX; i++)
        for(int j=0; j<YMAX; j++)
        {
            cache[k*XYMAX + j*XMAX + i] = calc(i,j,k);
        }
    }

    float calc(int i, int j, int k)
    {        
        float x = i*4;
        float y = j*4;
        float z = k*8;

        float v = 0.0; //value;

        int index2 = j*XMAX + i;
        int index3 = k*XYMAX + j*XMAX + i;

        float h2 = height2D->cache[index2];
        float r2 = roughness2D->cache[index2];

        float e2 = erosion2D->cache[index2];
        float e3 = erosion3D->cache[index3];

        /*
            Threshold height
        */

        static const float hrange = 16.0;   //half of range (can perturb this with another map)
        static const float hmin = 64;

        static const float _hmin = -1.0;
        static const float _hmax = 1.0;

        static const float _hmix = 0.5; //0.125;

        //if(k == 5) printf("h2= %f \n", h2);
        float tmp1 = _hmix*(z - (hmin + h2*hrange) );

        if(tmp1 < _hmin) tmp1 = _hmin;
        if(tmp1 > _hmax) tmp1 = _hmax;

        v += tmp1;

        //cache[index] = v;
        return v;
    }

    float get_cache(int i, int j, int k)
    {
        i &= 127;
        j &= 127;
        k &= 15;
        return cache[k*XYMAX + j*XMAX + i];
    }

    void generate_map(int tile_id)
    {
        populate_cache();


        for(int k=0; k<ZMAX-1; k++)
        for(int i=0; i<XMAX; i++)
        for(int j=0; j<YMAX; j++)
        {

            // Calculate noise contributions from each of the eight corners
            float n000= get_cache(i+0,j+0,k+0);
            float n100= get_cache(i+1,j+0,k+0);
            float n010= get_cache(i+0,j+1,k+0);
            float n110= get_cache(i+1,j+1,k+0);
            float n001= get_cache(i+0,j+0,k+1);
            float n101= get_cache(i+1,j+0,k+1);
            float n011= get_cache(i+0,j+1,k+1);
            float n111= get_cache(i+1,j+1,k+1);
            // Compute the fade curve value for each of x, y, z

        #if 0
            for(int i0=0; i0<4; i0++)
            {
                for(int j0=0; j0<4; j0++)

                {
                    for(int k0=0; k0<8; k0++)
                    {
                        float u = 0.25 * i0;
                        float v = 0.25 * j0;
                        float w = 0.125 * k0; 

                        float nx00 = mix(n000, n100, u);
                        float nx01 = mix(n001, n101, u);
                        float nx10 = mix(n010, n110, u);
                        float nx11 = mix(n011, n111, u);
                        // Interpolate the four results along y
                        float nxy0 = mix(nx00, nx10, v);
                        float nxy1 = mix(nx01, nx11, v);
                        // Interpolate the two last results along z
                        float nxyz = mix(nxy0, nxy1, w);

                        if(nxyz < 0.0)  t_map::set(4*i+i0, 4*j+j0, 8*k+k0, tile_id);
                    }
                }
            }
        #endif

        #if 1
            for(int i0=0; i0<4; i0++)
            {
                float u = 0.25 * i0;    //x interpolation
                float nx00 = mix(n000, n100, u);
                float nx01 = mix(n001, n101, u);
                float nx10 = mix(n010, n110, u);
                float nx11 = mix(n011, n111, u);

                for(int j0=0; j0<4; j0++)
                {
                    float v = 0.25 * j0;    //y interpolation
                    float nxy0 = mix(nx00, nx10, v);
                    float nxy1 = mix(nx01, nx11, v);

                    for(int k0=0; k0<8; k0++)
                    {
                        float w = 0.125 * k0;   //z interpolation
                        float nxyz = mix(nxy0, nxy1, w);

                        if(nxyz < 0.0)  t_map::set(4*i+i0, 4*j+j0, 8*k+k0, tile_id);
                    }
                }
            }
        #endif


        #if 0
            /*
                Optimize so it uses no multiplications
            */

            for(int i0=0; i0<4; i0++)
            {
                float u = 0.25 * i0;    //x interpolation
                float nx00 = mix(n000, n100, u);
                float nx01 = mix(n001, n101, u);
                float nx10 = mix(n010, n110, u);
                float nx11 = mix(n011, n111, u);

                for(int j0=0; j0<4; j0++)
                {
                    float v = 0.25 * j0;    //y interpolation
                    float nxy0 = mix(nx00, nx10, v);
                    float nxy1 = mix(nx01, nx11, v);

                    for(int k0=0; k0<8; k0++)
                    {
                        float w = 0.125 * k0;   //z interpolation
                        float nxyz = mix(nxy0, nxy1, w);
                    }
                }
            }
        #endif
        }
    }

};


void test_octave_3d()
{
	return;
    PerlinOctave3D m(6);
    m.save_octaves(8);
}

void test_octave_3d_map_gen(int tile_id)
{

    class MapGenerator1 mg;

    mg.set_persistance(0.5, 0.5, 0.5, 0.5);

    mg.generate_map(tile_id);
    mg.save_noisemaps();

}

}