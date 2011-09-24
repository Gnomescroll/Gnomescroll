// gCoord.h

#ifndef gCoordh
#define gCoordh

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Global.h"
#include "./Twister.h"

#include <string.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>> gCoord <<<<<<<<<<<<<<<<<<<<<<<<<<<<
This is a Quantised double (for when you only need a known number of decimal places in comparisons - which is ALWAYS).
The value is never truncated, so rounding errors are minimised.
The only effect of the quantisation is on the comparison functions which all ultimately use the sgn function.
Be aware that if you use the standard min and max macros (etc.) on these, then the Tolerance will be used...
This can introduce bugs if you let it!
For example:
  gCoord A=1.000000001;
  A=acos(min(max(-1.0, A), 1.0)); // This will NOT clamp the value of A!!!
To make sure that the normal, untoleranced <kbd><span class="KeyWord">double</span></kbd> comparison is used, you should use:
  A=acos(min(max(-1.0, A.Coord), 1.0));
  A=acos(MinMax (-1.0, A.Coord , 1.0)); // This will work. I also provide simple templated Min and Max to avoid the macro versions recalculation issues
  A.Clamp(-1,1); // This followed by...
  A=acos(A);    // ...this will work
  A=acos(A.Clamped(-1,1)); // This is what I would recommend!

All the rest of the code is to make the class work as if it were derived from "double" as a class.
*/

#define gCoordTolerance (gCoord::GetTolerance()) // <<<< This is the whole point of the gCoord struct.

