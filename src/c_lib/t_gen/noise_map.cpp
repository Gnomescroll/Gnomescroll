#include "noise_map.hpp"

#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <t_gen/noise_map2.hpp>

namespace t_gen
{

static const int _grad3[][3] = {
{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} 
};

class OctaveMap3D
{
    public:

    int xs; // = 8; //x sample density
    int zs; // = 4; //y sample density
    float xsf;
    float zsf;

    int xsize; // = 64;
    int zsize; // = 32;

    unsigned char* grad_array;

    OctaveMap3D()
    {
        grad_array = NULL;
    }


    void init(int xsample_density, int zsample_density)
    {
        xs = xsample_density;
        zs = zsample_density;

        xsf = 1.0 / ((float) xs);
        zsf = 1.0 / ((float) zs);

        xsize = 512 / xs;
        zsize = 512 / zs;

        if(!grad_array) grad_array = new unsigned char [xsize*xsize*zsize];
        populate_grad_array();
    }

    ~OctaveMap3D()
    {
        delete[] grad_array;
    }

    void populate_grad_array()
    {
        for(int i=0; i<xsize*xsize*zsize; i++) 
            grad_array[i] = genrand_int32() % 12; //gradient number
    }

    static inline int fastfloor(float x) 
    {
        #if 1
            return x>=0 ? (int)x : (int)x-1;
        #else
            return (int) x;
        #endif
    }

    static inline float dot(const int g[], float x, float y, float z)
    {
        return g[0]*x + g[1]*y + g[2]*z;
    }

    static inline float mix(float a, float b, float t) 
    {
        return a + t*(b-a);   //optimized version
    }

    static inline float fade(float t) 
    {
        return t*t*t*(t*(t*6-15)+10);
    }

    inline int get_gradient(int x, int y, int z)
    {
        x = x % xsize; //replace with bitmask
        y = y % xsize;
        z = z % zsize;

        return grad_array[x + y*xsize + z*xsize*xsize];
    }

