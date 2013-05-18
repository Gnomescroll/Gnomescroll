#pragma once

#if DC_CLIENT
# error Don't include this file in the client
#endif

#include <t_gen/twister.hpp>

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
        seed_twister(seed);

        gradient_array = new unsigned char[ssize];
        for (int i=0; i<ssize; i++)
        {
            gradient_array[i] = genrand_int32() % 12; //gradient number
        }
    }

    ~PerlinField3D()
    {
        delete[] this->gradient_array;
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fast_floor(float x)
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

    if (x + y*64 + z*64*64 >= ssize) GS_ABORT();

    return gradient_array[x + y*64 + z*64*64];
}

public:

// Classic Perlin noise, 3D version
float base(float x, float y, float z)
{
/*
    // Find unit grid cell containing point
    int X = fast_floor(x);
    int Y = fast_floor(y);
    int Z = fast_floor(z);
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
    int X = fast_floor(x);
    int Y = fast_floor(y);
    int Z = fast_floor(z);

    //get interpolation ratio

    //if (z >= 32 || z < 0) printf("ERROR1 z = %f \n", z);

    x = x - X;
    y = y - Y;
    z = z - Z;

    //if (z >= 32 || z < 0) printf("ERROR2 z = %f \n", z);
    //if (x<0 || y<0 || z<0) printf("x,y,z= %f %f %f \n", x,y,z);
    //if (z >= 32) printf("x,y,z= %f %f %f X,Y,Z= %i %i %i \n", x+X,y+Y,z+Z,X,Y,Z);
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

        //seed_twister(seed);
        seed_twister(rand());

        ga = new unsigned char[ssize];
        for (int i=0; i<ssize; i++)
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
        for (int i=0; i<grad_max; i++)
        {
            float x = 2*mrandf() -1;
            float y = 2*mrandf() -1;

            float len = sqrt(x*x+y*y);
            x /= len;
            y /= len;

            grad[2*i+0] = x;
            grad[2*i+1] = y;
        }
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fast_floor(float x)
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

    if (x + y*xsize >= ssize) GS_ABORT();

    return ga[x + y*xsize];
}

public:

// Classic Perlin noise, 3D version
float base(float x, float y)
{

    x *= xscale;  //replace with multiplication
    y *= xscale;
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

    // Interpolate along x the contributions from each of the corners
    float nx00 = mix(n00, n10, u);
    float nx10 = mix(n01, n11, u);
    // Interpolate the four results along y
    float nxy = mix(nx00, nx10, v);

    if (nxy < -1 || nxy > 1 ) printf("Error: noise %f \n", nxy);
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

float one_over_f(float x, float y, float persistence)
{
    float tmp = 0;
    float m = 1.0;

    tmp += base(x,y);
    m *= persistence;

    tmp += m * base(2*x, 2*y);
    m *= persistence;

    tmp += m * base(4*x,4*y);
    m *= persistence;

    tmp += m * base(8*x,8*y);

    m *= persistence;
    tmp += m * base(16*x,16*y);
    return tmp;
}

//order 0 is base
float order(float x, float y, float persistence, int order)
{
    float tmp = 0;
    float m = 1.0;
    int b = 1;

    for (int i=0; i<=order; i++)
    {
        tmp += base(b*x,b*y);
        m *= persistence;
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
    //if (a<0) a=0;
    //if (a>1) a=1;
    return x+ a*(y-x);
}

}

}   // t_gen