class gCoord { // A quantised double. Quantisation Tolerance is handled in the sgn function:
#if _MSC_VER < 1310
  public: // Older versions won't let NewTolerance access Tolerance() and you can't friend a sub-structure
#endif
  static double Tolerance(double NewTolerance=0) {
    static double GlobalTolerance=0.0001; // <<<< This is the whole point of the gCoord class. Smallest value would be DBL_EPSILON
    if(NewTolerance) GlobalTolerance=NewTolerance;
    return GlobalTolerance;
  }
public:
  static int sgn(double d) {return d<=-gCoordTolerance ? -1 : (d>=gCoordTolerance);}
  static double GetTolerance() {return Tolerance();}
  struct NewTolerance { // Use to temporarily alter the tolerance NOT THREAD SAFE
    double OldTolerance;
    NewTolerance(double NewTolerance) : OldTolerance(gCoord::Tolerance()) {gCoord::Tolerance(NewTolerance);}
   ~NewTolerance() {try{gCoord::Tolerance(OldTolerance);}catch(...){}}
  };
#ifdef _DEBUG
  union {
    double Coord; // <<<<<<<<<<<<<<<<<<< The value being controlled
    long lCoord;
  };
  gCoord() : lCoord(0xBadF00D) {} // Debug initialises to Bad Food. Coord=-9.2559594354769001e+061 lCoord=0x0badf00d
#else
  double Coord; // <<<<<<<<<<<<<<<<<<< The value being controlled
  gCoord() {}
#endif
  gCoord(double d) : Coord(d) {}
  gCoord(double RandMin, double RandMax) : Coord(Twister::Get(RandMin,RandMax)) {} // Random Value
  gCoord& operator= (const gCoord&        c)       {                    Coord= c.Coord; return *this;}
  gCoord& operator+=(const gCoord&        c)       {                    Coord+=c.Coord; return *this;}
  gCoord& operator-=(const gCoord&        c)       {                    Coord-=c.Coord; return *this;}
  gCoord& operator*=(const gCoord&        c)       {                    Coord*=c.Coord; return *this;}
  gCoord& operator/=(const gCoord&        c)       { Coord/=c.Coord; return *this;}
  gCoord  operator+ (const gCoord&        c) const {                    return Coord+c.Coord;}
  gCoord  operator- (const gCoord&        c) const {                    return Coord-c.Coord;}
  gCoord  operator* (const gCoord&        c) const {                    return Coord*c.Coord;}
  gCoord  operator/ (const gCoord&        c) const { return Coord/c.Coord;}
  bool    operator==(const gCoord&        c) const {                    return Compare(c.Coord)==0;}
  bool    operator!=(const gCoord&        c) const {                    return Compare(c.Coord)!=0;}
  bool    operator<=(const gCoord&        c) const {                    return Compare(c.Coord)<=0;}
  bool    operator>=(const gCoord&        c) const {                    return Compare(c.Coord)>=0;}
  bool    operator< (const gCoord&        c) const {                    return Compare(c.Coord)< 0;}
  bool    operator> (const gCoord&        c) const {                    return Compare(c.Coord)> 0;}
  gCoord& operator= (const double         d)       {              Coord= d; return *this;}
  gCoord& operator+=(const double         d)       {              Coord+=d; return *this;}
  gCoord& operator-=(const double         d)       {              Coord-=d; return *this;}
  gCoord& operator*=(const double         d)       {              Coord*=d; return *this;}
  gCoord& operator/=(const double         d)       { Coord/=d; return *this;}
  gCoord  operator+ (const double         d) const {              return Coord+d;}
  gCoord  operator- (const double         d) const {              return Coord-d;}
  gCoord  operator* (const double         d) const {              return Coord*d;}
  gCoord  operator/ (const double         d) const { return Coord/d;}
  bool    operator==(const double         d) const {              return Compare(d)==0;}
  bool    operator!=(const double         d) const {              return Compare(d)!=0;}
  bool    operator<=(const double         d) const {              return Compare(d)<=0;}
  bool    operator>=(const double         d) const {              return Compare(d)>=0;}
  bool    operator< (const double         d) const {              return Compare(d)< 0;}
  bool    operator> (const double         d) const {              return Compare(d)> 0;}
  gCoord& operator= (const short          i)       {              Coord= i; return *this;}
  gCoord& operator+=(const short          i)       {              Coord+=i; return *this;}
  gCoord& operator-=(const short          i)       {              Coord-=i; return *this;}
  gCoord& operator*=(const short          i)       {              Coord*=i; return *this;}
  gCoord& operator/=(const short          i)       { Coord/=i; return *this;}
  gCoord  operator+ (const short          i) const {              return Coord+i;}
  gCoord  operator- (const short          i) const {              return Coord-i;}
  gCoord  operator* (const short          i) const {              return Coord*i;}
  gCoord  operator/ (const short          i) const { return Coord/i;}
  bool    operator==(const short          i) const {              return Compare(i)==0;}
  bool    operator!=(const short          i) const {              return Compare(i)!=0;}
  bool    operator<=(const short          i) const {              return Compare(i)<=0;}
  bool    operator>=(const short          i) const {              return Compare(i)>=0;}
  bool    operator< (const short          i) const {              return Compare(i)< 0;}
  bool    operator> (const short          i) const {              return Compare(i)> 0;}
  gCoord& operator= (const int            i)       {              Coord= i; return *this;}
  gCoord& operator+=(const int            i)       {              Coord+=i; return *this;}
  gCoord& operator-=(const int            i)       {              Coord-=i; return *this;}
  gCoord& operator*=(const int            i)       {              Coord*=i; return *this;}
  gCoord& operator/=(const int            i)       { Coord/=i; return *this;}
  gCoord  operator+ (const int            i) const {              return Coord+i;}
  gCoord  operator- (const int            i) const {              return Coord-i;}
  gCoord  operator* (const int            i) const {              return Coord*i;}
  gCoord  operator/ (const int            i) const {  return Coord/i;}
  bool    operator==(const int            i) const {              return Compare(i)==0;}
  bool    operator!=(const int            i) const {              return Compare(i)!=0;}
  bool    operator<=(const int            i) const {              return Compare(i)<=0;}
  bool    operator>=(const int            i) const {              return Compare(i)>=0;}
  bool    operator< (const int            i) const {              return Compare(i)< 0;}
  bool    operator> (const int            i) const {              return Compare(i)> 0;}
  gCoord& operator= (const long           i)       {              Coord= i; return *this;}
  gCoord& operator+=(const long           i)       {              Coord+=i; return *this;}
  gCoord& operator-=(const long           i)       {              Coord-=i; return *this;}
  gCoord& operator*=(const long           i)       {              Coord*=i; return *this;}
  gCoord& operator/=(const long           i)       {  Coord/=i; return *this;}
  gCoord  operator+ (const long           i) const {              return Coord+i;}
  gCoord  operator- (const long           i) const {              return Coord-i;}
  gCoord  operator* (const long           i) const {              return Coord*i;}
  gCoord  operator/ (const long           i) const {  return Coord/i;}
  bool    operator==(const long           i) const {              return Compare(i)==0;}
  bool    operator!=(const long           i) const {              return Compare(i)!=0;}
  bool    operator<=(const long           i) const {              return Compare(i)<=0;}
  bool    operator>=(const long           i) const {              return Compare(i)>=0;}
  bool    operator< (const long           i) const {              return Compare(i)< 0;}
  bool    operator> (const long           i) const {              return Compare(i)> 0;}
  gCoord& operator= (const unsigned short i)       {              Coord= i; return *this;}
  gCoord& operator+=(const unsigned short i)       {              Coord+=i; return *this;}
  gCoord& operator-=(const unsigned short i)       {              Coord-=i; return *this;}
  gCoord& operator*=(const unsigned short i)       {              Coord*=i; return *this;}
  gCoord& operator/=(const unsigned short i)       {  Coord/=i; return *this;}
  gCoord  operator+ (const unsigned short i) const {              return Coord+i;}
  gCoord  operator- (const unsigned short i) const {              return Coord-i;}
  gCoord  operator* (const unsigned short i) const {              return Coord*i;}
  gCoord  operator/ (const unsigned short i) const {  return Coord/i;}
  bool    operator==(const unsigned short i) const {              return Compare(i)==0;}
  bool    operator!=(const unsigned short i) const {              return Compare(i)!=0;}
  bool    operator<=(const unsigned short i) const {              return Compare(i)<=0;}
  bool    operator>=(const unsigned short i) const {              return Compare(i)>=0;}
  bool    operator< (const unsigned short i) const {              return Compare(i)< 0;}
  bool    operator> (const unsigned short i) const {              return Compare(i)> 0;}
  gCoord& operator= (unsigned const int   i)       {              Coord= i; return *this;}
  gCoord& operator+=(unsigned const int   i)       {              Coord+=i; return *this;}
  gCoord& operator-=(unsigned const int   i)       {              Coord-=i; return *this;}
  gCoord& operator*=(unsigned const int   i)       {              Coord*=i; return *this;}
  gCoord& operator/=(unsigned const int   i)       {  Coord/=i; return *this;}
  gCoord  operator+ (unsigned const int   i) const {              return Coord+i;}
  gCoord  operator- (unsigned const int   i) const {              return Coord-i;}
  gCoord  operator* (unsigned const int   i) const {              return Coord*i;}
  gCoord  operator/ (unsigned const int   i) const {  return Coord/i;}
  bool    operator==(unsigned const int   i) const {              return Compare(i)==0;}
  bool    operator!=(unsigned const int   i) const {              return Compare(i)!=0;}
  bool    operator<=(unsigned const int   i) const {              return Compare(i)<=0;}
  bool    operator>=(unsigned const int   i) const {              return Compare(i)>=0;}
  bool    operator< (unsigned const int   i) const {              return Compare(i)< 0;}
  bool    operator> (unsigned const int   i) const {              return Compare(i)> 0;}
  gCoord& operator= (unsigned const long  i)       {              Coord= i; return *this;}
  gCoord& operator+=(unsigned const long  i)       {              Coord+=i; return *this;}
  gCoord& operator-=(unsigned const long  i)       {              Coord-=i; return *this;}
  gCoord& operator*=(unsigned const long  i)       {              Coord*=i; return *this;}
  gCoord& operator/=(unsigned const long  i)       {  Coord/=i; return *this;}
  gCoord  operator+ (unsigned const long  i) const {              return Coord+i;}
  gCoord  operator- (unsigned const long  i) const {              return Coord-i;}
  gCoord  operator* (unsigned const long  i) const {              return Coord*i;}
  gCoord  operator/ (unsigned const long  i) const {  return Coord/i;}
  bool    operator==(unsigned const long  i) const {              return Compare(i)==0;}
  bool    operator!=(unsigned const long  i) const {              return Compare(i)!=0;}
  bool    operator<=(unsigned const long  i) const {              return Compare(i)<=0;}
  bool    operator>=(unsigned const long  i) const {              return Compare(i)>=0;}
  bool    operator< (unsigned const long  i) const {              return Compare(i)< 0;}
  bool    operator> (unsigned const long  i) const {              return Compare(i)> 0;}
  gCoord  operator-      ()                  const {return -Coord;}
  bool    operator!      ()                  const {return Compare(0)==0;}
          operator   bool()                  const {return Compare(0)!=0;}
          operator    int()                  const {return Round(Coord) ;}
          operator double()                  const {return Coord           ;}
  int     Compare(double d)                  const {return sgn(Coord-d)    ;}
  int     sgn()                              const {return sgn(Coord)      ;}
  gCoord  abs()                              const {return fabs(Coord)     ;}
  //double  RawReport()                        const {gCoord c(gCoordTolerance*(Coord<0 ? ceil(-0.5+Coord/gCoordTolerance) : floor(0.5+Coord/gCoordTolerance))); return c.abs() ? c : 0;} // Rounds to gCoordTolerance and forces return of 0 to stop small returns like: 1e-14
  ///CString Report()                           const {CString S; S.Format("%g",RawReport()); return S;}
  gCoord Clamped(double lo, double hi)       const {return ::Clamped(lo,Coord,hi);} // Useful for acos, asin etc. Clamp(-1,1);
  void   Clamp  (double lo, double hi)             {Coord=Clamped(lo,hi);} // Useful for acos, asin etc. Clamp(-1,1);
  bool   Between(double lo, double hi)       const {return (lo<=Coord)&&(Coord<=hi);} // Inclusive
  bool   Within (double lo, double hi)       const {return (lo< Coord)&&(Coord< hi);} // Exclusive
};

