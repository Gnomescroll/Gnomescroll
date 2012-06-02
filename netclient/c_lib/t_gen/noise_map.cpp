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
        if (this->gradient_array != NULL) delete[] this->gradient_array;
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
    tmp += noise(x,y,z);
    tmp += 0.50 * noise(2*x, 2*y,2*z);
    tmp += 0.25 * noise(4*x,4*y,2*z);
    return tmp;
}


};



void noise_map_test()
{
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
}

void noise_map_generate_map()
{
    #if DC_SERVER

    PerlinField3D p3d(516514);

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
        float value = 64+8*p3d.noise(i,j,32.5);

        for (int k=0; k<value; k++) t_map::set(i,j,k,tile);

        //out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    }
#else
    double sum = 0.0;
    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        float x = i;
        float y = j;
        float divisor = (256.0f/float(abs(256-i) + abs(256-j) + abs(i-j))) * 16.0f;
        float multiplier = pow(2.0, 256.0f/float(abs(256-i)*abs(256-j) + abs(i-j)));
        multiplier = (multiplier > 1.0f) ? 1.0f : multiplier;
        x /= divisor;
        y /= divisor;
        x *= multiplier;
        y *= multiplier;
        float value = p3d.noise(x,y,0);
        sum += value;
        int h = (value+1) * 32 + 1;
        for (int k=0; k<h; k++) t_map::set(i,j,k,tile);
        
        //out[i+j*yres] = p3d.one_over_f(x,y,64.0);
    }
#endif
    printf("NOISE MAP AVERAGE: = %f \n", (float) sum / (float)(512*512*32));
    #endif
}


}
