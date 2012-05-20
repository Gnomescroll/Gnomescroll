#pragma once

#include "global.hpp"
#include "twister.hpp"

//#include <string.h>

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

#if 1
#define gCoordTolerance (gCoord::GetTolerance()) // <<<< This is the whole point of the gCoord struct.

class gCoord 
{ // A quantised double. Quantisation Tolerance is handled in the sgn function:

  public: // Older versions won't let NewTolerance access Tolerance() and you can't friend a sub-structure

  static double Tolerance(double NewTolerance=0) 
  {
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
   ~NewTolerance() { gCoord::Tolerance(OldTolerance); }
  };

  double Coord; // <<<<<<<<<<<<<<<<<<< The value being controlled
  gCoord() {}

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
#endif