__inline gCoord operator+ (double         d, const gCoord& c) {return d+c.Coord;}
__inline gCoord operator- (double         d, const gCoord& c) {return d-c.Coord;}
__inline gCoord operator* (double         d, const gCoord& c) {return d*c.Coord;}
__inline gCoord operator/ (double         d, const gCoord& c) {return d/c.Coord;}
__inline double operator+=(double&        d, const gCoord& c) {return d+=c.Coord;}
__inline double operator-=(double&        d, const gCoord& c) {return d-=c.Coord;}
__inline double operator*=(double&        d, const gCoord& c) {return d*=c.Coord;}
__inline double operator/=(double&        d, const gCoord& c) {return d/=c.Coord;}
__inline bool   operator==(double         d, const gCoord& c) {return c.Compare(d)==0;}
__inline bool   operator!=(double         d, const gCoord& c) {return c.Compare(d)!=0;}
__inline bool   operator<=(double         d, const gCoord& c) {return c.Compare(d)>=0;}
__inline bool   operator>=(double         d, const gCoord& c) {return c.Compare(d)<=0;}
__inline bool   operator< (double         d, const gCoord& c) {return c.Compare(d)> 0;}
__inline bool   operator> (double         d, const gCoord& c) {return c.Compare(d)< 0;}
__inline gCoord operator+ (short          i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (short          i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (short          i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (short          i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(short          i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(short          i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(short          i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(short          i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (short          i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (short          i, const gCoord& c) {return c.Compare(i)< 0;}
__inline gCoord operator+ (int            i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (int            i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (int            i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (int            i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(int            i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(int            i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(int            i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(int            i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (int            i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (int            i, const gCoord& c) {return c.Compare(i)< 0;}
__inline gCoord operator+ (long           i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (long           i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (long           i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (long           i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(long           i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(long           i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(long           i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(long           i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (long           i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (long           i, const gCoord& c) {return c.Compare(i)< 0;}
__inline gCoord operator+ (unsigned short i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (unsigned short i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (unsigned short i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (unsigned short i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(unsigned short i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(unsigned short i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(unsigned short i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(unsigned short i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (unsigned short i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (unsigned short i, const gCoord& c) {return c.Compare(i)< 0;}
__inline gCoord operator+ (unsigned int   i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (unsigned int   i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (unsigned int   i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (unsigned int   i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(unsigned int   i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(unsigned int   i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(unsigned int   i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(unsigned int   i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (unsigned int   i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (unsigned int   i, const gCoord& c) {return c.Compare(i)< 0;}
__inline gCoord operator+ (unsigned long  i, const gCoord& c) {return i+c.Coord;}
__inline gCoord operator- (unsigned long  i, const gCoord& c) {return i-c.Coord;}
__inline gCoord operator* (unsigned long  i, const gCoord& c) {return i*c.Coord;}
__inline gCoord operator/ (unsigned long  i, const gCoord& c) {return i/c.Coord;}
__inline bool   operator==(unsigned long  i, const gCoord& c) {return c.Compare(i)==0;}
__inline bool   operator!=(unsigned long  i, const gCoord& c) {return c.Compare(i)!=0;}
__inline bool   operator<=(unsigned long  i, const gCoord& c) {return c.Compare(i)>=0;}
__inline bool   operator>=(unsigned long  i, const gCoord& c) {return c.Compare(i)<=0;}
__inline bool   operator< (unsigned long  i, const gCoord& c) {return c.Compare(i)> 0;}
__inline bool   operator> (unsigned long  i, const gCoord& c) {return c.Compare(i)< 0;}

#ifdef Assert
static struct gCoordTester : Tester {
  gCoordTester() {
    Assert(MinMax(0,1,2)==1);
    Assert(MinMax(1,0,2)==1);
    Assert(MinMax(0,2,1)==1);
    gCoord a(0.00011);
    Assert(a.Coord==0.00011); // double:double comparisons
    Assert(a.Coord!=0.0001 );
    Assert(a==0.00011); // gCoord:double Comparisons
    Assert(a==0.0001 );
    Assert(0.00011==a); // double:gCoord Comparisons
    Assert(0.0001 ==a);
    gCoord b(0.00012);
    gCoord c(0.00022);
    Assert(a==a); // gCoord:gCoord Comparisons
    Assert(a==b);
    Assert(b==a);
    Assert(a!=c);
    Assert(c!=a);
    Assert(c-0.00011==a); // gCoord:double subtraction
    Assert(0.00022-a==a); // double:gCoord subtraction
    Assert(c-a==a);       // gCoord:gCoord subtraction
    Assert(c-0.0001==a);  // gCoord:double subtraction ensure Tolerance works
    Assert(0.0002-a==a);  // double:gCoord subtraction ensure Tolerance works
    Assert(a.RawReport()==0.0001);
    Assert((1-a)==0.9998); // 0.99989
    Assert((1-a).RawReport()==0.9999);
    // gCoord:double
    a=2.00034; // Assignment
    Assert(a.Coord==2.00034);
    Assert((a+=1.).Coord==(2.00034+1.));
    Assert((a-=1.).Coord==(2.00034));
    Assert((a*=2.).Coord==(2.00034*2.));
    Assert((a/=2.).Coord==(2.00034));
    Assert((a+1.).Coord==(2.00034+1.));
    Assert((a-1.).Coord==(2.00034-1.));
    Assert((a*2.).Coord==(2.00034*2.));
    Assert((a/2.).Coord==(2.00034/2.));
    Assert( (a<=2.00034)); // <= and >= test the = bit:
    Assert( (a>=2.00034));
    Assert( (a<=2.00034+0.0001));
    Assert(!(a<=2.00034-0.0001));
    Assert( (a>=2.00034-0.0001));
    Assert(!(a>=2.00034+0.0001));
    Assert( (a< 2.00034+0.0001));
    Assert(!(a< 2.00034-0.0001));
    Assert( (a> 2.00034-0.0001));
    Assert(!(a> 2.00034+0.0001));
    // gCoord:int
    a=200034; // Assignment
    Assert(a.Coord==200034);
    Assert((a+=1).Coord==(200034+1));
    Assert((a-=1).Coord==(200034));
    Assert((a*=2).Coord==(200034*2));
    Assert((a/=2).Coord==(200034));
    Assert((a+1).Coord==(200034+1));
    Assert((a-1).Coord==(200034-1));
    Assert((a*2).Coord==(200034*2));
    Assert((a/2).Coord==(200034/2));
    Assert( (a<=200034)); // <= and >= test the = bit:
    Assert( (a>=200034));
    Assert( (a<=200034+1));
    Assert(!(a<=200034-1));
    Assert( (a>=200034-1));
    Assert(!(a>=200034+1));
    Assert( (a< 200034+1));
    Assert(!(a< 200034-1));
    Assert( (a> 200034-1));
    Assert(!(a> 200034+1));
    // gCoord
    a=2.00034; // Assignment
    b=2.00034;
    Assert(a.Coord==b);
    Assert((a+=1.).Coord==(b+1.));
    Assert((a-=1.).Coord==(b));
    Assert((a*=2.).Coord==(b*2.));
    Assert((a/=2.).Coord==(b));
    Assert((a+1.).Coord==(b+1.));
    Assert((a-1.).Coord==(b-1.));
    Assert((a*2.).Coord==(b*2.));
    Assert((a/2.).Coord==(b/2.));
    Assert( (a<=b)); // <= and >= test the = bit:
    Assert( (a>=b));
    Assert( (a<=b+0.0001));
    Assert(!(a<=b-0.0001));
    Assert( (a>=b-0.0001));
    Assert(!(a>=b+0.0001));
    Assert( (a< b+0.0001));
    Assert(!(a< b-0.0001));
    Assert( (a> b-0.0001));
    Assert(!(a> b+0.0001));
    Assert(-a==-2.00034);
    Assert(a==-(-a));
    Assert(a);
    b=0;
    Assert(!b);
    Assert(a.sgn()==1);
    Assert(b.sgn()==0);
    Assert((-a).sgn()==-1);
    // double:gCoord
    b=2.;
    double d=a;
    Assert(d==2.00034);
    Assert((d+=a)==(2.00034*2.));
    Assert((d-=a)==(2.00034));
    Assert((d*=b)==(2.00034*2.));
    Assert((d/=b)==(2.00034));
    Assert((d+a)==(2.00034*2.));
    Assert((d-a)==(0.));
    Assert((d*b)==(2.00034*2.));
    Assert((d/b)==(2.00034/2.));
    Assert( (2.00034       <=a)); // <= and >= test the = bit:
    Assert( (2.00034       >=a));
    Assert( (2.00034+0.0001>=a));
    Assert(!(2.00034-0.0001>=a));
    Assert( (2.00034-0.0001<=a));
    Assert(!(2.00034+0.0001<=a));
    Assert( (2.00034+0.0001> a));
    Assert(!(2.00034-0.0001> a));
    Assert( (2.00034-0.0001< a));
    Assert(!(2.00034+0.0001< a));
    // double:gCoord
    a=200034; // Assignment
    b=2;
    int i=a;
    Assert(i==200034);
    Assert((i+=a)==(200034*2));
    Assert((i-=a)==(200034));
    Assert((i*=b)==(200034*2));
    Assert((i/=b)==(200034));
    Assert((i+a)==(200034*2));
    Assert((i-a)==(0));
    Assert((i*b)==(200034*2));
    Assert((i/b)==(200034/2));
    Assert( (200034  <=a)); // <= and >= test the = bit:
    Assert( (200034  >=a));
    Assert( (200034+1>=a));
    Assert(!(200034-1>=a));
    Assert( (200034-1<=a));
    Assert(!(200034+1<=a));
    Assert( (200034+1> a));
    Assert(!(200034-1> a));
    Assert( (200034-1< a));
    Assert(!(200034+1< a));
  }
} gCoordTester;
#endif // def Assert

#endif // ndef gCoordh
