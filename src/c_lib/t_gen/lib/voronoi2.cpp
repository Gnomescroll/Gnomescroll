
#include "voronoi2.hpp"

#include <t_gen/lib/voronoi.hpp>

double Vo(double x, double y, double z, int a, int b)
{
    //BYTE B=BYTE(Round(255*Clamped(0.0, Voronoi::Get(x/10.0,y/10.0, 0.5, Voronoi::Difference21, Voronoi::Quadratic), 1.0))); // Turn the parameter into a Greyscale Level.
    //SetPixel(x,y, RGB(B,B,B));

    //return Voronoi::Get(x/10.0,y/10.0, 0.5, Voronoi::Difference21, Voronoi::Quadratic);

    //enum VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};


    enum Voronoi::VoronoiType A = Voronoi::First;
    switch (a)
    {
        case 0:
            A = Voronoi::First;
            break;
        case 1:
            A = Voronoi::Second;
            break;
        case 2:
            A = Voronoi::Third;
            break;
        case 3:
            A = Voronoi::Fourth;
            break;
        case 4:
            A = Voronoi::Difference21;
            break;
        case 5:
            A = Voronoi::Difference32;
            break;
        case 6:
            A = Voronoi::Crackle;
            break;

        default:
            printf("option %i not valid for a\n", a);
            A = Voronoi::First;
            break;
    }
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};

    enum Voronoi::DistanceMethod B = Voronoi::Length;

    switch (b)
    {
        case 0:
            B = Voronoi::Length;break;
        case 1:
            B = Voronoi::Length2;break;
        case 2:
            B = Voronoi::Manhattan;break;
        case 3:
            B = Voronoi::Chebychev;break;
        case 4:
            B = Voronoi::Quadratic;break;
        case 5:
            B = Voronoi::Minkowski4;break;
        case 6:
            B = Voronoi::Minkowski5;break;
        default:
            printf("option %i not valid for b\n", b);
            B = Voronoi::Length;
            break;
    }

    return Voronoi::Get(x,y,z, A,B);
    // VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};

}



float voronoi_float(float x, float y, float z);
float voronoi_float_fast(int x, int y, int z);


float* vornoi_map_00 = NULL;

static const int VOI_00_MASK = ~7; // every 8 blocks
static const int VOI_00_INTERPOLATION_XY = 8; // every 8 blocks
static const int VOI_00_INTERPOLATION_Z = 8; // every 16 blocks


/*
static const float lerp_factor[16] = {
    0.0,1.0,2.0,3.0,
    4.0,5.0,6.0,7.0,
    8.0,9.0,10.0,11.0,
    12.0,13.0,14.0,15.0
};
*/





void dump_voronoi_to_disc()
{
    const int xdim = 512;
    const int ydim = 512;

    static const int xmax = 512 / VOI_00_INTERPOLATION_XY;
    static const int ymax = 512 / VOI_00_INTERPOLATION_XY;
    static const int xymax = xmax*ymax;

    float* fm1 = new float[xdim*ydim];
    float* fm2 = new float[xdim*ydim];
    float* fm3 = new float[xdim*ydim];
    //const double scale = 1.0/32.0;
    //const double depth = 0.0;
    for (int i=0; i<xdim; i++)
    for (int j=0; j<ydim; j++)
    {
        //fm[j*xdim + i ] = Voronoi::Get(x,y, depth, Voronoi::First, Voronoi::Minkowski4);
        fm1[j*xdim + i ] = voronoi_float(i,j, 0);
        fm2[j*xdim + i ] = vornoi_map_00[xymax*(0>>3) + xmax*(j>>3) + (i>>3)];
        fm3[j*xdim + i ] = voronoi_float_fast(i,j,0);
    }

    t_gen::save_png("voronoi1", fm1, xdim, ydim);
    t_gen::save_png("voronoi2", fm2, xdim, ydim);
    t_gen::save_png("voronoi3", fm3, xdim, ydim);
    delete[] fm1;
    delete[] fm2;
    delete[] fm3;
}

void init_voronoi_noise_maps()
{
    /*
        DEBUG RETURN
    */
    return;

    static const int xmax = 512 / VOI_00_INTERPOLATION_XY;
    static const int ymax = 512 / VOI_00_INTERPOLATION_XY;
    static const int zmax = 128 / VOI_00_INTERPOLATION_Z;

    static const int xymax = xmax*ymax;

    vornoi_map_00 = new float [xmax*ymax*zmax*2]; //2 for safety

    //const float scale = 1.0/32.0;
    //const float zscale = 1.0/32.0;

    //const float scale = 1.0/512.0;
    //const float zscale = 1.0/512.0;


    //float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::First, Voronoi::Manhattan);

    for (int i=0; i<xmax; i++)
    for (int j=0; j<ymax; j++)
    for (int k=0; k<zmax; k++)
    {

        vornoi_map_00[xymax*k + xmax*j + i] = voronoi_float(i*8, j*8, k*8);
    }


    //dump_voronoi_to_disc();

}

//#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

