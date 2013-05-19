#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_gen/twister.hpp>
#include <t_gen/noise_map2.hpp>
#include <common/time/physics_timer.hpp>
#include <t_map/server/env_process.hpp>

#ifdef __MSVC__
# pragma optimize("gt", on)
#endif

namespace t_gen
{

class PerlinField3D
{

    public:

    unsigned char* ga;  //gradient array
    float* grad; //gradient vector array
    //static const int ssize = 64*64*32;
    //static const int xsize = 64;

    int ssize;  //number of gradients

    int xsize; int xsize2;
    int zsize;

    float xscale;   //scale multiplier
    float zscale;

    static const int grad_max = 12;   //number of gradients

    //xsize is number of gradients
    PerlinField3D() : ga(NULL), grad(NULL) {}

    void init(int _xsize, int _zsize)
    {
        if (_xsize < 1) GS_ABORT();
        if (_zsize < 1) GS_ABORT();

        xsize = _xsize;
        xsize2 = xsize*xsize;
        zsize = _zsize;
        ssize = xsize*xsize*zsize;

        ga = new unsigned char[ssize];

        generate_gradient_array();
    }

    ~PerlinField3D()
    {
        if (this->ga != NULL) delete[] this->ga;
    }

    //OPTIMIZED
    void generate_gradient_array()
    {
        for (int i=0; i<this->ssize; i++) ga[i] = genrand_int32() % grad_max; //gradient number
    }

    OPTIMIZED
    inline int get_gradient(int x, int y, int z)
    {
        x = x % xsize; //replace with bitmask
        y = y % xsize;
        z = z % zsize;

        return ga[x + y*xsize + z*xsize2];
    }

    public:

    // Classic Perlin noise, 3D version
    OPTIMIZED
    float base(float x, float y, float z)
    {
        x *= xsize;
        y *= xsize;
        z *= zsize;

        //get grid point
        int X = fast_floor(x);
        int Y = fast_floor(y);
        int Z = fast_floor(z);

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

        return nxyz * 0.707106781f;   //-1 to 1
    }

};

class PerlinOctave3D
{
    private:
        int runs;
    public:
        int octaves;
        class PerlinField3D* octave_array;

        float* cache;
        float cache_persistence;
        unsigned long cache_seed;

    PerlinOctave3D(int _octaves) : runs(0)
    {
        cache_persistence = 0.0f;
        cache_seed = 0;

        octaves = _octaves;
        octave_array = new PerlinField3D[octaves];

        //for (int i=0; i<octaves; i++) octave_array[i].init(pow(2,i+2), 15);
        //for (int i=0; i<octaves; i++) octave_array[i].init(2*(i+1)+1, 4);
        //for (int i=0; i<octaves; i++) octave_array[i].init((i*(i+1))+1, 4);

        cache = new float[(512/4)*(512/4)*(128/4)];

        for (int i=0; i<octaves; i++) octave_array[i].init(
            primes[i+1], primes[i+1]);
    }

    ~PerlinOctave3D()
    {
        delete[] octave_array;
        delete[] cache;
    }

    OPTIMIZED
    float sample(float x, float y, float z, float persistence)
    {
        float p = 1.0f;
        float tmp = 0.0f;
        for (int i=0; i<octaves; i++)
        {
        #if 1
            tmp += octave_array[i].base(x,y,z);
            p *= persistence;
        #else
            p *= persistence;
            tmp += p*octave_array[i].base(x,y,z);
        #endif
        }
        return tmp;
    }

    void save_octaves(float _DEIN) {}

    void set_persistence(float persistence)
    {
        if (cache_persistence != persistence)
        {
            cache_persistence = persistence;
            populate_cache(persistence);
        }

    }

    void set_param(int persistence)
    {
        bool update = false;
        if (runs == 0)
        {
            update = true;
            this->cache_seed = rand();
            seed_twister(this->cache_seed);
            for (int i=0; i<octaves; i++)
                octave_array[i].generate_gradient_array();
        }

        if (persistence != cache_persistence || update)
        {
            cache_persistence = persistence;
            populate_cache(persistence);
        }

        runs++;
    }

