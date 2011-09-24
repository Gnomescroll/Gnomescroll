// Simplex.h

#pragma once

class Simplex {
  static unsigned char Mutate[512];
  static void Grad1(int Hash, double* gx);
  static void Grad2(int Hash, double* gx, double* gy);
  static void Grad3(int Hash, double* gx, double* gy, double* gz);
  static void Grad4(int Hash, double* gx, double* gy, double* gz, double* gw);
  static double Max(double a, double b) {return a>b ? a : b;}
  static double Min(double a, double b) {return a<b ? a : b;}
  static double MinMax(double lo, double me, double hi) {return Min(Max(lo,me),hi);}
/*Standard Floor method flushes the processor pipeline when rounding mode is set!
If exactly in the middle (like 0.5) the FPU rounds to the nearest even number.
Shift and scale this to the answer required. */
  static int Floor(double x) {  return x>0 ? int(x) : int(x)-1;}
  /*
    ASSERT(a>(INT_MIN/2.0-1));
    ASSERT(a<(INT_MAX/2.0+1));
    const double Half=-0.5;
    int i;
    __asm {
      fld   a        //         a
      fadd  st,st(0) //       2*a
      fadd  Half     //       2*a-0.5   use fsubr instead to implement Ceil
      fistp i        // Round(2*a-0.5)
      sar   i,1      // Round(2*a-0.5)/2
    }
    return i;
  }
  * */
public:
  static double Noise1(double x, double* dx=0); // 1D simplex noise with optionally returned derivative.
  static double Noise2(double x, double y, double* dx=0, double* dy=0); // The analytic derivative is for flow animations, curl noise, analytic antialiasing etc.
  static double Noise3(double x, double y, double z, double* dx=0, double* dy=0, double* dz=0);
  static double Noise4(double x, double y, double z, double w, double* dx=0, double* dy=0, double* dz=0, double* dw=0);
};
