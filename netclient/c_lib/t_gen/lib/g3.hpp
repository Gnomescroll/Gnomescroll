#pragma once

#include "gCoord.hpp"
//#include <stdio.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>> g3D <<<<<<<<<<<<<<<<<<<<<<<<<<<<
Lots of 3D packages use simple double* parameters to pass the coordinates around.
When you need to copy to a double* from a g3D or derived class you can use the following shortcut:
  void Myfunction(g3Point in, double* out) {*((g3D*)out)=in;}
this works because none of the primitives have Virtual Functions. */

#if 0
struct g3D { // 3 Doubles (Used as x 3 Dimensional Point or Vector)
  gCoord x,y,z;
  g3D() {}
  //g3D(double RandMin, double RandMax) : x(RandMin,RandMax), y(RandMin,RandMax), z(RandMin,RandMax) {} // Random Values
  g3D(double x, double y, double z) : x(x), y(y), z(z) {}
  //g3D(const double* DDD)                 {Set(DDD);}

  //void Set(const double* DDD)            {x=*DDD++; y=*DDD++; z=*DDD;}
  //void Set(double X, double Y, double Z) {x=X; y=Y; z=Z;}
#if 0

  void Set(const g3D& DDD)               {x=DDD.x; y=DDD.y; z=DDD.z;}
  bool IsZero()                    const {return !x && !y && !z;}
  void ToArray(double* DDD)        const {*DDD++=x; *DDD++=y; *DDD=z;}
  void ToFloatArray(float* FFF)    const {*FFF++=static_cast<float>(x.Coord); *FFF++=static_cast<float>(y.Coord); *FFF=static_cast<float>(z.Coord);}
#endif


  //g3D& operator =(const g3D& DDD)        {Set(DDD);                       return *this;}

#if 0
  g3D& operator+=(const g3D& DDD)        {  x+=DDD.x; y+=DDD.y; z+=DDD.z; return *this;}
  g3D& operator-=(const g3D& DDD)        {  x-=DDD.x; y-=DDD.y; z-=DDD.z; return *this;}
  g3D& operator*=(double d)              {              x*=d; y*=d; z*=d; return *this;}
  g3D& operator/=(double d)              { x/=d; y/=d; z/=d; return *this;}
  g3D  operator- ()                const {              return g3D(-x, -y, -z);}
  g3D  operator+ (const g3D& DDD)  const {              return g3D(x+DDD.x, y+DDD.y, z+DDD.z);}
  g3D  operator- (const g3D& DDD)  const {              return g3D(x-DDD.x, y-DDD.y, z-DDD.z);}
  g3D  operator* (double d)        const {              return g3D(x*d, y*d, z*d);}
  g3D  operator/ (double d)        const { return g3D(x/d, y/d, z/d);}

  operator double*()                     {return &x.Coord;}
  operator const double*()         const {return &x.Coord;}
#endif

#if 0
  bool operator==(const g3D& DDD)  const {return Compare(DDD)==0;}
  bool operator!=(const g3D& DDD)  const {return Compare(DDD)!=0;}
  bool operator<=(const g3D& DDD)  const {return Compare(DDD)<=0;}
  bool operator>=(const g3D& DDD)  const {return Compare(DDD)>=0;}
  bool operator< (const g3D& DDD)  const {return Compare(DDD)< 0;}
  bool operator> (const g3D& DDD)  const {return Compare(DDD)> 0;}
#endif

#if 0
  bool SameX     (const g3D& DDD)  const {return DDD.x==x;}
  bool SameY     (const g3D& DDD)  const {return DDD.y==y;}
  bool SameZ     (const g3D& DDD)  const {return DDD.z==z;}
  int Compare(const g3D& DDD)      const { // For Sorting
    int Result=z.Compare(DDD.z); if(Result) return Result;
        Result=y.Compare(DDD.y); if(Result) return Result;
    return     x.Compare(DDD.x);
  }
#endif
  //CString Report() const {CString S; S.Format("(%g, %g, %g)",x.RawReport(),y.RawReport(),z.RawReport()); return S;}
};

#if 0
inline g3D operator* (gCoord c, const g3D& DDD) {return g3D(DDD.x*c, DDD.y*c, DDD.z*c);}
inline g3D operator* (double d, const g3D& DDD) {return g3D(DDD.x*d, DDD.y*d, DDD.z*d);}
inline g3D operator* (int    i, const g3D& DDD) {return g3D(DDD.x*i, DDD.y*i, DDD.z*i);}
#endif

#endif

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3Vector <<<<<<<<<<<<<<<<<<<<<<<<<<<<

#if 1
//struct g3Vector : public g3D { // 3D Vector
struct g3Vector
{
  gCoord x,y,z;
  g3Vector() {}
  //g3Vector(const g3D& V)                 : g3D(V)     {}
  g3Vector(double X, double Y, double Z) {x=X; y=Y; z=Z;}
  void Set(double X, double Y, double Z) {x=X; y=Y; z=Z;}