float voronoi_float_fast(int x, int y, int z)
{
/*
    static const float lerp_factor4[4] =
    {
        0.0, 0.25, 0.50, 0.75
    };
*/
    static const float lerp_factor8[8] =
    {
        0.000, 0.125, 0.250, 0.375,
        0.500, 0.625, 0.750, 0.875
    };
/*
    static const float lerp_factor16[16] =
    {
        0.000, 0.0625, 0.125, 0.1875,
        0.250, 0.3125, 0.375, 0.4375,
        0.500, 0.5625, 0.625, 0.6875,
        0.750, 0.8125, 0.875, 0.9675
    };
*/
    static const int xmax = 512 / VOI_00_INTERPOLATION_XY;
    static const int ymax = 512 / VOI_00_INTERPOLATION_XY;
    //static const int zmax = 128 / VOI_00_INTERPOLATION_Z;

    static const int xymax = xmax*ymax;

/*
    const float ax = lerp_factor[x%4];
    const float ay = lerp_factor[y%4];
    const float az = lerp_factor[z%4];
*/

    const float ax = lerp_factor8[x&7];
    const float ay = lerp_factor8[y&7];
    const float az = lerp_factor8[z&7];

    x = x >> 3;
    y = y >> 3;
    z = z >> 4;

    const float l1 = vornoi_map_00[xymax*z + xmax*y + x];
    const float l2 = vornoi_map_00[xymax*z + xmax*y + (x+1)];
    const float l3 = vornoi_map_00[xymax*z + xmax*(y+1) + x];
    const float l4 = vornoi_map_00[xymax*z + xmax*(y+1) + (x+1)];

    const float lx1 = l1 + ax*(l2-l1);
    const float lx2 = l3 + ax*(l4-l3);
    const float lx3 = lx1 + ay*(lx2 - lx1);

    //const float lx1 = lerp(ax, l1, l2);
    //const float lx2 = lerp(ax, l3, l4);
    //const float lx3 = lerp(ay, lx1, lx2);


    //return lx3;

    const float l5 = vornoi_map_00[xymax*(z+1) + xmax*y + x];
    const float l6 = vornoi_map_00[xymax*(z+1) + xmax*y + (x+1)];
    const float l7 = vornoi_map_00[xymax*(z+1) + xmax*(y+1) + x];
    const float l8 = vornoi_map_00[xymax*(z+1) + xmax*(y+1) + (x+1)];

    const float lx4 = l5 + ax*(l6-l5);
    const float lx5 = l7 + ax*(l8-l7);
    const float lx6 = lx4 + ay*(lx5 - lx4);

    //const float lx4 = lerp(ax, l5, l6);
    //const float lx5 = lerp(ax, l7, l8);
    //const float lx6 = lerp(ay, lx4, lx5);

    const float value = lx3 + az*(lx6-lx3);
    //const float value = lerp(az, lx3, lx6);
/*
    GS_ASSERT(lerp(ax, l1, l2) == l1 + ax*(l2-l1) );
    GS_ASSERT(lerp(ax, l3, l4) == l3 + ax*(l4-l3) );
    GS_ASSERT(lerp(ay, lx1, lx2) == lx1 + ay*(lx2 - lx1) );

    GS_ASSERT(lerp(ax, l5, l6) == l5 + ax*(l6-l5) );
    GS_ASSERT(lerp(ax, l7, l8) == l7 + ax*(l8-l7) );
    GS_ASSERT(lerp(ay, lx4, lx5) == lx4 + ay*(lx5 - lx4) );

    GS_ASSERT(lerp(az, lx3, lx6) == lx3 + az*(lx6-lx3) );
*/
    return value;

}

// VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
//enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};
float voronoi_float(float x, float y, float z)
{

    //static const float scale = 1.0/64.0;
    //static const float zscale = 1.0/64.0;

    const float scale = 1.0/32.0;
    const float zscale = 1.0/16.0;

    const double _x = scale*x;
    const double _y = scale*y;
    const double _z = zscale*z;

    //float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::First, Voronoi::Manhattan);
    float tmp = Voronoi::Get(_x,_y,_z, Voronoi::First, Voronoi::Length2);

//    if (tmp <= 0.0) return 0.0;
//    if (tmp >= 1.0) return 1.0;

    //tmp = tmp*2.0 - 1.0;

    if (tmp <= 0.0) return 0.0;

    if (tmp >= 1.0) return 1.0;
/*
    const float f = 3;
    tmp = f*tmp - f/2.0;
    //tmp = 1.0 - tmp;
    if (tmp <= 0.0) return 0.0;
    if (tmp >= 1.0) return 1.0;
*/
    //printf("out= %i \n", (int)(255*tmp) );
    return tmp;
}

//#define lerp(t, a, b) ((a) + (t) * ((b) - (a)))

unsigned char voronoi_char(float x, float y, float z)
{
    const float scale = 1.0/16.0;
    const float zscale = 1.0/16.0;
    float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::First, Voronoi::Minkowski4);

    tmp = 1.0 - tmp;
    if (tmp <= 0.0) return 128;
    if (tmp >= 1.0) return 255;

    //printf("out= %i \n", (int)(255*tmp) );
    return (unsigned char) 64+((63+128)*tmp);
}
