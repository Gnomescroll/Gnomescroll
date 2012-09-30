#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

//#pragma GCC optimize ("O3")

#include <t_gen/twister.hpp>

#ifdef __MSVC__
    #pragma optimize( "gt", on )
#endif

namespace t_gen
{

int primes[20] = {
    2,3,5,7,11,
    13,17,19,23,29,
    31,37,41,43,47,
    53,59,61,67,71
};

#if __GNUC__
__attribute((always_inline, optimize("-O3"))) static float dot(float* g, float x, float y);
__attribute((always_inline, optimize("-O3"))) static float dot(int gi, float x, float y, float z);

__attribute((always_inline, optimize("-O3"))) static float mix(float a, float b, float t);
__attribute((always_inline, optimize("-O3"))) static float fade(float t);
#endif

int fast_floor(float value);

int fast_floor(float value) 
{
    return (int)( value>=0 ? (int)value : (int)value-1 );
}

static float dot(float* g, float x, float y)
{
    return g[0]*x + g[1]*y;
}

static float dot(int gi, float x, float y, float z)
{
    static const int g3[][3] = {
    {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} 
    };

    return g3[gi][0]*x + g3[gi][1]*y + g3[gi][2]*z;
}

static float mix(float a, float b, float t) 
{
    return a + t*(b-a);   //optimized version
}

static float fade(float t) 
{
    return t*t*t*(t*(t*6-15)+10);
}


class PerlinField2D
{
    public:

    unsigned char* ga;  //gradient array
    float* grad; //gradient vector array
    //static const int ssize = 64*64*32;
    //static const int xsize = 64;

    int ssize;  //number of gradients

    int xsize;

    //float xscale;   //scale multiplier

    int grad_max;   //number of gradients

    //xsize is number of gradients
    PerlinField2D() {}

    void init(int _xsize, int _grad_max)
    {
        if(_xsize < 1) GS_ABORT();

        xsize = _xsize;
        ssize = xsize*xsize;

        grad_max = _grad_max;

        ga = new unsigned char[ssize];
        grad = new float[2*grad_max];

        generate_gradient_array();
        generate_gradient_vectors();
    }

    ~PerlinField2D()
    {
        delete[] this->grad;
        delete[] this->ga;
    }

    __attribute((optimize("-O3")))
    void generate_gradient_array()
    {
        for(int i=0; i<this->ssize; i++) ga[i] = genrand_int32() % grad_max; //gradient number
    }

    void generate_gradient_vectors()
    {
    #if 1
        for(int i=0; i<grad_max; i++)
        {
            float t = 6.28318531f*i* (1.0f / ((float) grad_max));
            float x = sinf(t);
            float y = cosf(t);

            grad[2*i+0] = x;
            grad[2*i+1] = y;
        }
    #else
        for(int i=0; i<grad_max; i++)
        {
            float x = 2*genrand_real1() -1.0;
            float y = 2*genrand_real1() -1.0;

            float len = sqrt(x*x+y*y);
            x /= len;
            y /= len;

            grad[2*i+0] = x;
            grad[2*i+1] = y;
        }
    #endif
    }

// This method is a *lot* faster than using (int)Math.floor(x)
//__attribute((always_inline, optimize("-O3")))

//__attribute((always_inline, optimize("-O3")))
inline int get_gradient(int x, int y)
{
    x = x % xsize; //replace with bitmask
    y = y % xsize;

    return ga[x + y*xsize];
}

public:

// Classic Perlin noise, 3D version
//__attribute((optimize("-O3")))
float base(float x, float y) 
{
    x *= xsize;  //replace with multiplication
    y *= xsize;
    //get grid point
    int X = fast_floor(x);
    int Y = fast_floor(y);

    x = x - X;
    y = y - Y;

    int gi00 = get_gradient(X+0,Y+0);
    int gi01 = get_gradient(X+0,Y+1);
    int gi10 = get_gradient(X+1,Y+0);
    int gi11 = get_gradient(X+1,Y+1);
    
    // Calculate noise contributions from each of the eight corners
    float n00= dot(grad+2*gi00, x, y);
    float n10= dot(grad+2*gi10, x-1, y);
    float n01= dot(grad+2*gi01, x, y-1);
    float n11= dot(grad+2*gi11, x-1, y-1);
    // Compute the fade curve value for each of x, y, z
    
    #if 1
        float u = fade(x);
        float v = fade(y);
    #else
        float u = x;
        float v = y;
    #endif

    float nx00 = mix(n00, n10, u);
    float nx10 = mix(n01, n11, u);
    float nxy = mix(nx00, nx10, v);

    return nxy;   //-1 to 1
}

};

class PerlinOctave2D
{
    public:
    int octaves;
    class PerlinField2D* octave_array;