  //g3Vector(const double* DDD)            : g3D(DDD)   {}
  //gCoord        Dot(const g3Vector& V)                    const {return x*V.x+y*V.y+z*V.z;}                               // =|a||b|cos(t)
  //gCoord   CrossDot(const g3Vector& C, const g3Vector& D) const {return Cross(C).Dot(D);} // scalar triple product: |D.(VxC)| = Volume of parallelepiped.
  //g3Vector Cross   (const g3Vector& V)                    const {return g3Vector(y*V.z-z*V.y, z*V.x-x*V.z, x*V.y-y*V.x);} // =|a||b|sin(t)
  //void     MakeAxis(const g3Vector& V)                          {*this =Cross(V).Cross(V).Reverse().Normalised();} // Move this (on the plane holding both Vectors) to be perpendicular to the given Vector. Finds the perp to both, then uses that to get back (Vector Triple Product).
  //g3Vector GetAxis (const g3Vector& V)                    const {return Cross(V).Cross(V).Reverse().Normalised();} // Move this (on the plane holding both Vectors) to be perpendicular to the given Vector. Finds the perp to both, then uses that to get back (Vector Triple Product).
  // vector triple product: Ax(BxC)=(A.C)B-(A.B)C  (AxB)xC=(C.A)B-(C.B)A
  //bool   IsHorizontal () const {return !z;}
  //bool   IsVertical   () const {return (!x)&&(!y);}
  //bool   IsNull       () const {return IsZero();}
  gCoord GetLength    () const {return sqrt(GetL2());} // Euclidean (shortest line)
  gCoord GetL2        () const {return x*x+y*y+z*z;} // The length squared. Saves the slow Square Root for some calculations.
  gCoord GetManhattan () const {double d=x.abs()+y.abs()+z.abs(); return d*d;} // The length of the distance in axial directions (as if travelling around a city).
  gCoord GetChebychev () const {double d=Max(x.abs(),Max(y.abs(),z.abs())); return d*d;} // The length of the longest Axial journey.
  gCoord GetQuadratic () const {return GetL2()+x*y+x*z+y*z;} // The sum of everything multiplied by everything else!
  gCoord GetMinkowski(double p) const {return pow(pow(x.abs(), p) + pow(y.abs(), p) + pow(z.abs(), M_E), 1.0/p);} // General (and slowest) case. p=1 is Chebychev, p=2 is the Euclidean and P at infinity is Chebychev!
  gCoord GetMinkowski4() const {return pow(x*x*x*x+y*y*y*y+z*z*z*z,0.25);}                       // Same as Minkowski(4);
  gCoord GetMinkowski5() const {double d=sqrt(x.abs())+sqrt(y.abs())+sqrt(z.abs()); return d*d;} // Same as Minkowski(0.5);
  //gCoord GetArea      () const {return 2*(x*y+x*z+y*z);} // The Surface area of the axially-aligned cuboid this vector fills.
  //gCoord GetVolume    () const {return x*y*z;}

#if 0
  g3Vector& SetLength(double Length) {Normalise(); return operator*=(Length);}
  g3Vector Normalised(double Tolerance=0) const { // If Tolerance is non-zero (like around 1e-8) and the Vector is "nearly" pointing at an axis, make it really point there!
    g3Vector V(0,0,0);
    double Length=GetLength();
    if(Length) V=*this/(Length);
    if(Tolerance==0) return V; // Snap to Axis:
    //ASSERT((Tolerance>0)&&(Tolerance<1));
         if(fabs(V.x)>=(1-Tolerance)) V.Set(V.x.sgn(),0,0);
    else if(fabs(V.y)>=(1-Tolerance)) V.Set(0,V.y.sgn(),0);
    else if(fabs(V.z)>=(1-Tolerance)) V.Set(0,0,V.z.sgn());
    return V;
  }
#endif

/* Used to look for two small ones rather than one big one:
    switch(((fabs(V.x)<(1-Tolerance)) ? 0 : 4) // Create a BitFlag
         | ((fabs(V.y)<(1-Tolerance)) ? 0 : 2)
         | ((fabs(V.z)<(1-Tolerance)) ? 0 : 1)) {
      case 3: V.Set(V.x.sgn(),0,0); break;
      case 5: V.Set(0,V.y.sgn(),0); break;
      case 6: V.Set(0,0,V.z.sgn()); break;
    }
    return V;
  }*/

#if 0
  g3Vector& Normalise  (double Tolerance=0)      {Set(Normalised(Tolerance)); return *this;} // If Tolerance is non-zero and the Vector is "nearly" pointing at an axis, make it really point there!
  g3Vector& Reverse    ()                        {Set(-x,-y,-z);              return *this;}
  g3Vector& operator*= (double d)                {g3D::operator*=(d);         return *this;}
#endif


#if 0
  g3Vector& operator/= (double d)                {g3D::operator/=(d);         return *this;}
  g3Vector& operator=  (const g3Vector& V)       {g3D::operator= (V);         return *this;}
  g3Vector  operator+  (const g3Vector& V) const {return g3Vector(x+V.x, y+V.y, z+V.z);}
  g3Vector  operator-  (const g3Vector& V) const {return g3Vector(x-V.x, y-V.y, z-V.z);}
  g3Vector  operator-  ()                  const {return g3Vector(-x, -y,  -z  );}
  g3Vector  operator*  (double d)          const {return g3Vector(x*d, y*d, z*d);}
  g3Vector  operator/  (double d)          const {return g3Vector(x/d, y/d, z/d);}