    float sample(float x, float y, float z)
    {
        x *= xsf;  //replace with multiplication
        y *= xsf;
        z *= zsf;
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
        float n000= dot(_grad3[gi000], x, y, z);
        float n100= dot(_grad3[gi100], x-1, y, z);
        float n010= dot(_grad3[gi010], x, y-1, z);
        float n110= dot(_grad3[gi110], x-1, y-1, z);
        float n001= dot(_grad3[gi001], x, y, z-1);
        float n101= dot(_grad3[gi101], x-1, y, z-1);
        float n011= dot(_grad3[gi011], x, y-1, z-1);
        float n111= dot(_grad3[gi111], x-1, y-1, z-1);
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
    private:
    
    //individual octave map temps
    int _xsize, _xsize2;  //max for modulo roll over
    int _zsize;

    float _xsf; //for lerp
    float _zsf;

    //globals

    int xbase;
    int xbase2;
    int zbase;

    int xsize, xsize2;
    int zsize;
    
    int ssize;  //value array size
    unsigned char* gradient_array;

    public:

    int octaves;
    class OctaveMap3D* octave_array;

    float* value_array;

    //base sample rates
    PerlinOctave3D(int _octaves, int base_x, int base_z)
    {
        init_genrand(randf());
        octaves = _octaves;
        xbase = base_x;
        zbase = base_z;
        ssize = xbase*xbase*zbase;

        xsize = 512 / xbase;
        zsize = 512 / zbase;
        xsize2 = xsize*xsize;

        _xsf = 1.0 / ((float) xbase);
        _zsf = 1.0 / ((float) zbase);

        octave_array = new OctaveMap3D[octaves];


        for(int i=0; i<octaves; i++)
        {
            octave_array[i].init(base_x, base_z);

            base_x *= 2;
            base_z *= 2;
        }

        if(base_x >= 512 || base_z >= 128)
        {
            printf("PerlinField3D warning: base octaves too large for map size \n");
            printf("octaves= %i, base_x= %i, base_y= %i \n", octaves, base_x, base_z);
        } else
        {
            if(base_x >= 256 || base_z >= 64)
            {
                printf("PerlinField3D warning: base octaves too large for map size \n");
                printf("octaves= %i, base_x= %i, base_y= %i \n", octaves, base_x, base_z);
            }
            abort();
        }

        value_array = new float[xsize*xsize*zsize];
    }

    ~PerlinOctave3D()
    {
        delete[] value_array;
        delete[] octave_array;
    }

    float sample(float x, float y, float z)
    {
        float tmp = 0.0;
        for(int i=0; i<octaves; i++)
        {
            tmp += octave_array[i].sample(x,y,z);
        }
        return tmp;
    }


    float sample(float x, float y, float z, float persistance)
    {

        float p = 1.0;
        float tmp = 0.0;
        for(int i=octaves-1; i>=0; i--)
        {
            tmp += p*octave_array[i].sample(x,y,z);
            p *= persistance;
        }
        return tmp;
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
    //return x>=0 ? (int)x : (int)x-1;
    return (int) x;
}

static inline float dot(const int g[], float x, float y, float z)
{
#if 1
    return g[0]*x + g[1]*y + g[2]*z;
#else
    float tmp =  g[0]*x + g[1]*y + g[2]*z;
    printf("tmp= %f x,y,z= %f %f %f g1,g2,g3= %i %i %i \n", tmp, x,y,z, g[0],g[1],g[2] );
    return tmp;
#endif
}

static inline float mix(float a, float b, float t) 
{
    return a + t*(b-a);
}

static inline float fade(float t) 
{
    return t*t*t*(t*(t*6-15)+10);
}

inline int get_gradient(int x, int y, int z)
{
    x = x % _xsize; //replace with bitmask
    y = y % _xsize;
    z = z % _zsize;

    return gradient_array[x + y*_xsize + z*_xsize2];
}

void populate_value_array()
{

    for(int i=0; i<ssize; i++) value_array[i] = 0;

    for(int octave=0; octave < octaves; octave++)
    {
        sum_octave(octave);
    }

}

/*
    int xs; // = 8; //x sample density
    int zs; // = 4; //y sample density
    float xsf;
    float zsf;

    int xsize; // = 64;
    int zsize; // = 32;
*/
void sum_octave(int octave)
{
    //xsize = octave_array[octave].xsize;
    //xsize2 = xsize*xsize;
    //zsize = octave_array[octave].zsize;

    _xsf = octave_array[octave].xsf;
    _zsf = octave_array[octave].zsf;

    _xsize = octave_array[octave].xsize;
    _xsize2 = _xsize*_xsize;
    _zsize = octave_array[octave].zsize;

    gradient_array = octave_array[octave].grad_array;

    for(int k=0; k< zsize; k++)
    for(int j=0; j< xsize; j++)
    for(int i=0; i< xsize; i++)
    {
    /*
        float x = i*xbase + 2.0;
        float y = j*xbase + 2.0;
        float z = k*zbase + 2.0;
    */
        float x = i*xbase;
        float y = j*xbase;
        float z = k*zbase;

        value_array[i + xsize*j + xsize2*k] += base(x,y,z);

        if(k==2) return;
    }

}

float base(float x, float y, float z)
{
        x *= _xsf;
        y *= _xsf;
        z *= _zsf;
        //get grid point
        int X = (int) x;    //fast floor for positive ints
        int Y = (int) y;
        int Z = (int) z;

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

        float n000= dot(_grad3[gi000], x, y, z);
        float n100= dot(_grad3[gi100], x-1, y, z);
        float n010= dot(_grad3[gi010], x, y-1, z);
        float n110= dot(_grad3[gi110], x-1, y-1, z);
        float n001= dot(_grad3[gi001], x, y, z-1);
        float n101= dot(_grad3[gi101], x-1, y, z-1);
        float n011= dot(_grad3[gi011], x, y-1, z-1);
        float n111= dot(_grad3[gi111], x-1, y-1, z-1);
        // Compute the fade curve value for each of x, y, z
    
        //printf("gi000= %i n000= %f \n", gi000, n000);
        //printf("x= %f y= %f z= %f \n", x,y,z);

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

float sample2(float x, float y, float z)
{
    int i = (int)(x /_xsf ); //for lerp
    int j = (int)(y /_xsf );
    int k = (int)(z /_zsf );


    return value_array[i + xsize*j + xsize2*k];
}

};

const float PERSISTANCE = 0.40;

void noise_map_test()
{

    //test_octave_2d();

#if 0
    printf("df\n");

    PerlinOctave3D oct_0(6,4,8); //int _octaves, int base_x, int base_z

    oct_0.populate_value_array();

    const int xres = 512;
    const int yres = 512;

    float* out = new float[xres*yres];

    for(int i=0; i<xres; i++)
    for(int j=0; j<yres; j++)
    {
        float x = i;
        float y = j;

        float n = oct_0.sample(x,y, 32.0, PERSISTANCE);
        //printf("n= %f \n", n);
        out[i+j*yres] = n;
    }

    save_png("n_map_01", out, xres, yres);

    delete[] out;
#endif
}

void noise_map_generate_map()
{
#if DC_SERVER
    int tile = t_map::dat_get_cube_id("regolith");
    test_octave_2d_map_gen(tile);
#endif
}


}