    OPTIMIZED
    void populate_cache(float persistence)
    {
        //if (cache == NULL) cache = new float[(512/4)*(512/4)*(128/8)];

        const int XMAX = 512/4;
        const int YMAX = 512/4;
        const int ZMAX = 128/4;

        float x,y,z;

        for (int k=0; k<ZMAX; k++)
        for (int i=0; i<XMAX; i++)
        for (int j=0; j<YMAX; j++)
        {
            x = i*(4.0f/512.0f);
            y = j*(4.0f/512.0f);
            z = k*(4.0f/512.0f);

            cache[k*XMAX*YMAX + j*XMAX + i] = sample(x,y,z, persistence);
        }
    }

};

//http://en.wikipedia.org/wiki/Sigmoid_function

//reaction is inverse of interval over which effect occurs
OPTIMIZED
inline float sigmoid(float t, float mean, float reaction)
{
    t -= mean;
    t *= reaction;
    return (1.0f / (1+expf(-t)));
}

//-1 to 1
OPTIMIZED
inline float sigmoid2(float t, float mean, float reaction)
{
    t = t -mean;
    t *= reaction;
    return (2.0f / (1+expf(-t))) - 1;
}


class MapGenerator1
{
    public:

        PerlinOctave3D* erosion3D;
        PerlinOctave2D* erosion2D;

        PerlinOctave2D* height2D;
        PerlinOctave2D* ridge2D;

        PerlinOctave2D* roughness2D;

        static const int xmax = 512/4;
        static const int ymax = 512/4;
        static const int zmax = 128/4;
        static const int xymax = xmax*ymax;

        /*
            Multiply by 3, subtract 2 and then clamp to -1 to 1
        */
        float* cache;

    /*
        Octaves
    */
    MapGenerator1()
    {
        seed_twister(rand());

        this->cache = new float[this->xmax * this->ymax * this->zmax];
        this->erosion3D = new PerlinOctave3D(4);
        this->erosion2D = new PerlinOctave2D(4);
        this->height2D = new PerlinOctave2D(6);
        this->ridge2D = new PerlinOctave2D(4);
        this->roughness2D = new PerlinOctave2D(4);
    }

    ~MapGenerator1()
    {
        if (this->cache != NULL) delete[] this->cache;
        if (this->erosion3D != NULL) delete this->erosion3D;
        if (this->erosion2D != NULL) delete this->erosion2D;
        if (this->height2D != NULL) delete this->height2D;
        if (this->ridge2D != NULL) delete this->ridge2D;
        if (this->roughness2D != NULL) delete this->roughness2D;
    }

    void set_persistence(float p1, float p2, float p3, float p4, float p5)
    {
        this->erosion2D->set_persistence(p2);
        this->erosion3D->set_persistence(p1);

        this->height2D->set_persistence(p3);
        this->ridge2D->set_persistence(p4);
        this->roughness2D->set_persistence(p5);
    }

    void save_noisemaps()
    {
        this->height2D->save_octaves2(8,"n_height");
    }


    OPTIMIZED
    void populate_cache()
    {
        for (int k=0; k<this->zmax; k++)
        for (int i=0; i<this->xmax; i++)
        for (int j=0; j<this->ymax; j++)
        {
            this->cache[k*this->xymax + j*this->xmax + i] = calc(i,j,k);
        }
    }

