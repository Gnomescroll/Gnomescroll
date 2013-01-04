#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

//#pragma GCC optimize ("O3")

#include <t_gen/twister.hpp>

#ifdef __MSVC__
# pragma optimize("gt", on)
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
OPTIMIZED static inline float dot(float* g, float x, float y);
OPTIMIZED static inline float dot(int gi, float x, float y, float z);

OPTIMIZED static inline float mix(float a, float b, float t);
OPTIMIZED static inline float fade(float t);
#endif

int inline fast_floor(float value);

int inline fast_floor(float value)
{
    return (int)(value>=0 ? (int)value : (int)value-1);
}

/*
static inline float dot(float* g, float x, float y)
{
    return g[0]*x + g[1]*y;
}
*/

static inline float dot(int gi, float x, float y, float z)
{
    static const int g3[12][3] = {
    {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}
    };

    return g3[gi][0]*x + g3[gi][1]*y + g3[gi][2]*z;
}

static inline float mix(float a, float b, float t)
{
    return a + t*(b-a);   //optimized version
}

static inline float fade(float t)
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
    PerlinField2D() :
        ga(NULL), grad(NULL), ssize(0), xsize(0), grad_max(0)
    {}

    void init(int _xsize, int _grad_max)
    {
        IF_ASSERT(this->ga != NULL) return;
        IF_ASSERT(this->grad != NULL) return;

        if (_xsize < 1) GS_ABORT();

        this->xsize = _xsize;
        this->ssize = xsize*xsize;
        this->grad_max = _grad_max;

        this->ga = new unsigned char[this->ssize];
        this->grad = new float[2*this->grad_max];

        generate_gradient_array();
        generate_gradient_vectors();
    }

    ~PerlinField2D()
    {
        if (this->grad != NULL) delete[] this->grad;
        if (this->ga != NULL) delete[] this->ga;
    }

    OPTIMIZED
    void generate_gradient_array()
    {
        for (int i=0; i<this->ssize; i++) ga[i] = genrand_int32() % grad_max; //gradient number
    }

    void generate_gradient_vectors()
    {
        #if 1
        for (int i=0; i<grad_max; i++)
        {
            float t = 6.28318531f*i* (1.0f / ((float) grad_max));
            float x = sinf(t);
            float y = cosf(t);

            grad[2*i+0] = x;
            grad[2*i+1] = y;
        }
        #else
        for (int i=0; i<grad_max; i++)
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
    //OPTIMIZED

    //OPTIMIZED
    inline int get_gradient(int x, int y)
    {
        x = x % xsize; //replace with bitmask
        y = y % xsize;

        return ga[x + y*xsize];
    }

    inline float dot_grad(int index, float x, float y)
    {
        return grad[2*index+0]*x + grad[2*index+1]*y;
    }
    
    // Classic Perlin noise, 3D version
    //OPTIMIZED
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
        /*
            float n00= dot(grad+2*gi00, x, y);
            float n10= dot(grad+2*gi10, x-1, y);
            float n01= dot(grad+2*gi01, x, y-1);
            float n11= dot(grad+2*gi11, x-1, y-1);
        */

        float n00 = dot_grad(gi00, x, y);
        float n10 = dot_grad(gi10, x-1, y);
        float n01 = dot_grad(gi01, x, y-1);
        float n11 = dot_grad(gi11, x-1, y-1);

        // Compute the fade curve value for each of x, y, z

        float u = fade(x);
        float v = fade(y);

        float nx00 = mix(n00, n10, u);
        float nx10 = mix(n01, n11, u);
        float nxy  = mix(nx00, nx10, v);

        return nxy;   //-1 to 1
    }

};

class PerlinOctave2D
{
    public:
        int octaves;
        class PerlinField2D* octave_array;

        float* cache;
        float cache_persistence;
        unsigned long cache_seed;

    explicit PerlinOctave2D(int _octaves) : runs(0)
    {
        cache = NULL;
        cache_persistence = 0.0f;
        cache_seed = 0;

        octaves = _octaves;

        init_genrand(rand());
        this->octave_array = new PerlinField2D[octaves];

        this->cache = new float[(512/4)*(512/4)];

        for (int i=0; i<octaves; i++)
            this->octave_array[i].init(primes[i+1], 16);
    }

    ~PerlinOctave2D()
    {
        if (this->octave_array != NULL) delete[] this->octave_array;
        if (this->cache != NULL) delete[] this->cache;
    }

    void set_persistence(float persistence)
    {
        if (cache_persistence == persistence) return;
        cache_persistence = persistence;
        populate_cache(persistence);
    }

    void set_param(int persistence)
    {
        bool update = false;
        if (!this->runs)
        {
            update = true;
            cache_seed = rand();
            init_genrand(this->cache_seed);
            for (int i=0; i<octaves; i++)
                octave_array[i].generate_gradient_array();
        }

        if (persistence != cache_persistence || update)
        {
            cache_persistence = persistence;
            populate_cache(persistence);
        }

        this->runs++;
    }

    OPTIMIZED
    void populate_cache(float persistence)
    {
        const int xmax = 512/4;
        const int ymax = 512/4;

        float x,y;

        for (int i=0; i<xmax; i++)
        for (int j=0; j<ymax; j++)
        {
            x = i*(4.0f/(float)XMAX);
            y = j*(4.0f/(float)YMAX);

            cache[j*xmax + i] = sample(x,y, persistence);
        }
    }

