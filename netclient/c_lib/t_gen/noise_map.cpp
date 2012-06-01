#include "noise_map.hpp"


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
        init_genrand(seed);

        gradient_array = new unsigned char[ssize];
        for(int i=0; i<ssize; i++)
        {
            gradient_array[i] = genrand_int32() % 12; //gradient number
        }
    }

// This method is a *lot* faster than using (int)Math.floor(x)
static inline int fastfloor(float x) 
{
return x>0 ? (int)x : (int)x-1;
}

static inline float dot(const int g[], float x, float y, float z)
{
return g[0]*x + g[1]*y + g[2]*z;
}

static inline float mix(float a, float b, float t) 
{
    return (1-t)*a + t*b;
//    return a + t*(a-b);   //optimized version
}

static inline float fade(float t) 
{
return t*t*t*(t*(t*6-15)+10);
}

inline int get_gradiant(int x, int y, int z)
{
    x = x % 64; //replace with bitmask
    y = y % 64;
    GS_ASSERT(z > 0 && z < 32);

    GS_ASSERT(x + y*64 + z*64*64 < ssize)
    return gradient_array[x + y*64 + z*64*64];
}

public:

// Classic Perlin noise, 3D version
float noise(float x, float y, float z) 
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
    x /= 8.0;
    y /= 8.0;
    z /= 4.0;
    //get grid point
    int X = fastfloor(x);
    int Y = fastfloor(y);
    int Z = fastfloor(z);

    //get interpolation ratio

    x = x - X;
    y = y - Y;
    z = z - Z;

    // Calculate a set of eight hashed gradient indices


    int gi000 = get_gradiant(X+0,Y+0,Z+0);
    int gi001 = get_gradiant(X+0,Y+0,Z+1);
    int gi010 = get_gradiant(X+0,Y+1,Z+0);
    int gi011 = get_gradiant(X+0,Y+1,Z+1);

    int gi100 = get_gradiant(X+1,Y+0,Z+0);
    int gi101 = get_gradiant(X+1,Y+0,Z+1);
    int gi110 = get_gradiant(X+1,Y+1,Z+0);
    int gi111 = get_gradiant(X+1,Y+1,Z+1);

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
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
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

    return nxyz;
}

};



void noise_map_test()
{
    PerlinField3D p3d(5);

    const int xres = 512;
    const int yres = 512;

    float* out = new float[xres*yres];

    for(int i=0; i<xres; i++)
    for(int j=0; j<yres; j++)
    {
        out[i+j*yres] = p3d.noise(2*i,2*j,64.0);
    }

    save_png("n_map_00", out, xres, yres);
}


}