//#include "Simplex.cpp"

#include "interface.hpp"
//#include "Simplex.h"

#include <stdio.h>

Simplex s;

void init_p() {

}

double p3(double x, double y, double z) {

return s.Noise3(x,y,z);

}



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