    //ROCK if less than zero
    ALWAYS_INLINE OPTIMIZED
    float calc(int i, int j, int k)
    {
        //float x = i*4;
        //float y = j*4;
        float z = k*4;

        float v = 0.0f; //value;

        int index2 = j*this->xmax + i;
        int index3 = k*this->xymax + j*this->xmax + i;

        float h2 = this->height2D->cache[index2];
        //float r2 = roughness2D->cache[index2];
        //float ri2 = ridge2D->cache[index2];

        //printf("ri2= %f \n", ri2);
        //float e2 = erosion2D->cache[index2];
        float e3 = this->erosion3D->cache[index3];

        //printf("e3= %f \n", e3);
        #if 1
        static const float hmin = 64;

        /*
        if (ri2 < 0) ri2 *= -1;
        ri2 = ((int)(ri2 * 5));
        ri2 *= 0.20f;
        ri2 *= 40;
        if (z < hmin + ri2) v -= 0.25f; //0,25  //hard threshold
        if (v < -1) v = -1;
        if (v > 1) v = 1;
        */

        v += 2.40f*e3*e3;   //only erodes in this form

        static const float hrange = 4.0f;   //half of range (can perturb this with another map)

        //static const float _hmin = -1.0f;
        //static const float _hmax = 1.0f;

        static const float _hmix = 0.050f; //0.25;

        //if (r2 < 0.125f) r2 = 0.0125f;
        //float tmp1 = _hmix*(z - (hmin + r2*h2*hrange));

        //float tmp1 = _hmix*(z - (hmin + r2*h2*hrange));

        float tmp1 = _hmix*(z - hmin - h2*hrange);

        //if (tmp1 < _hmin) tmp1 = _hmin;
        //if (tmp1 > _hmax) tmp1 = _hmax;

        v += tmp1;

        return v;
        #else
        /*
            Threshold height
        */

        static const float hmin = 96;

        if (ri2 < 0) ri2 *= -1;

        ri2 = ((int)(ri2 * 5));

        ri2 *= 0.20f;

        ri2 *= 40;

        if (z < hmin + ri2) v -= 0.25f; //0,25  //hard threshold

        if (v < -1) v = -1;
        if (v > 1) v = 1;

        # if 1
        v += 0.40f*e3*e3;   //only erodes in this form

        if (v < -1) v = -1;
        if (v > 1) v = 1;
        # endif
        //return v;

        static const float hrange = 4.0f;   //half of range (can perturb this with another map)

        static const float _hmin = -1.0f;
        static const float _hmax = 1.0f;

        static const float _hmix = 0.01f; //0.125;

        if (r2 < 0.125f) r2 = 0.0125f;
        float tmp1 = _hmix*(z - (hmin + r2*h2*hrange));

        if (tmp1 < _hmin) tmp1 = _hmin;
        if (tmp1 > _hmax) tmp1 = _hmax;


        v += tmp1;

        return v;
        #endif
    }

    ALWAYS_INLINE OPTIMIZED
    float get_cache(int i, int j, int k)
    {
        i &= 0x7F;
        j &= 0x7F;
        k &= 0x1F;
        return cache[k*this->xymax + j*this->xmax + i];
    }