    float* cache;
    float cache_persistance;
    unsigned long cache_seed;

    PerlinOctave2D(int _octaves)
    {
        cache = NULL;
        cache_persistance = 0.0f;
        cache_seed = 0;

        octaves = _octaves;


        init_genrand(randf());
        octave_array = new PerlinField2D[octaves];

        cache = new float[(512/4)*(512/4)];
        //for(int i=0; i<octaves; i++) octave_array[i].init(pow(2,i+2), 15);
        //for(int i=0; i<octaves; i++) octave_array[i].init(2*(i+1)+1, 4);
        //for(int i=0; i<octaves; i++) octave_array[i].init((i*(i+1))+1, 4);
    
        for(int i=0; i<octaves; i++) octave_array[i].init(primes[i+1], 16);

        for(int i=0; i<octaves; i++)
            octave_array[i].init(primes[i+1], 16);

    }

    ~PerlinOctave2D()
    {
        delete[] octave_array;
        delete[] cache;
    }

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

    __attribute((optimize("-O3")))
    void populate_cache(float persistance)
    {
        const int XMAX = 512/4;
        const int YMAX = 512/4;

        float x,y;

        for(int i=0; i<XMAX; i++)
        for(int j=0; j<YMAX; j++)
        {
            x = i*(4.0f/512.0f);
            y = j*(4.0f/512.0f);

            cache[j*XMAX + i] = sample(x,y, persistance);
        }
    }

    __attribute((always_inline, optimize("-O3")))
    float sample(float x, float y, float persistance)
    {   
        float p = 1.0f;
        float tmp = 0.0f;
        for(int i=0; i<octaves; i++)
        {
        #if 1
            tmp += octave_array[i].base(x,y);
            p *= persistance;
        #else
            p *= persistance;
            tmp += p*octave_array[i].base(x,y,z);
        #endif
        }
        return tmp;
    }

//    __attribute((always_inline, optimize("-O3")))
    float sample2(float x, float y, float persistance)
    {   
        float p = 1.0f;
        float tmp = 0.0f;
        for(int i=0; i<octaves; i++)
        {
            printf("octave %i: %f \n", i, octave_array[i].base(x,y));
            p *= persistance;
            tmp += p*octave_array[i].base(x,y);
        }
        printf("tmp= %f x,y= %f %f \n", tmp, x,y);
        return tmp;
    }

    __attribute((optimize("-O3")))
    void save_octaves()
    {

        const int xres = 512;
        const int yres = 512;

        const float xresf = 1.0f / ((float) xres);
        const float yresf = 1.0f / ((float) yres);

        float* out = new float[xres*yres*octaves];

        class PerlinField2D* m;
        for(int k=0; k<octaves; k++)
        {
            m = &octave_array[k];
            int zoff = k*xres*yres;
            for(int i=0; i<xres; i++)
            for(int j=0; j<yres; j++)
            {
                float x = i*xresf;
                float y = j*yresf;

                out[i+j*yres+zoff] = m->base(x,y);
            }
        }

        save_perlin("octave_map_01", out, xres, yres*octaves);

        for(int k=0; k<octaves; k++)
        for(int i=0; i<xres; i++)
        for(int j=0; j<yres; j++)
        {
            out[i+j*yres+k*xres*yres] = 0.0f;
        }

        for(int k=0; k<octaves; k++)
        {

            for(int k2=0; k2< k; k2++)
            {
                m = &octave_array[k2];
                float p = 1.0f;
                int zoff = k*xres*yres;

                for(int i=0; i<xres; i++)
                for(int j=0; j<yres; j++)
                {
                    float x = i*xresf;
                    float y = j*yresf;

                    out[i+j*yres+zoff] += p*m->base(x,y);
                }

                p *= 0.50f;
            }
        }


        //save_png("octave_map_01", out, xres, yres*octaves);
        save_perlin("octave_map_02", out, xres, yres*octaves);
        //void save_png(const char* filename, float* in, int xres, int yres)
    }

