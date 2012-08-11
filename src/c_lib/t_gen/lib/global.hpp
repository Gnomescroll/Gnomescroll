#pragma once

#include "global.hpp"
#include "twister.hpp"

//#include <limits.h> // For INT_MAX etc.
//#include <float.h>  // For DBL_MAX etc.

//#define _USE_MATH_DEFINES // Exposes  M_PI and M_SQRT2
//#include <math.h> // For sqrt, hypot, M_PI and M_SQRT2

#define M_SQRT3     1.7320508075688772935274463415059 // Add a few more defines suitable for long double:
#define M_GOLD      1.6180339887498948482045868343656 // The Golden Ratio suitable for long double

double Min(double  a, double b) {return a<b ? a : b;}
double Max(double a, double b) {return a>b ? a : b;}

static int Floor(double x) {  return x>0 ? int(x) : int(x)-1; }

struct g3Vector
{
  //gCoord x,y,z;
  double x,y,z;

  g3Vector() {}

  g3Vector(double X, double Y, double Z) {x=X; y=Y; z=Z;}
  void Set(double X, double Y, double Z) {x=X; y=Y; z=Z;}

  double GetLength    () const {return sqrt(GetL2());} // Euclidean (shortest line)
  double GetL2        () const {return x*x+y*y+z*z;} // The length squared. Saves the slow Square Root for some calculations.
  double GetManhattan () const {double d=fabs(x)+fabs(y)+fabs(z); return d*d;} // The length of the distance in axial directions (as if travelling around a city).
  double GetChebychev () const {double d=Max(fabs(x),Max(fabs(y),fabs(z))); return d*d;} // The length of the longest Axial journey.
  double GetQuadratic () const {return GetL2()+x*y+x*z+y*z;} // The sum of everything multiplied by everything else!
  double GetMinkowski(double p) const {return pow(pow(fabs(x), p) + pow(fabs(y), p) + pow(fabs(z), M_E), 1.0/p);} // General (and slowest) case. p=1 is Chebychev, p=2 is the Euclidean and P at infinity is Chebychev!
  double GetMinkowski4() const {return pow(x*x*x*x+y*y*y*y+z*z*z*z,0.25);}                       // Same as Minkowski(4);
  double GetMinkowski5() const {double d=sqrt(fabs(x))+sqrt(fabs(y))+sqrt(fabs(z)); return d*d;} // Same as Minkowski(0.5);

};
