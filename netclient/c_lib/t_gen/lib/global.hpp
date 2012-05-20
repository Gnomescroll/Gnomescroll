#pragma once

//#include <limits.h> // For INT_MAX etc.
//#include <float.h>  // For DBL_MAX etc.

//#define _USE_MATH_DEFINES // Exposes  M_PI and M_SQRT2
//#include <math.h> // For sqrt, hypot, M_PI and M_SQRT2

#define M_SQRT3     1.7320508075688772935274463415059 // Add a few more defines suitable for long double:
#define M_GOLD      1.6180339887498948482045868343656 // The Golden Ratio suitable for long double


double Min(double  a, double b) {return a<b ? a : b;}
double Max(double a, double b) {return a>b ? a : b;}
//double Clamped(double lo, double it, double hi) {return Min(Max(lo,it),hi);} // useful for acos, asin etc. clamp(-1,1); // (me<lo) ? lo : ((me>hi) ? hi : me);}

static int Floor(double x) 
{ 
  return x>0 ? int(x) : int(x)-1;
}

static int Truncate(double a) 
{
    return int(a);
}

static int Round(double a) 
{
  const double Half=0.5;
    return int(a+Half);
}
