#include "noise_map.hpp"


#include <t_gen/twister.hpp>

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

namespace t_gen
{

static const int _grad3[][3] = {
{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} 
};

class PerlinField3D
{
    public:

    unsigned char* gradient_array;

    static const int xs = 8; //x sample density
    static const int zs = 4; //y sample density

    static const int ssize = 64*64*32;
    static const int xsize = 64;
    static const int zsize = 32;

    PerlinField3D(int seed)
    {
        init_genrand(seed);

        gradient_array = new unsigned char[ssize];
        for(int i=0; i<ssize; i++)
        {
            gradient_array[i] = genrand_int32() % 12; //gradient number
        }
    }

    ~PerlinField3D()
    {
        delete[] this->gradient_array;
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
return x>=0 ? (int)x : (int)x-1;
}

static inline float dot(const int g[], float x, float y, float z)
{
return g[0]*x + g[1]*y + g[2]*z;
}

static inline float mix(float a, float b, float t) 
{
//    return (1-t)*a + t*b;
    return a + t*(b-a);   //optimized version
}

static inline float fade(float t) 
{
return t*t*t*(t*(t*6-15)+10);
}

inline int get_gradient(int x, int y, int z)
{
    x = x % 64; //replace with bitmask
    y = y % 64;
    z = z % 32;

    if(x + y*64 + z*64*64 >= ssize) GS_ABORT();

    return gradient_array[x + y*64 + z*64*64];
}

public:

// Classic Perlin noise, 3D version
float base(float x, float y, float z) 
{
/*
    // Find unit grid cell containing point
    int X = fastfloor(x);
    int Y = fastfloor(y);
    int Z = fastfloor(z);
    // Get relative xyz coordinates of point within that cell
    x = x - X;
    y = y - Y;
    z = z - Z;
    // Wrap the integer cells at 255 (smaller integer period can be introduced here)
    X = X & 255;
    Y = Y & 255;
    Z = Z & 255;
*/
    x /= 8.0;  //replace with multiplication
    y /= 8.0;
    z /= 4.0;
    //get grid point
    int X = fastfloor(x);
    int Y = fastfloor(y);
    int Z = fastfloor(z);

    //get interpolation ratio

    //if(z >= 32 || z < 0) printf("ERROR1 z = %f \n", z);

    x = x - X;
    y = y - Y;
    z = z - Z;

    //if(z >= 32 || z < 0) printf("ERROR2 z = %f \n", z);
    //if(x<0 || y<0 || z<0) printf("x,y,z= %f %f %f \n", x,y,z);
    //if(z >= 32) printf("x,y,z= %f %f %f X,Y,Z= %i %i %i \n", x+X,y+Y,z+Z,X,Y,Z);
    // Calculate a set of eight hashed gradient indices


    int gi000 = get_gradient(X+0,Y+0,Z+0);
    int gi001 = get_gradient(X+0,Y+0,Z+1);
    int gi010 = get_gradient(X+0,Y+1,Z+0);
    int gi011 = get_gradient(X+0,Y+1,Z+1);

    int gi100 = get_gradient(X+1,Y+0,Z+0);
    int gi101 = get_gradient(X+1,Y+0,Z+1);
    int gi110 = get_gradient(X+1,Y+1,Z+0);
    int gi111 = get_gradient(X+1,Y+1,Z+1);

/*
    int gi000 = perm[X+perm[Y+perm[Z]]] % 12;
    int gi001 = perm[X+perm[Y+perm[Z+1]]] % 12;
    int gi010 = perm[X+perm[Y+1+perm[Z]]] % 12;
    int gi011 = perm[X+perm[Y+1+perm[Z+1]]] % 12;

    int gi100 = perm[X+1+perm[Y+perm[Z]]] % 12;
    int gi101 = perm[X+1+perm[Y+perm[Z+1]]] % 12;
    int gi110 = perm[X+1+perm[Y+1+perm[Z]]] % 12;
    int gi111 = perm[X+1+perm[Y+1+perm[Z+1]]] % 12;
*/
    // The gradients of each corner are now:
    // g000 = grad3[gi000];
    // g001 = grad3[gi001];
    // g010 = grad3[gi010];
    // g011 = grad3[gi011];
    // g100 = grad3[gi100];
    // g101 = grad3[gi101];
    // g110 = grad3[gi110];
    // g111 = grad3[gi111];
    
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

float noise(float x, float y, float z)
{
    return base(x,y,z);
}

float one_over_f(float x, float y, float z) 
{   
    float tmp = 0;
    tmp += base(x,y,z);
    tmp += 0.50 * base(2*x, 2*y,2*z);
    tmp += 0.25 * base(4*x,4*y,2*z);
    return tmp;
}


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
            grad_array[i] = genrand_int32() & 0xff; //gradient number
    }
};


class PerlinOctave3D
{
    private:
    
    //individual octave map temps
    int xsize;  //max for modulo roll over
    int xsize2;
    int zsize;


    //globals
    int xbase;
    int xbase2;
    int zbase;

    float _xsf; //for lerp
    float _ysf;

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
        xbase2 = xbase*xbase;
        zbase = base_z;
        ssize = xbase*xbase*zbase;

