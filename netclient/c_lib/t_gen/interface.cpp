
#include "interface.hpp"

#include <t_gen/lib/voronoi.hpp>

double Vo(double x, double y, double z, int a, int b) {

    //BYTE B=BYTE(Round(255*Clamped(0.0, Voronoi::Get(x/10.0,y/10.0, 0.5, Voronoi::Difference21, Voronoi::Quadratic), 1.0))); // Turn the parameter into a Greyscale Level.
    //SetPixel(x,y, RGB(B,B,B));

    //return Voronoi::Get(x/10.0,y/10.0, 0.5, Voronoi::Difference21, Voronoi::Quadratic);

    //enum VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};


    enum Voronoi::VoronoiType A;
    switch(a) {
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
        printf("option %i not valid for a\n", a);
        A = Voronoi::First;
    }
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};

    enum Voronoi::DistanceMethod B;

    switch(b) {
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
    }

    return Voronoi::Get(x,y,z, A,B);
// VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};

}

float voronoi_float(float x, float y, float z);

void dump_voronoi_to_disc()
{
    const int xdim = 512;
    const int ydim = 512;

    float* fm = new float[xdim*ydim];

    //const double scale = 1.0/32.0;
    const double depth = 0.0;
    for(int i=0; i<xdim; i++)
    {
        for(int j=0; j<ydim; j++)
        {
            //double x = scale*((double)i);
            //double y = scale*((double)j);
            //fm[j*xdim + i ] = Voronoi::Get(x,y, depth, Voronoi::First, Voronoi::Minkowski4);
            fm[j*xdim + i ] = voronoi_float(i,j, depth);

        }
    }
    t_gen::save_png("voronoi", fm, xdim, ydim);
    delete[] fm;
}



float voronoi_float(float x, float y, float z)
{

    const float scale = 1.0/32.0;
    const float zscale = 1.0/32.0;
    //float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::First, Voronoi::Manhattan);
    float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::Fourth, Voronoi::Chebychev);

    const float f = 1.5;
    tmp = f*tmp - f/2.0;
    //tmp = 1.0 - tmp;
    if(tmp <= 0.0) return 0.0;
    if(tmp >= 1.0) return 1.0;

    //printf("out= %i \n", (int)(255*tmp) );
    return tmp;
}


unsigned char voronoi_char(float x, float y, float z)
{
    const float scale = 1.0/16.0;
    const float zscale = 1.0/16.0;
    float tmp = Voronoi::Get(x*scale,y*scale,x*zscale, Voronoi::First, Voronoi::Minkowski4);

    tmp = 1.0 - tmp;
    if(tmp <= 0.0) return 128;
    if(tmp >= 1.0) return 255;

    //printf("out= %i \n", (int)(255*tmp) );
    return (unsigned char) 64+((63+128)*tmp);
}