    //OPTIMIZED
    inline float sample(float x, float y, float persistence)
    {
        float p = 1.0f;
        float tmp = 0.0f;
        for (int i=0; i<octaves; i++)
        {
            #if 1
            tmp += octave_array[i].base(x,y);
            p *= persistence;
            #else
            p *= persistence;
            tmp += p*octave_array[i].base(x,y,z);
            #endif
        }
        return tmp;
    }

    //OPTIMIZED
    float sample2(float x, float y, float persistence)
    {
        float p = 1.0f;
        float tmp = 0.0f;
        for (int i=0; i<octaves; i++)
        {
            printf("octave %i: %f \n", i, octave_array[i].base(x,y));
            p *= persistence;
            tmp += p*octave_array[i].base(x,y);
        }
        printf("tmp= %f x,y= %f %f \n", tmp, x,y);
        return tmp;
    }

    OPTIMIZED
    void save_octaves()
    {

        const int xres = 512;
        const int yres = 512;

        const float xresf = 1.0f / ((float) xres);
        const float yresf = 1.0f / ((float) yres);

        float* out = new float[xres*yres*octaves];

        class PerlinField2D* m;
        for (int k=0; k<octaves; k++)
        {
            m = &octave_array[k];
            int zoff = k*xres*yres;
            for (int i=0; i<xres; i++)
            for (int j=0; j<yres; j++)
            {
                float x = i*xresf;
                float y = j*yresf;

                out[i+j*yres+zoff] = m->base(x,y);
            }
        }

        save_perlin("octave_map_01", out, xres, yres*octaves);

        for (int k=0; k<octaves; k++)
        for (int i=0; i<xres; i++)
        for (int j=0; j<yres; j++)
        {
            out[i+j*yres+k*xres*yres] = 0.0f;
        }

        for (int k=0; k<octaves; k++)
        {

            for (int k2=0; k2< k; k2++)
            {
                m = &octave_array[k2];
                float p = 1.0f;
                int zoff = k*xres*yres;

                for (int i=0; i<xres; i++)
                for (int j=0; j<yres; j++)
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

    OPTIMIZED
    void save_octaves2(int degree, const char* filename)
    {

        const int xres = 128;
        const int yres = 128;

        const float xresf = 1.0f / ((float) xres);
        const float yresf = 1.0f / ((float) yres);

        //(i + xres*degree*n)+ (j*xres*yres*degree)
        float* out = new float[xres*yres*octaves*degree];

        int line_width = xres*degree;

        for (int i=0; i<xres*yres*octaves*degree; i++) out[i] = 0.0f;

        class PerlinField2D* m;


        for (int k=0; k<octaves; k++)
        {
            m = &octave_array[k];
            int zoff = k*yres*line_width;

            for (int i=0; i<xres; i++)
            for (int j=0; j<yres; j++)
            {
                float x = i*xresf;
                float y = j*yresf;

                out[i+j*line_width+zoff] += m->base(x,y);
            }
        }

        for (int n=1; n<degree; n++)
        {
            float persistence = (n+1)*(1.0f/( (float) degree ));

            int xoff = n*xres;

            for (int k=0; k<octaves; k++)
            {
                int yoff = k*yres*line_width;
                float p = 1.0f;

                for (int k2=0; k2 <= k; k2++)
                {
                    m = &octave_array[k2];

                    for (int i=0; i<xres; i++)
                    for (int j=0; j<yres; j++)
                    {
                        float x = i*xresf;
                        float y = j*yresf;

                        out[ (i+xoff)+ (j*line_width+yoff) ] += p*m->base(x,y);
                    }

                    p *= persistence;
                }
            }

        }
        //save_png("octave_map_01", out, xres, yres*octaves);
        save_perlin(filename, out, xres*degree, yres*octaves);
        //void save_png(const char* filename, float* in, int xres, int yres)
        delete[] out;
    }

    private:
        int runs;
};


void test_octave_2d()
{
    //m.save_octaves2(8, "test");
}

void test_octave_2d_map_gen(CubeType tile)
{
    class PerlinOctave2D* m1 = new class PerlinOctave2D(6);
    class PerlinOctave2D* m2 = new class PerlinOctave2D(6);

    const int xres = 512;
    const int yres = 512;

    const float xresf = 1.0f / ((float) xres);
    const float yresf = 1.0f / ((float) yres);

    float* values = (float*)malloc(512*512*sizeof(float));

    float min_value_generated = 100000;
    float max_value_generated = -100000;

    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
    {

        float x = i*xresf;
        float y = j*yresf;

        //value += 32*m.sample(x,y, 0.75);
        float roughness = m2->sample(x,y, 0.125f);
        //if (roughness < 0) roughness = 0.0;
        if (roughness < 0) roughness *= -1.0f/32.0f;

        float value = 32 + 32.0f*roughness*m1->sample(x,y, 0.125f);
        if (value < min_value_generated) min_value_generated = value;
        if (value > max_value_generated) max_value_generated = value;
        values[i+512*j] = value;

        //value += 32*oct_0.sample(x, y, 32.5, persistence);

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
    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
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

    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
    {
        for (int k=0; k<baseline+values[i+512*j]; k++)
            t_map::set_fast(i,j,k,tile);
    }

    free(values);
    delete m1;
    delete m2;
}

}   // t_gen

#ifdef __MSVC__
# pragma optimize("", off)
#endif