        _xsf = 1.0 / ((float) xbase);
        _ysf = 1.0 / ((float) base);

        octave_array = new OctaveMap3D[octaves];


        for(int i=0; i<octaves; i++)
        {
            octave_array[i].init(base_x, base_z);

            base_x *= 2;
            base_z *= 2;
        }

        if(base_x >= 256 || base_z >= 64)
        {
            printf("PerlinField3D warning: base octaves too large for map size \n");
            printf("octaves= %i, base_x= %i, base_y= %i \n", octaves, base_x, base_z);
            abort();
        }

        value_array = new float[base_x*base_x*base_z];
    }

    ~PerlinOctave3D()
    {
        delete[] value_array;
        delete[] octave_array;
    }


// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
    //return x>=0 ? (int)x : (int)x-1;
    return (int) x;
}

static inline float dot(const int g[], float x, float y, float z)
{
return g[0]*x + g[1]*y + g[2]*z;
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
    x = x % xsize; //replace with bitmask
    y = y % xsize;
    z = z % zsize;

    return gradient_array[x + y*xsize + z*xsize2];
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
    xsize = octave_array[octave].xsize;
    xsize2 = xsize*xsize;
    zsize = octave_array[octave].zsize;

    float xsf = octave_array[octave].xsf;
    float zsf = octave_array[octave].zsf;

    xsize = octave_array[octave].xs;
    xsize2 * xsize*xsize;
    zsize = octave_array[octave].zs;

    gradient_array = octave_array[octave].grad_array;

    for(int k=0; k< 128; k += zbase)
    for(int j=0; j< 512; j += xbase)
    for(int i=0; i< 512; i += xbase)
    {
        float x = i;
        float y = j;
        float z = k;

        x *= xsf;
        y *= xsf;
        z *= zsf;
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

        value_array[i + xbase*j + xbase2*k] += nxyz * 0.707106781;   //-1 to 1
    }

}

float sample(float x, float y, float z)
{


}

};


class PerlinField2D
{
    public:

    unsigned char* ga;  //gradient array
    float* grad; //gradient vector array
    //static const int ssize = 64*64*32;
    //static const int xsize = 64;

    int ssize;
    int xsize;
    int xs;

    float xscale;   //scale multiplier

    int grad_max;   //number of gradients


    PerlinField2D(int seed, int _xs, int _grad_max)
    {
        xs = _xs;
        xsize = 512 / xs;
        ssize = xsize*xsize;
        grad_max = _grad_max;
        xscale = 1.0 / ((float) xs);

        //init_genrand(seed);
        init_genrand(rand());

        ga = new unsigned char[ssize];
        for(int i=0; i<ssize; i++)
        {
            ga[i] = genrand_int32() % grad_max; //gradient number
        }
        grad = new float[2*grad_max];
        generate_gradient_vectors();
    }

    ~PerlinField2D()
    {
        delete[] this->grad;
    }

    void generate_gradient_vectors()
    {
        for(int i=0; i<grad_max; i++)
        {
            float x = 2*genrand_real1() -1;
            float y = 2*genrand_real1() -1;

            float len = sqrt(x*x+y*y);
            x /= len;
            y /= len;

            grad[2*i+0] = x;
            grad[2*i+1] = y;
        }
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
return x>=0 ? (int)x : (int)x-1;
}

static inline float dot(float* g, float x, float y)
{
return g[0]*x + g[1]*y;
}

static inline float mix(float a, float b, float t) 
{
//    return (1-t)*a + t*b;
    return a + t*(b-a);   //optimized version
}

static inline float fade(float t) 
{
return t*t*t*(t*(t*6-15)+10);
}

inline int get_gradient(int x, int y)
{
    x = x % xsize; //replace with bitmask
    y = y % xsize;

    if(x + y*xsize >= ssize) GS_ABORT();

    return ga[x + y*xsize];
}

public:

// Classic Perlin noise, 3D version
float base(float x, float y) 
{

    x *= xscale;  //replace with multiplication
    y *= xscale;
    //get grid point
    int X = fastfloor(x);
    int Y = fastfloor(y);

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

    // Interpolate along x the contributions from each of the corners
    float nx00 = mix(n00, n10, u);
    float nx10 = mix(n01, n11, u);
    // Interpolate the four results along y
    float nxy = mix(nx00, nx10, v);

    if(nxy < -1 || nxy > 1 ) printf("Error: noise %f \n", nxy);
    return nxy;   //-1 to 1
}

float noise(float x, float y)
{
    return base(x,y);
}

float one_over_f(float x, float y) 
{   
    float tmp = 0;
    tmp += base(x,y);
    tmp += 0.50 * base(2*x, 2*y);
    tmp += 0.25 * base(4*x,4*y);
    tmp += 0.125 * base(8*x,8*y);
    tmp += 0.0625 * base(16*x,16*y);
    return tmp;
}

float one_over_f(float x, float y, float persistance) 
{   
    float tmp = 0;
    float m = 1.0;

    tmp += base(x,y);
    m *= persistance;

    tmp += m * base(2*x, 2*y);
    m *= persistance;

    tmp += m * base(4*x,4*y);
    m *= persistance;

    tmp += m * base(8*x,8*y);
    
    m *= persistance;
    tmp += m * base(16*x,16*y);
    return tmp;
}

//order 0 is base
float order(float x, float y, float persistance, int order) 
{   
    float tmp = 0;
    float m = 1.0;
    int b = 1;

    for(int i=0; i<=order; i++)
    {
        tmp += base(b*x,b*y);
        m *= persistance;
        b *= 2;
    }
    return tmp;
}

float abs(float x, float y)
{
    float tmp = base(x,y);
    return sqrt(tmp*tmp);
}

};