    __attribute((optimize("-O3")))
    void save_octaves2(int DEGREE, const char* filename)
    {

        const int xres = 128;
        const int yres = 128;

        const float xresf = 1.0f / ((float) xres);
        const float yresf = 1.0f / ((float) yres);

        //(i + xres*DEGREE*n)+ (j*xres*yres*DEGREE)
        float* out = new float[xres*yres*octaves*DEGREE];

        int line_width = xres*DEGREE;

        for(int i=0; i<xres*yres*octaves*DEGREE; i++) out[i] = 0.0f;

        class PerlinField2D* m;


        for(int k=0; k<octaves; k++)
        {
            m = &octave_array[k];
            int zoff = k*yres*line_width;

            for(int i=0; i<xres; i++)
            for(int j=0; j<yres; j++)
            {
                float x = i*xresf;
                float y = j*yresf;

                out[i+j*line_width+zoff] += m->base(x,y);
            }
        }

        for(int n=1; n<DEGREE; n++)
        {
            float PERSISTANCE = (n+1)*(1.0f/( (float) DEGREE ));

            int xoff = n*xres;

            for(int k=0; k<octaves; k++)
            {
                int yoff = k*yres*line_width;
                float p = 1.0f;

                for(int k2=0; k2 <= k; k2++)
                {
                    m = &octave_array[k2];

                    for(int i=0; i<xres; i++)
                    for(int j=0; j<yres; j++)
                    {
                        float x = i*xresf;
                        float y = j*yresf;

                        out[ (i+xoff)+ (j*line_width+yoff) ] += p*m->base(x,y);
                    }

                    p *= PERSISTANCE;
                }
            }

        }
        //save_png("octave_map_01", out, xres, yres*octaves);
        save_perlin(filename, out, xres*DEGREE, yres*octaves);
        //void save_png(const char* filename, float* in, int xres, int yres)
        delete[] out;
    }

};


void test_octave_2d()
{
    return;
    PerlinOctave2D m(6);

    //m.save_octaves2(8, "test");
}

void test_octave_2d_map_gen(int tile)
{
    //return;

    PerlinOctave2D m1(6);
    PerlinOctave2D m2(6);

    const int xres = 512;
    const int yres = 512;

    const float xresf = 1.0f / ((float) xres);
    const float yresf = 1.0f / ((float) yres);

    float* values = (float*)malloc(512*512*sizeof(float));

    float min_value_generated = 100000;
    float max_value_generated = -100000;
    
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {

        float x = i*xresf;
        float y = j*yresf;

        //value += 32*m.sample(x,y, 0.75);
        float roughness = m2.sample(x,y, 0.125f);
        //if(roughness < 0) roughness = 0.0;
        if(roughness < 0) roughness *= -1.0f/32.0f;

        float value = 32 + 32.0f*roughness*m1.sample(x,y, 0.125f);
        if (value < min_value_generated) min_value_generated = value;
        if (value > max_value_generated) max_value_generated = value;
        values[i+512*j] = value;
        
        //value += 32*oct_0.sample(x, y, 32.5, PERSISTANCE);

        //for (int k=0; k<value; k++) t_map::set(i,j,k,tile);


        //out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    }

    GS_ASSERT(min_value_generated < 0);
    GS_ASSERT(max_value_generated > 0);

    float min_value_found = 10000.0f;

    const int max_depth = -24;
    const int max_height = 64;

    float min_scale = 1.0f;
    if (min_value_generated < max_depth) min_scale = max_depth / min_value_generated;
    GS_ASSERT(min_scale >= 0.0f);
    float max_scale = 1.0f;
    if (max_value_generated > max_height) max_scale = max_height / max_value_generated;
    GS_ASSERT(max_scale >= 0.0f);
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        float v = values[i+512*j];
        if (v > 0) v *= max_scale;
        if (v < 0) v *= min_scale;
        if (v < min_value_found) min_value_found = v;
        values[i+512*j] = v;
    }

    GS_ASSERT(min_value_found >= max_depth);

    const int floor_height = 32;
    int baseline = floor_height - min_value_found;

    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        for (int k=0; k<baseline+values[i+512*j]; k++)
            t_map::set(i,j,k,tile);
    }

    free(values);
}

}   // t_gen

#ifdef __MSVC__
    #pragma optimize( "", off )
#endif
