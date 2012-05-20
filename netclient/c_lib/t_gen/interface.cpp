//#include "Simplex.cpp"

#include "interface.hpp"
//#include "Simplex.h"

#include <stdio.h>

#include <math.h>

Simplex s;

void init_p() {

}

double p3(double x, double y, double z) {

return s.Noise3(x,y,z);

}

#define pi 3.14159265
double p4(int x, int y, double x1, double y1, double x2, double y2, double height, double width) {
    float _s=x/width;
    float _t=y/height;

    float dx=x2-x1;
    float dy=y2-y1;

    double nx=x1+cos(_s*2*pi)*dx/(2*pi);
    double ny=y1+cos(_t*2*pi)*dy/(2*pi);
    double nz=x1+sin(_s*2*pi)*dx/(2*pi);
    double nw=y1+sin(_t*2*pi)*dy/(2*pi);

    //double ret =s.Noise3(nx,ny,nz); //,nw);
    //double ret =s.Noise4(nx,ny,nz,nw);
    return s.Noise4(nx,ny,nz,nw);
}
/*
for x=0,bufferwidth-1,1 do
        for y=0,bufferheight-1,1 do
                local s=x/bufferwidth
                local t=y/bufferheight
                local dx=x2-x1
                local dy=y2-y1

                local nx=x1+cos(s*2*pi)*dx/(2*pi)
                local ny=y1+cos(t*2*pi)*dy/(2*pi)
                local nz=x1+sin(s*2*pi)*dx/(2*pi)
                local nw=y1+sin(t*2*pi)*dy/(2*pi)

                buffer:set(x,y,Noise4D(nx,ny,nz,nw))
        end
}
*/


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
        B = Voronoi::Length;
        break;
        case 1:
        B = Voronoi::Length2;
        break;
        case 2:
        B = Voronoi::Manhattan;
        break;
        case 3:
        B = Voronoi::Chebychev;
        break;
        case 4:
        B = Voronoi::Quadratic;
        break;
        case 5:
        B = Voronoi::Minkowski4;
        break;
        case 6:
        B = Voronoi::Minkowski5;
        break;
        default:
        printf("option %i not valid for b\n", b);
        B = Voronoi::Length;
    }

    return Voronoi::Get(x,y,z, A,B);
// VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
    //enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};

}
