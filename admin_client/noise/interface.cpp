//#include "Simplex.cpp"

#include "interface.hpp"
//#include "Simplex.h"

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

    return Voronoi::Get(x,y,z, Voronoi::Difference21, Voronoi::Quadratic);
}
