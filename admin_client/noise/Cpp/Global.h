// Global.h

#ifndef Globalh
#define Globalh

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* This file provides the most fundamental and generic constants and templates for the graphics primitives.
The things in here are used a lot: it's a tiny file, so it's worth remembering what's here.
Most things are obvious, but the following four are worth understanding:
_______________________________________________________________________________
Minimise mapping by having everything return normalised ranges [0,1] or [-1,1].
Unsigned Interval is [0,1]; Signed interval is [-1,1]; These change between the two (useful for double or float): */
template<typename T> T  Unsigned(T t) {return (t+1)/2;}
template<typename T> T    Signed(T t) {return 2*t-1;}

template<typename T> double Parameterize(T x, T a, T b) { return double(x-a)/(b-a);} // x is normally between a and b. returns 0 if x==a, 1 if x==b, 0.5 if x is in the middle of a and b etc.
template<typename T> T Interpolate(const double& t, T a, T b) {return T(a+(b-a)*t);} // The reverse of Parameterize: if t=0.5, returns the value in the middle of a and b.
//_____________________________________________________________________________

#include <limits.h> // For INT_MAX etc.
#include <float.h>  // For DBL_MAX etc.

#define _USE_MATH_DEFINES // Exposes  M_PI and M_SQRT2
#include <math.h> // For sqrt, hypot, M_PI and M_SQRT2
#define M_SQRT3     1.7320508075688772935274463415059 // Add a few more defines suitable for long double:
#define M_GOLD      1.6180339887498948482045868343656 // The Golden Ratio suitable for long double
//_____________________________________________________________________________

template<typename T> T        Min(T  a, T  b)       {return a<b ? a : b;}
template<typename T> T        Max(T  a, T  b)       {return a>b ? a : b;}
template<typename T> T        Min(T  a, T  b, T  c) {return Min(Min(a,b),c);}
template<typename T> T        Max(T  a, T  b, T  c) {return Max(Max(a,b),c);}
template<typename T> T   Saturate(      T it      ) {return Min(Max(0.0,it),1.0);} // Convention from GPU shaders
template<typename T> T    Clamped(T lo, T it, T hi) {return Min(Max(lo,it),hi);} // useful for acos, asin etc. clamp(-1,1); // (me<lo) ? lo : ((me>hi) ? hi : me);}
template<typename T> bool Between(T lo, T it, T hi) {return (lo<=it)&&(it<=hi);} // Inclusive
template<typename T> bool  Within(T lo, T it, T hi) {return (lo< it)&&(it< hi);} // Exclusive
/*
template<typename T> T  Floor(T a) This is defined at the bottom of this file.
template<typename T> T  Round(T a) This is defined at the bottom of this file.

_________________________ Assembly Language Speed-ups _________________________

Standard Floor method flushes the processor pipeline when rounding mode is set!
If exactly in the middle (like 0.5) the FPU rounds to the nearest even number.
Shift and scale this to the answer required. */
static int Floor(double x) { return x>0 ? int(x) : int(x)-1;}

/*
  //ASSERT(a>(INT_MIN/2.0-1));
  //ASSERT(a<(INT_MAX/2.0+1));
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
*/

// Use the same trick for casting to integer types:
static int Truncate(double a) { // c++ equivalent is return int(x);
    return int(a);
}
// Use the same trick for rounding:
static int Round(double a) {
  //ASSERT(a>(INT_MIN/2.0-1));
  //ASSERT(a<(INT_MAX/2.0+1));
  const double Half=0.5;
    return int(a+Half);
}

//________________ Extra functions that depend on those above _________________

static double Frac(const double& a)          {return a- Truncate(a);}
static double Mod(double a, const double& b) {return a-=Truncate(a/b)*b;}

//________________________ Fast Trigonometry functions ________________________

// Sine and Cosine: Angle in Radians must be in the range [-2^63, 2^63]:

/*
static void sincos(double Radians, double* Sine, double* Cosine) {
  //ASSERT(Radians>-2e63);
  //ASSERT(Radians< 2e63);
  __asm {
    fld  Radians
    fsincos
    mov  eax,dword ptr [Cosine]
    fstp qword ptr [eax]
    mov  eax,dword ptr [Sine]
    fstp qword ptr [eax]
  }
}
*/

/*
#pragma warning (disable: 4725) // fdiv instruction may be inaccurate on some Pentiums
  static double sinc(double Radians) {
    //ASSERT(Radians>-2e63);
    //ASSERT(Radians< 2e63);
    if(Radians==0) return 1;
    double d;
    __asm { // C++ equivalent is: Radians*=M_PI; return (Radians1==0 ? 1 : sin(Radians1)/Radians1);
      fld Radians      //    Radians
      fldpi            //              PI
      fmul             //    Radians * PI
      fst qword ptr d  //  d=Radians * PI
      fsin             //   sin(d)
      fdiv d           //   sin(d) / d
      fstp qword ptr d // d=sin(d) / d
    }
    return d;
  }
  */
#pragma warning (default: 4725)
//_____________________________________________________________________________

#endif // ndef Globalh