    OPTIMIZED
    void generate_map(CubeType tile_id)
    {
        for (int k=0; k<this->zmax-1; k++)
        for (int i=0; i<this->xmax; i++)
        for (int j=0; j<this->ymax; j++)
        {
            // Calculate noise contributions from each of the eight corners
            float n000 = this->get_cache(i+0,j+0,k+0);
            float n100 = this->get_cache(i+1,j+0,k+0);
            float n010 = this->get_cache(i+0,j+1,k+0);
            float n110 = this->get_cache(i+1,j+1,k+0);
            float n001 = this->get_cache(i+0,j+0,k+1);
            float n101 = this->get_cache(i+1,j+0,k+1);
            float n011 = this->get_cache(i+0,j+1,k+1);
            float n111 = this->get_cache(i+1,j+1,k+1);

            //map volume lerp: 962 ms
            for (int i0=0; i0<4; i0++)
            {
                float u = 0.25f * i0;    //x interpolation
                float nx00 = mix(n000, n100, u);
                float nx01 = mix(n001, n101, u);
                float nx10 = mix(n010, n110, u);
                float nx11 = mix(n011, n111, u);

                for (int j0=0; j0<4; j0++)
                {
                    float v = 0.25f * j0;    //y interpolation
                    float nxy0 = mix(nx00, nx10, v);
                    float nxy1 = mix(nx01, nx11, v);

                    for (int k0=0; k0<4; k0++)
                    {
                        float w = 0.25f * k0;   //z interpolation
                        float nxyz = mix(nxy0, nxy1, w);

                        if (nxyz < 0.0)
                        {
                            t_map::set_fast(4*i+i0, 4*j+j0, 4*k+k0, tile_id);
                        }
                        else
                        {
                            t_map::set_fast(4*i+i0, 4*j+j0, 4*k+k0, EMPTY_CUBE);
                        }
                    }
                }
            }
        }
    }
};


void test_octave_3d()
{
    //class PerlinOctave3D* m = new class PerlinOctave3D(6);
    //m->save_octaves(8);
    //delete m;
}

class MapGenerator1* map_generator = NULL;

void test_octave_3d_map_gen(CubeType tile_id)
{
    IF_ASSERT(map_generator == NULL) return;

    int ti[6];
    int i=0;
    ti[i++] = _GET_MS_TIME();
    ti[i++] = _GET_MS_TIME();

    //set seeds for each of the noise maps
    map_generator->erosion3D->set_param(0.9f);
    map_generator->erosion2D->set_param(0.7f);
    map_generator->height2D->set_param(0.8f);
    map_generator->ridge2D->set_param(0.5f);
    map_generator->roughness2D->set_param(0.9f);

    ti[i++] = _GET_MS_TIME();

    map_generator->populate_cache();
    ti[i++] = _GET_MS_TIME();

    map_generator->generate_map(tile_id);
    ti[i++] = _GET_MS_TIME();

    map_generator->save_noisemaps();
    ti[i++] = _GET_MS_TIME();

    printf("Map Gen: \n");
    printf("0 init noise map: %i ms \n", ti[1]-ti[0]);
    printf("1 set persistence: %i ms \n", ti[2]-ti[1]);
    printf("3 populate cache: %i ms \n", ti[3]-ti[2]);
    printf("4 map volume lerp: %i ms \n", ti[4]-ti[3]);
    printf("5 save noisemaps: %i ms \n", ti[5]-ti[4]);
}

void init_map_generator()
{
    GS_ASSERT(map_generator == NULL);
    map_generator = new class MapGenerator1;
}

void teardown_map_generator()
{
    delete map_generator;
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

void LUA_set_noisemap_param(int noise_map, float persistence, unsigned char* seed_string)
{
    IF_ASSERT(map_generator == NULL) return;

    switch (noise_map)
    {
        case 0:
            map_generator->erosion3D->set_param(persistence);
            break;
        case 1:
            map_generator->erosion2D->set_param(persistence);
            break;
        case 2:
            map_generator->height2D->set_param(persistence);
            break;
        case 3:
            map_generator->ridge2D->set_param(persistence);
            break;
        case 4:
            map_generator->roughness2D->set_param(persistence);
            break;
        default:
            GS_ASSERT(false);
            printf("%s Error: noisemap %d does not exist \n", __FUNCTION__, noise_map);
            break;
    }
}


float* LUA_get_noisemap_map_cache(int noise_map)
{
    IF_ASSERT(map_generator == NULL) return NULL;

    switch (noise_map)
    {
        case 0:
            return map_generator->erosion3D->cache;
        case 1:
            return map_generator->erosion2D->cache;
        case 2:
            return map_generator->height2D->cache;
        case 3:
            return map_generator->ridge2D->cache;
        case 4:
            return map_generator->roughness2D->cache;
         default:
            GS_ASSERT(false);
            printf("%s Error: noisemap %d does not exist \n", __FUNCTION__, noise_map);
            break;
    }
    return NULL;
}

float* LUA_get_map_lerp_array()
{
    IF_ASSERT(map_generator == NULL) return NULL;
    return map_generator->cache;
}

void LUA_generate_map()
{
    #if !PRODUCTION
    CubeType tile = t_map::get_cube_type("regolith");
    map_generator->generate_map(tile);
    t_map::map_post_processing();
    #endif
}
}   // extern "C"

}   // t_gen

#ifdef __MSVC__
# pragma optimize("", off)
#endif
