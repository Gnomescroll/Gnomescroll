#pragma once

//#pragma GCC push_options
//#pragma GCC pop_options
//#pragma GCC optimize ("O3")

#include <t_gen/twister.hpp>
#include <t_gen/noise_map2.hpp>

#include <common/time/physics_timer.hpp>


#include <t_map/server/env_process.hpp>

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

        ga = new unsigned char[ssize];

        generate_gradient_array();
    }

    ~PerlinField3D()
    {
        delete[] this->ga;
    }

    //__attribute((optimize("-O3")))
    void generate_gradient_array()
    {
        for(int i=0; i<this->ssize; i++) ga[i] = genrand_int32() % grad_max; //gradient number
    }


// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
    return x>=0 ? (int)x : (int)x-1;
}

__attribute__((optimize("-O3")))
inline int get_gradient(int x, int y, int z)
{
    x = x % xsize; //replace with bitmask
    y = y % xsize;
    z = z % zsize;

    return ga[x + y*xsize + z*xsize2];
}

public:

// Classic Perlin noise, 3D version
__attribute__((optimize("-O3")))
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
    unsigned long cache_seed;

	PerlinOctave3D(int _octaves)
    {
        cache = NULL;
        cache_persistance = 0.0;
        cache_seed = 0;

		octaves = _octaves;
		octave_array = new PerlinField3D[octaves];

		//for(int i=0; i<octaves; i++) octave_array[i].init(pow(2,i+2), 15);
		//for(int i=0; i<octaves; i++) octave_array[i].init(2*(i+1)+1, 4);
		//for(int i=0; i<octaves; i++) octave_array[i].init((i*(i+1))+1, 4);

        cache = new float[(512/4)*(512/4)*(128/8)];

        for(int i=0; i<octaves; i++) octave_array[i].init(
            primes[i+1], primes[i+1]);
	}

	~PerlinOctave3D()
	{
		delete[] octave_array;
        delete[] cache;
	}

    __attribute__((optimize("-O3")))
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

    void set_param(int persistance, unsigned long seed)
    {
        static int first_run = 0;
        bool update = false;
        if(seed != cache_seed || first_run == 0)
        {
            update = true;
            cache_seed = seed;
            init_genrand(seed);
            for(int i=0; i<octaves; i++)
                octave_array[i].generate_gradient_array();
        }

        if(persistance != cache_persistance || update)
        {
            cache_persistance = persistance;
            populate_cache(persistance); 
        }

        first_run++;
    }

    __attribute__((optimize("-O3")))
    void populate_cache(float persistance)
    {
        //if(cache == NULL) cache = new float[(512/4)*(512/4)*(128/8)];

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
    PerlinOctave2D* ridge2D;

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
        cache = new float[XMAX*YMAX*ZMAX];
        init_genrand(rand());

        erosion3D = new PerlinOctave3D(4);
        erosion2D = new PerlinOctave2D(4);

        height2D = new PerlinOctave2D(4);
        ridge2D = new PerlinOctave2D(4);

        roughness2D = new PerlinOctave2D(4);
    }

    ~MapGenerator1()
    {
        delete[] cache;
        delete erosion3D;
        delete erosion2D;
        delete height2D;
        delete ridge2D;
        delete roughness2D;

    }
    void set_persistance(float p1, float p2, float p3, float p4, float p5)
    {
        erosion2D->set_persistance(p2);
        erosion3D->set_persistance(p1);

        height2D->set_persistance(p3);
        ridge2D->set_persistance(p4);
        roughness2D->set_persistance(p5);
    }

    void save_noisemaps()
    {
        height2D->save_octaves2(8,"n_height");
    }


    __attribute__((optimize("-O3")))
    void populate_cache()
    {
        for(int k=0; k<ZMAX; k++)
        for(int i=0; i<XMAX; i++)
        for(int j=0; j<YMAX; j++)
        {
            cache[k*XYMAX + j*XMAX + i] = calc(i,j,k);
        }
    }


    __attribute((always_inline, optimize("-O3")))
    inline float calc(int i, int j, int k)
    {        
        float x = i*4;
        float y = j*4;
        float z = k*8;

        float v = 0.0; //value;

        int index2 = j*XMAX + i;
        int index3 = k*XYMAX + j*XMAX + i;

        float h2 = height2D->cache[index2];
        float r2 = roughness2D->cache[index2];

        float ri2 = ridge2D->cache[index2];

        float e2 = erosion2D->cache[index2];
        float e3 = erosion3D->cache[index3];

        /*
            Threshold height
        */

        static const float hmin = 64;

        if(ri2 < 0) ri2 *= -1;

        ri2 = ((int)(ri2 * 5));

        ri2 *= .20;

        ri2 *= 40;

        //v += 0.0625*(z - (hmin + ri2) );
        if( z < hmin + ri2) v -= 0.25;

        if(v < -1) v = -1;
        if(v > 1) v = 1;


        //v += e3*(e2*e2);
        v += 0.40*e3*e3;

        if(v < -1) v = -1;
        if(v > 1) v = 1;

        //return v;

        static const float hrange = 4.0;   //half of range (can perturb this with another map)

        static const float _hmin = -1.0;
        static const float _hmax = 1.0;

        static const float _hmix = 0.01; //0.125;

        //if(k == 5) printf("h2= %f \n", h2);

        static const float rmix = 0.8;


        //r2 = 5*r2 -2;
        //if(r2 > 1) r2 = 1.0;
        //if(r2 < 0.50) r2 = 0.50;

        if(r2 < 0.125) r2 = 0.0125; 
        float tmp1 = _hmix*(z - (hmin + r2*h2*hrange) );

        if(tmp1 < _hmin) tmp1 = _hmin;
        if(tmp1 > _hmax) tmp1 = _hmax;


        v += tmp1;

        //roughness
        //r2 = 3*r2 -2;


        //v += tmp1 * r2;
        //v += r2;
        //ridge

    /*
        static const float errosion_factor = 0.4;
        if(e2 > 0.0)
        {

            v += errosion_factor*e3;
        }
    */
        //


        return v;
    }


    __attribute__((optimize("-O3")))
    inline float get_cache(int i, int j, int k)
    {
        i &= 127;
        j &= 127;
        k &= 15;
        return cache[k*XYMAX + j*XMAX + i];
    }

    __attribute__((optimize("-O3")))
    void generate_map(int tile_id)
    {

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
            //map volume lerp: 962 ms 
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

                        if(nxyz < 0.0)
                        {

                            t_map::set(4*i+i0, 4*j+j0, 8*k+k0, tile_id);
                        }
                        else
                        {
                            t_map::set(4*i+i0, 4*j+j0, 8*k+k0, 0);
                        }
                    }
                }
            }
        #endif

        #if 0
            // map volume lerp: 1746 ms
            //float u,v,w;

            float u=0;
            for(int i0=0; i0<4; i0++)
            {
                //float u = 0.25 * i0;    //x interpolation
                u += 0.25;
                float nx00 = mix(n000, n100, u);
                float nx01 = mix(n001, n101, u);
                float nx10 = mix(n010, n110, u);
                float nx11 = mix(n011, n111, u);

                float v=0;
                for(int j0=0; j0<4; j0++)
                {
                    //float v = 0.25 * j0;    //y interpolation
                    v += 0.25;
                    float nxy0 = mix(nx00, nx10, v);
                    float nxy1 = mix(nx01, nx11, v);

                    float w = 0;
                    for(int k0=0; k0<8; k0++)
                    {
                        //float w = 0.125 * k0;   //z interpolation
                        w += 0.125;
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


            float bx = n000;
            float inc_x = 0.25*(n100-n000)

            float by = n010;
            float inc_x = 0.25*(n010-n010)

            for(int k0=0; k0<8; k0++)
            {
                float w = 0.125 * k0;   //z interpolation


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

class MapGenerator1* map_generator;

void test_octave_3d_map_gen(int tile_id)
{
/*
    void set_persistance(float p1, float p2, float p3, float p4, float p5)
    {
        erosion2D->set_persistance(p2);
        erosion3D->set_persistance(p1);

        height2D->set_persistance(p3);
        ridge2D->set_persistance(p4);
        roughness2D->set_persistance(p5);
*/

    int ti[6]; int i=0;
    ti[i++] = _GET_MS_TIME();

    map_generator = new MapGenerator1;
    ti[i++] = _GET_MS_TIME();

    //map_generator->set_persistance(0.6, 0.8, 0.8, 0.5, 0.85);

    map_generator->erosion3D->set_param(0.6, 15);
    map_generator->erosion2D->set_param(0.8, 17);
    map_generator->height2D->set_param(0.8, 21);
    map_generator->ridge2D->set_param(0.5, 57);
    map_generator->roughness2D->set_param(0.85, 81);

    ti[i++] = _GET_MS_TIME();

    map_generator->populate_cache();
    ti[i++] = _GET_MS_TIME();

    map_generator->generate_map(tile_id);
    ti[i++] = _GET_MS_TIME();

    map_generator->save_noisemaps();
    ti[i++] = _GET_MS_TIME();

    printf("Map Gen: \n");
    printf("0 init noise map: %i ms \n", ti[1]-ti[0] );
    printf("1 set persistance: %i ms \n", ti[2]-ti[1] );
    printf("3 populate cache: %i ms \n", ti[3]-ti[2] );
    printf("4 map volume lerp: %i ms \n", ti[4]-ti[3] );
    printf("5 save noisemaps: %i ms \n", ti[5]-ti[4] );
}


static unsigned long hash_string(unsigned char *str)
{
    unsigned long hash = 0;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

extern "C"
{
/*
    PerlinOctave3D* erosion3D;
    PerlinOctave2D* erosion2D;

    PerlinOctave2D* height2D;
    PerlinOctave2D* ridge2D;

    PerlinOctave2D* roughness2D;
*/
    void LUA_set_noisemap_param(int noise_map, float persistance, unsigned char* seed_string)
    {
        if(map_generator == NULL)   map_generator = new MapGenerator1;

        unsigned long seed = hash_string(seed_string);

        printf("i= %i seed= %li \n", noise_map, seed);
        switch(noise_map)
          {
             case 0:
                map_generator->erosion3D->set_param(persistance, seed);
                break;
             case 1:
                map_generator->erosion2D->set_param(persistance, seed);
                break;
             case 2:
                map_generator->height2D->set_param(persistance, seed);
                break;
             case 3:
                map_generator->ridge2D->set_param(persistance, seed);
                break;
             case 4:
                map_generator->roughness2D->set_param(persistance, seed);
                break;
             default:
                printf("LUA_set_noisemap_param Error: noisemap %i does not exist \n", noise_map);
                abort();
          }
    }


   float* LUA_get_noisemap_map_cache(int noise_map)
    {
        switch(noise_map)
          {
             case 0:
                return map_generator->erosion3D->cache;
                break;
             case 1:
                return map_generator->erosion2D->cache;
                break;
             case 2:
                return map_generator->height2D->cache;
                break;
             case 3:
                return map_generator->ridge2D->cache;
                break;
             case 4:
                return map_generator->roughness2D->cache;
                break;
             default:
                printf("LUA_get_noisemap_map_cache Error: noisemap %i does not exist \n", noise_map);
                abort();
          }
        return NULL;
    }

    float* LUA_get_map_lerp_array()
    {
        return map_generator->cache;
    }

    void LUA_generate_map()
    {
        int tile = t_map::dat_get_cube_id("regolith");
        map_generator->generate_map(tile);
        t_map::map_post_processing();
    }
}

}