float MIX(float a, float x, float y)
{
    //if(a<0) a=0;
    //if(a>1) a=1;
    return x+ a*(y-x);
}

void noise_map_test()
{
/*
    PerlinField3D p3d(516514);

    const int xres = 512;
    const int yres = 512;

    float* out = new float[xres*yres];

    for(int i=0; i<xres; i++)
    for(int j=0; j<yres; j++)
    {
        const float factor = 1.0;
        float x = i*factor;
        float y = j*factor;

        out[i+j*yres] = p3d.noise(x,y,64.5);
        //out[i+j*yres] = p3d.one_over_f(x,y,64.5);
    }

    save_png("n_map_00", out, xres, yres);

    delete[] out;
*/

    //PerlinField2D p2d_1(515414, 64, 32); //(int seed, int _xs, int _grad_max)
    PerlinField2D p2d_0(5141473, 64, 16); //(int seed, int _xs, int _grad_max)
    PerlinField2D p2d_1(5141473, 32, 16); //(int seed, int _xs, int _grad_max)
    PerlinField2D p2d_2(5154426, 16, 16); //(int seed, int _xs, int _grad_max)
    PerlinField2D p2d_3(1541417, 8, 16); //(int seed, int _xs, int _grad_max)

    const int xres = 512;
    const int yres = 512;

    float* out = new float[xres*yres];

    for(int i=0; i<xres; i++)
    for(int j=0; j<yres; j++)
    {
        float x = i;
        float y = j;

        float n = 0.0;

        n += p2d_0.one_over_f(x,y) + 0.5;
        //n += p2d_1.one_over_f(x,y);
        //n += p2d_2.one_over_f(x,y);
        //n += p2d_3.one_over_f(x,y);


        //n = MIX(p2d_1.one_over_f(x,y), p2d_2.one_over_f(x,y), p2d_3.one_over_f(x,y));
        //n = p2d_1.one_over_f(x,y);
        //n *= p2d_0.abs(x,y);;
        out[i+j*yres] = n;
    }

    save_png("n_map_01", out, xres, yres);

    delete[] out;

}

void noise_map_generate_map()
{
    #if DC_SERVER

    PerlinField3D p3d(516514);

    PerlinField2D p2d_0(51473, 128, 32); //(int seed, int _xs, int _grad_max)
    PerlinField2D p2d_1(5141473, 16, 32); //(int seed, int _xs, int _grad_max)

    int tile = t_map::dat_get_cube_id("regolith");

    // set floor
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        t_map::set(i,j,0,tile);
    }
#if 1
    double sum = 0.0;
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        //float value = 64+8*p3d.noise(i,j,32.5);
        float value = 64;
        //value += 32*p2d_0.one_over_f(i,j);
        value += 1*p2d_1.one_over_f(i,j);

        for (int k=0; k<value; k++) t_map::set(i,j,k,tile);


        //out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    }
#else
    //{
        //float x = i;
        //float y = j;
        //float divisor = (256.0f/float(abs(256-i) + abs(256-j) + abs(i-j))) * 16.0f;
        //float multiplier = pow(2.0, 256.0f/float(abs(256-i)*abs(256-j) + abs(i-j)));
        //multiplier = (multiplier > 1.0f) ? 1.0f : multiplier;
        //x /= divisor;
        //y /= divisor;
        //x *= multiplier;
        //y *= multiplier;
        //float value = p3d.noise(x,y,0);
        //sum += value;
        //int h = (value+1) * 32 + 1;
        //for (int k=0; k<h; k++) t_map::set(i,j,k,tile);
        
        ////out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    //}
#endif
    //float* noisemap = (float*)calloc(512*512, sizeof(float));
    //int n_maps = 4;
    //for (int n=0; n<n_maps; n++)
    //{
        //init_genrand(rand());
        //for (int i=0; i<512; i++)
        //for (int j=0; j<512; j++)
            //noisemap[i+512*j] += p3d.noise(i,j,0);
    //}

    //for (int i=0; i<512; i++)
    //for (int j=0; j<512; j++)
    //{
        //float value = noisemap[i + 512*j] / n_maps;
        //int h = (value+1) * 32 + 1;
        //for (int k=0; k<h; k++) t_map::set(i,j,k,tile);
    //}
    //free(noisemap);
    printf("NOISE MAP AVERAGE: = %f \n", (float) sum / (float)(512*512*32));
    #endif
}


}