  g3Vector  Reversed   ()                  const {return g3Vector(-x,-y,-z);}
  g3Vector  ProjectedTo(const g3Vector& V, bool Normalised=false) const {return Normalised ? V*Dot(V) : V*(Dot(V)/V.GetL2());} // Quicker if you know that V is normalised!
  bool      SameX      (const g3Vector& V) const {return V.x==x;}
  bool      SameY      (const g3Vector& V) const {return V.y==y;}
  bool      SameZ      (const g3Vector& V) const {return V.z==z;}
  bool      Colinear   (const g3Vector& V) const {return ((V.x==x) && (V.y==y) && (V.z==z)) || ((V.x==-x) && (V.y==-y) && (V.z==-z));} // (Faster than abs(Dot)==1) Assumes both vectors are Normalised
#endif 
/* Get an axis from a Z-axis. The returned axis will be Horizontal (perpendicular to the Global Z-Axis).
The Cross Product of dz with the Global Z-axis will give us a local axis as long as dz isn't coincident with the Global Z-axis...
If dz _is_ coincident with the Global Z-axis, the Global X-axis is returned (or the Global Y-Axis if HorizontalAxis() is yHorizontal).
Otherwise the Cross Product of the Global Z-axis with dz is returned: (0,0,1) ^ Axis (^ is Cross Product):
Cross Product (y*dz.z-z*dz.y, z*dz.x-x*dz.z, x*dz.y-y*dz.x)
if x and y are both zero, this reduces to  (-dz.y, dz.x, 0)
HorizontalAxisType helps describe how Axes should be created.
When given a Z-Axis, you can specify that X or Y are to be horizontal.
Looking down the local horizontal axis to the origin, local Z-Axis is either anti-clockwise (default) or clockwise from Vertical (rotating around the horizontal Axis). */

#if 0
  enum     HorizontalAxisType {GetHorizontal,xHorizontal,yHorizontal,xHorizontalCW,yHorizontalCW};
  static   HorizontalAxisType HorizontalAxis(HorizontalAxisType Axis=GetHorizontal) {
    static HorizontalAxisType Horizontal=xHorizontal;
    if(Axis!=GetHorizontal)   Horizontal=Axis;
    return Horizontal;
  }
  static   bool Try4Axis(int Try=2) {
    static bool Try4Axis=false;   // Initialize
    if(Try!=2)  Try4Axis=Try!=0; // Set
    return      Try4Axis;       // Get
  }
  g3Vector GetPerpendicular() const {
    g3Vector Result(-y,x,0);
    if(!Result.IsNull()) return Result.Normalised();
    switch(HorizontalAxis()) {
      default:
      case xHorizontal:   return g3Vector( 1, 0,0);
      case xHorizontalCW: return g3Vector(-1, 0,0);
      case yHorizontal:   return g3Vector( 0, 1,0);
      case yHorizontalCW: return g3Vector( 0,-1,0);
    }
  }
  void GetAxes(g3Vector& dx, g3Vector& dy) const { // Set dx and dy as Vectors perpendicular to this and each other.
    switch(HorizontalAxis()) {
      case xHorizontalCW:
      case xHorizontal: {
        dx=GetPerpendicular();
        dy=Cross(dx);
        break;
      }
      case yHorizontalCW:
      case yHorizontal: {
        dy=GetPerpendicular();
        dx=dy.Cross(*this);
        break;
    } }
    switch(HorizontalAxis()) {
      case xHorizontalCW:
      case yHorizontalCW: {
        dx.Reverse();
        dy.Reverse();
    } }
    if(Try4Axis()) { // If the horizontal axis is anti-parallel with it's Global equivalent, make it parallel:
      switch(HorizontalAxis()) {
        default:
        case xHorizontal:
        case xHorizontalCW: if(dx.x==-1) {dx.x=1; dy.y=-dy.y;} break;
        case yHorizontal:   ;
        case yHorizontalCW: if(dy.y==-1) {dx.x=-dx.x; dy.y=1;} break;
    } }
  }
#endif
};
#endif 