// g3.h

#ifndef g3h
#define g3h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gCoord.h"
#include <stdio.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>> g3D <<<<<<<<<<<<<<<<<<<<<<<<<<<<
Lots of 3D packages use simple double* parameters to pass the coordinates around.
When you need to copy to a double* from a g3D or derived class you can use the following shortcut:
  void Myfunction(g3Point in, double* out) {*((g3D*)out)=in;}
this works because none of the primitives have Virtual Functions. */
struct g3D { // 3 Doubles (Used as x 3 Dimensional Point or Vector)
  gCoord x,y,z;
  g3D() {}
  g3D(double RandMin, double RandMax) : x(RandMin,RandMax), y(RandMin,RandMax), z(RandMin,RandMax) {} // Random Values
  g3D(double x, double y, double z) : x(x), y(y), z(z) {}
  g3D(const double* DDD)                 {Set(DDD);}
  void Set(double X, double Y, double Z) {x=X; y=Y; z=Z;}
  void Set(const double* DDD)            {x=*DDD++; y=*DDD++; z=*DDD;}
  void Set(const g3D& DDD)               {x=DDD.x; y=DDD.y; z=DDD.z;}
  bool IsZero()                    const {return !x && !y && !z;}
  void ToArray(double* DDD)        const {*DDD++=x; *DDD++=y; *DDD=z;}
  void ToFloatArray(float* FFF)    const {*FFF++=static_cast<float>(x.Coord); *FFF++=static_cast<float>(y.Coord); *FFF=static_cast<float>(z.Coord);}
  g3D& operator =(const g3D& DDD)        {Set(DDD);                       return *this;}
  g3D& operator+=(const g3D& DDD)        {  x+=DDD.x; y+=DDD.y; z+=DDD.z; return *this;}
  g3D& operator-=(const g3D& DDD)        {  x-=DDD.x; y-=DDD.y; z-=DDD.z; return *this;}
  g3D& operator*=(double d)              {              x*=d; y*=d; z*=d; return *this;}
  g3D& operator/=(double d)              { x/=d; y/=d; z/=d; return *this;}
  g3D  operator- ()                const {              return g3D(-x, -y, -z);}
  g3D  operator+ (const g3D& DDD)  const {              return g3D(x+DDD.x, y+DDD.y, z+DDD.z);}
  g3D  operator- (const g3D& DDD)  const {              return g3D(x-DDD.x, y-DDD.y, z-DDD.z);}
  g3D  operator* (double d)        const {              return g3D(x*d, y*d, z*d);}
  g3D  operator/ (double d)        const { return g3D(x/d, y/d, z/d);}
#ifdef g2h
  g2D GetEuclidean()               const { return g2D(x/z, y/z);} // From Homogeneous coordinates
#endif
  operator double*()                     {return &x.Coord;}
  operator const double*()         const {return &x.Coord;}
  bool operator==(const g3D& DDD)  const {return Compare(DDD)==0;}
  bool operator!=(const g3D& DDD)  const {return Compare(DDD)!=0;}
  bool operator<=(const g3D& DDD)  const {return Compare(DDD)<=0;}
  bool operator>=(const g3D& DDD)  const {return Compare(DDD)>=0;}
  bool operator< (const g3D& DDD)  const {return Compare(DDD)< 0;}
  bool operator> (const g3D& DDD)  const {return Compare(DDD)> 0;}
  bool SameX     (const g3D& DDD)  const {return DDD.x==x;}
  bool SameY     (const g3D& DDD)  const {return DDD.y==y;}
  bool SameZ     (const g3D& DDD)  const {return DDD.z==z;}
  int Compare(const g3D& DDD)      const { // For Sorting
    int Result=z.Compare(DDD.z); if(Result) return Result;
        Result=y.Compare(DDD.y); if(Result) return Result;
    return     x.Compare(DDD.x);
  }
  //CString Report() const {CString S; S.Format("(%g, %g, %g)",x.RawReport(),y.RawReport(),z.RawReport()); return S;}
};

inline g3D operator* (gCoord c, const g3D& DDD) {return g3D(DDD.x*c, DDD.y*c, DDD.z*c);}
inline g3D operator* (double d, const g3D& DDD) {return g3D(DDD.x*d, DDD.y*d, DDD.z*d);}
inline g3D operator* (int    i, const g3D& DDD) {return g3D(DDD.x*i, DDD.y*i, DDD.z*i);}

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3Vector <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g3Vector : public g3D { // 3D Vector
  g3Vector() {}
  g3Vector(const g3D& V)                 : g3D(V)     {}
  g3Vector(double x, double y, double z) : g3D(x,y,z) {}
  g3Vector(const double* DDD)            : g3D(DDD)   {}
  gCoord        Dot(const g3Vector& V)                    const {return x*V.x+y*V.y+z*V.z;}                               // =|a||b|cos(t)
  gCoord   CrossDot(const g3Vector& C, const g3Vector& D) const {return Cross(C).Dot(D);} // scalar triple product: |D.(VxC)| = Volume of parallelepiped.
  g3Vector Cross   (const g3Vector& V)                    const {return g3Vector(y*V.z-z*V.y, z*V.x-x*V.z, x*V.y-y*V.x);} // =|a||b|sin(t)
  void     MakeAxis(const g3Vector& V)                          {*this =Cross(V).Cross(V).Reverse().Normalised();} // Move this (on the plane holding both Vectors) to be perpendicular to the given Vector. Finds the perp to both, then uses that to get back (Vector Triple Product).
  g3Vector GetAxis (const g3Vector& V)                    const {return Cross(V).Cross(V).Reverse().Normalised();} // Move this (on the plane holding both Vectors) to be perpendicular to the given Vector. Finds the perp to both, then uses that to get back (Vector Triple Product).
  // vector triple product: Ax(BxC)=(A.C)B-(A.B)C  (AxB)xC=(C.A)B-(C.B)A
  bool   IsHorizontal () const {return !z;}
  bool   IsVertical   () const {return (!x)&&(!y);}
  bool   IsNull       () const {return IsZero();}
  gCoord GetLength    () const {return sqrt(GetL2());} // Euclidean (shortest line)
  gCoord GetL2        () const {return x*x+y*y+z*z;} // The length squared. Saves the slow Square Root for some calculations.
  gCoord GetManhattan () const {double d=x.abs()+y.abs()+z.abs(); return d*d;} // The length of the distance in axial directions (as if travelling around a city).
  gCoord GetChebychev () const {double d=Max(x.abs(),Max(y.abs(),z.abs())); return d*d;} // The length of the longest Axial journey.
  gCoord GetQuadratic () const {return GetL2()+x*y+x*z+y*z;} // The sum of everything multiplied by everything else!
  gCoord GetMinkowski(double p) const {return pow(pow(x.abs(), p) + pow(y.abs(), p) + pow(z.abs(), M_E), 1.0/p);} // General (and slowest) case. p=1 is Chebychev, p=2 is the Euclidean and P at infinity is Chebychev!
  gCoord GetMinkowski4() const {return pow(x*x*x*x+y*y*y*y+z*z*z*z,0.25);}                       // Same as Minkowski(4);
  gCoord GetMinkowski5() const {double d=sqrt(x.abs())+sqrt(y.abs())+sqrt(z.abs()); return d*d;} // Same as Minkowski(0.5);
  gCoord GetArea      () const {return 2*(x*y+x*z+y*z);} // The Surface area of the axially-aligned cuboid this vector fills.
  gCoord GetVolume    () const {return x*y*z;}
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
  g3Vector& Normalise  (double Tolerance=0)      {Set(Normalised(Tolerance)); return *this;} // If Tolerance is non-zero and the Vector is "nearly" pointing at an axis, make it really point there!
  g3Vector& Reverse    ()                        {Set(-x,-y,-z);              return *this;}
  g3Vector  Reversed   ()                  const {return g3Vector(-x,-y,-z);}
  g3Vector& operator*= (double d)                {g3D::operator*=(d);         return *this;}
  g3Vector& operator/= (double d)                {g3D::operator/=(d);         return *this;}
  g3Vector& operator=  (const g3Vector& V)       {g3D::operator= (V);         return *this;}
  g3Vector  operator+  (const g3Vector& V) const {return g3Vector(x+V.x, y+V.y, z+V.z);}
  g3Vector  operator-  (const g3Vector& V) const {return g3Vector(x-V.x, y-V.y, z-V.z);}
  g3Vector  operator-  ()                  const {return g3Vector(-x, -y,  -z  );}
  g3Vector  operator*  (double d)          const {return g3Vector(x*d, y*d, z*d);}
  g3Vector  operator/  (double d)          const {return g3Vector(x/d, y/d, z/d);}
  g3Vector  ProjectedTo(const g3Vector& V, bool Normalised=false) const {return Normalised ? V*Dot(V) : V*(Dot(V)/V.GetL2());} // Quicker if you know that V is normalised!
  bool      SameX      (const g3Vector& V) const {return V.x==x;}
  bool      SameY      (const g3Vector& V) const {return V.y==y;}
  bool      SameZ      (const g3Vector& V) const {return V.z==z;}
  bool      Colinear   (const g3Vector& V) const {return ((V.x==x) && (V.y==y) && (V.z==z)) || ((V.x==-x) && (V.y==-y) && (V.z==-z));} // (Faster than abs(Dot)==1) Assumes both vectors are Normalised
/* Get an axis from a Z-axis. The returned axis will be Horizontal (perpendicular to the Global Z-Axis).
The Cross Product of dz with the Global Z-axis will give us a local axis as long as dz isn't coincident with the Global Z-axis...
If dz _is_ coincident with the Global Z-axis, the Global X-axis is returned (or the Global Y-Axis if HorizontalAxis() is yHorizontal).
Otherwise the Cross Product of the Global Z-axis with dz is returned: (0,0,1) ^ Axis (^ is Cross Product):
Cross Product (y*dz.z-z*dz.y, z*dz.x-x*dz.z, x*dz.y-y*dz.x)
if x and y are both zero, this reduces to  (-dz.y, dz.x, 0)
HorizontalAxisType helps describe how Axes should be created.
When given a Z-Axis, you can specify that X or Y are to be horizontal.
Looking down the local horizontal axis to the origin, local Z-Axis is either anti-clockwise (default) or clockwise from Vertical (rotating around the horizontal Axis). */
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
};

#ifdef Assert
static struct g3VectorTester : Tester {
  g3VectorTester() {
    OperatorTester();
    NormaliseTester();
  }
  void OperatorTester() {
    g3Vector v1(1,2,3);
    g3Vector v2(1,2,4);
    Assert(v1==v1);
    Assert(v1<=v1);
    Assert(v1>=v1);
    Assert(v1==g3Vector(1,2,3));
    Assert(v1!=v2);
    Assert(v1< v2);
    Assert(v1<=v2);
    Assert(v2> v1);
    Assert(v2>=v1);
  }
  void NormaliseTester() {
    g3Vector v1(100, 2, 3);
    v1.Normalise(0.0006); //Should just Normalise
    Assert(v1.Report()=="(0.9994, 0.02, 0.03)");
    v1.Normalise(0.0007); //Should normalise and Snap to X Axis
    Assert(v1.Report()=="(1, 0, 0)");
    v1.Set(-100, 2, 3);
    v1.Normalise(0.0006); //Should just Normalise
    Assert(v1.Report()=="(-0.9994, 0.02, 0.03)");
    v1.Normalise(0.0007); //Should normalise and Snap to X Axis
    Assert(v1.Report()=="(-1, 0, 0)");
  }
} g3VectorTester;
#endif

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3Point <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g3Point : public g3D { // 3D Point Vertex
  g3Point() {}
  g3Point(const g3D& P)                 : g3D(P)     {}
  g3Point(double x, double y, double z) : g3D(x,y,z) {}
  g3Point(const double* DDD)            : g3D(DDD)   {}
  g3Point& operator =(const g3Point&  P)         {        x =P.x; y =P.y; z =P.z; return *this;}
  g3Point& operator+=(const g3Vector& V)         {        x+=V.x; y+=V.y; z+=V.z; return *this;}
  g3Point& operator-=(const g3Vector& V)         {        x-=V.x; y-=V.y; z-=V.z; return *this;}
  g3Point& operator*=(const double    d)         {              x*=d; y*=d; z*=d; return *this;}
  g3Point& operator/=(const double    d)         { x/=d; y/=d; z/=d; return *this;}
  g3Point  operator+ (const g3Vector& V)   const {return g3Point (x+V.x, y+V.y, z+V.z);}
  g3Vector operator- (const g3Point&  P)   const {return g3Vector(x-P.x, y-P.y, z-P.z);}
  g3Point  operator* (const double    d)   const {              return g3Point(x*d, y*d, z*d);}
  g3Point  operator/ (const double    d)   const { return g3Point(x/d, y/d, z/d);}
  bool     operator==(const g3Point&  P)   const {return Compare(P)==0;}
  bool     operator!=(const g3Point&  P)   const {return Compare(P)!=0;}
           operator g3Vector()             const {return g3Vector(x,y,z);} // Points can be considered a vector from the Global Origin
  bool     SameX     (const g3Point&  P)   const {return P.x==x;}
  bool     SameY     (const g3Point&  P)   const {return P.y==y;}
  bool     SameZ     (const g3Point&  P)   const {return P.z==z;}
  bool     IsOrigin  ()                    const {return IsZero();}
  void     Move(double dx, double dy, double dz) {x+=dx; y+=dy; z+=dz;}
  g3Point  OnViewPlane(g3Vector& Axis) const {return *this-Axis*Axis.Dot(*this);} // Returns the Projected point to a plane through the Global Origin having the given Normal Axis.
  static g3Point FromSpherical(double Radius, double Theta, double Phi) {
    double SinTheta,CosTheta;  sincos(Theta, &SinTheta, &CosTheta);
    double SinPhi  ,CosPhi;    sincos(  Phi, &SinPhi,   &CosPhi);
    return g3Point(Radius*CosTheta*SinPhi, Radius*SinTheta*SinPhi, Radius*CosPhi);
  }
  void ToSpherical(double& Radius, double& Theta, double& Phi) const {Radius=GetRadius(); Theta=GetTheta(); Phi=GetPhi();}
  gCoord GetRadius() const {return sqrt(x*x+y*y+z*z);}
  gCoord GetTheta () const {return atan2(y,x);}
  gCoord GetPhi   () const {return acos(z/GetRadius());}
  gCoord DistanceTo(const g3Point& E) const {return (E-*this).GetLength();} // For signed Distances use g3PV::GetDisplacement();
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3Cr <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g3Cr { // Center and radius (Sphere)
  g3Point C;
  gCoord  r;
  g3Cr() {}
  explicit g3Cr(bool zero) {if(zero) Zero();}
  g3Cr(const double* Cr)                       : C(   Cr), r(Cr[2]) {}
  g3Cr(double x, double y, double z, double r) : C(x,y,z), r(r) {}
  g3Cr(const g3Point& C, double r)             : C(    C), r(r) {}
  g3Cr(const g3Point& S, const g3Point&  M, const g3Point& E) {Set(S,M,E);}
  g3Cr(const g3Point& S, const g3Vector& V, const g3Point& E) {Set(S,V,E);}
  void Zero()                            {memset(this,0,sizeof(*this));}
  bool IsZero() const {return C.IsZero() && !r;}
  void Set(const g3Point& _C, double _r) {C=_C; r=_r;}
  void Set(double x, double y, double z, double Radius) {C.Set(x,y,z); r=Radius;}
  g3Vector Set(const g3Point& S, const g3Vector& V, const g3Point& E) { // Two point Arc with Start Tangent. Returns Axis
    g3Vector SE(E-S); // Start to End Chord
    g3Vector Axis(V.Cross(SE).Normalised());
    if(Axis.IsNull()) Set(S+SE/2, 0); // Linear
    else{
      g3Vector SC(Axis.Cross(V).Normalised()); // Start to Center Direction
      r=(SE.GetLength()/2) / SE.Normalised().Dot(SC); // Hypotenuse=Adjacent/cos(Theta) - Dot gives cos(Theta).
      C=S+SC*r;
    }
    return Axis;
  }
  g3Vector Set(const g3Point& S, const g3Point& M, const g3Point& E) { // 3 Point Arc. Returns Axis
    g3Vector MS(S-M);
    g3Vector ME(E-M);
    double MS2=MS.GetL2();
    double ME2=ME.GetL2();
    double MSE=MS.Dot(ME);
    MS*=ME2*(MS2-MSE);
    ME*=MS2*(ME2-MSE);
    double D=2.*(MS2*ME2-MSE*MSE);
    if(D) {
      C=M+(MS+ME)/D;
      r=sqrt((MS2*ME2*(MS2+ME2-2*MSE))/(2*D));
      return ME.Cross(MS).Normalised(); // CS^CE = -(MS^ME) = ME^MS
    }else{ // Points are co-linear:
      C=M;
      r=0;
      return (E-S).GetPerpendicular();
    }
  }
  g3Cr operator+(const g3Cr& Cr) { // Find sphere enclosing this and Cr
    g3Vector Cc(Cr.C-C);
    double NewRadius=(r+Cc.GetLength()+Cr.r)/2;
    if(NewRadius<=r) return *this;
    Cc.SetLength(NewRadius-r);
    return g3Cr(C+Cc,NewRadius);
  }
  g3Cr& operator+=(const g3Cr& Cr) { // Find sphere enclosing this and Cr
    g3Vector Cc(Cr.C-C);
    double NewRadius=(r+Cc.GetLength()+Cr.r)/2;
    if(NewRadius>r) {
      Cc.SetLength(NewRadius-r);
      C+=Cc;
      r=NewRadius;
    }
    return *this;
  }
  g3Cr&  operator*=(double d)             {              r*=d; return *this;}
  g3Cr&  operator/=(double d)             { r/=d; return *this;}
  bool   operator==(const g3Cr& Cr) const {return Compare(Cr)==0;}
  bool   operator!=(const g3Cr& Cr) const {return Compare(Cr)!=0;}
  bool   IsPoint()                  const {return r==0;}
  bool   IsValid()                  const {return r;}
  gCoord GetVolume()                const {return M_PI*r*r*r;}
  int Compare(const g3Cr& Cr)       const { // For Sorting
    int    Result=           C.Compare(Cr.C);
    return Result ? Result : r.Compare(Cr.r);
  }
  g3Point GetClosestPoint (const g3Point& P) {return (P-C).Normalised()*r;}
  g3Point GetFurthestPoint(const g3Point& P) {return C+(C-GetClosestPoint(P));}
  //CString Report()                     const {return C.Report()+", "+r.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3PV <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g3PV { // Position Vector (or "Point and Vector"). May describe a Plane (P is the Origin, V is the Axis), or Cuboid (V is the diagonal extents).
  g3Point  P; // NormalisePlaneOrigin in the constructors, moves the provided point to the closest point on the plane to the global origin. This helps identify coincident planes.
  g3Vector V;
  g3PV() {}
  g3PV(const double* PV, bool NormalisePlaneOrigin=false)   : P(PV), V(PV+3) {if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  g3PV(double x, double y, double z, double dx, double dy, double dz, bool NormalisePlaneOrigin=false) : P(x,y,z), V(dx,dy,dz) {if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  g3PV(const g3Point& S, const g3Point&  E, bool NormalisePlaneOrigin=false) : P(S), V(E-S) {if(NormalisePlaneOrigin)       P=GetPlaneOrigin();}
  g3PV(const g3Point& P, const g3Vector& V, bool NormalisePlaneOrigin=false) : P(P), V(V)   {if(NormalisePlaneOrigin) g3PV::P=GetPlaneOrigin();}
  g3PV(const double*  P, const double*   V, bool NormalisePlaneOrigin=false) : P(P), V(V)   {if(NormalisePlaneOrigin)       P=GetPlaneOrigin();}
  g3PV& operator =(const g3PV& PV)        {              P=PV.P; V=PV.V; return *this;}
  g3PV& operator*=(double d)              {              P*=d;   V*=d;   return *this;}
  g3PV& operator/=(double d)              {  P/=d;   V/=d;   return *this;}
  bool  operator==(const g3PV& PV)  const {return Compare(PV)==0;}
  bool  operator!=(const g3PV& PV)  const {return Compare(PV)!=0;}
  bool  operator<=(const g3PV& PV)  const {return Compare(PV)<=0;}
  bool  operator>=(const g3PV& PV)  const {return Compare(PV)>=0;}
  bool  operator< (const g3PV& PV)  const {return Compare(PV)< 0;}
  bool  operator> (const g3PV& PV)  const {return Compare(PV)> 0;}
  g3PV  operator* (const double  d) const {return g3PV(P*d, V*d);}
  g3PV  operator/ (const double  d) const {return g3PV(P/d, V/d);}
  void Set(double x, double y, double z, double dx, double dy, double dz, bool NormalisePlaneOrigin=false) {P.Set(x,y,z); V.Set(dx,dy,dz); if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  void Set(const g3Point&  S, const g3Point&   E, bool NormalisePlaneOrigin=false) {P= S; V=E-S;                if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  void Set(const g3Point& _P, const g3Vector& _V, bool NormalisePlaneOrigin=false) {P=_P; V=_V;                 if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  void Set(const double*  _P, const double*   _V, bool NormalisePlaneOrigin=false) {P.Set(_P); V.Set(_V);       if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  void Set(const double* PV, bool NormalisePlaneOrigin=false) {P.Set(PV); V.Set(PV+sizeof(g3Point)/sizeof(double)); if(NormalisePlaneOrigin) P=GetPlaneOrigin();}
  bool    IsZero   ()       const {return P.IsZero() && V.IsZero();}
  g3PV&   Reverse  ()             {P=GetEnd(); V.Reverse(); return *this;}
  g3PV    Reversed ()       const {return g3PV(GetEnd(),V.Reversed());}
  g3Point GetCenter()       const {return P+V/2;} // For Box or Line
  g3Point GetEnd   ()       const {return P+V;}
  gCoord  GetVolume()       const {return V.GetVolume();}
  gCoord  GetDisplacement(bool Normalised=false) const {return (Normalised ? V : V.Normalised()).Dot(P);} // If V is the Axis of a Plane, and P a point on the Plane, this returns the signed Global Origin to Plane distance (negative is away from V). Quicker if you know that V is Normalised!
  g3Point GetPlaneOrigin()  const { // If V is the Axis of a Plane: The projection of the Global Origin onto the Plane.
    g3Vector N(V.Normalised()); // Point to Plane distance reduces to [Axis Dot P] when the Origin is used in the equation.
    return N*N.Dot(P); // Multiply the Axis by this Distance and turn it into a point relative to the origin and you have projected the Point to the Plane.
  }
  bool CoplanarLines(const g3PV& PV) const { // Make a Plane from this and PV.V then see if PV.P is on that Plane:
    g3Vector Axis(V.Cross(PV.V));
    if(Axis.IsNull()) return true; // Parallel Lines
    return PV.P==g3PV(P,Axis.Normalised()).PointToPlane(PV.P,true);
  }
  gCoord    DistanceTo(const g3Point& Point, bool Normalised=false) const {g3Vector D(VectorTo(Point,Normalised)); return D.Dot(V).sgn()*D.GetLength();} // If V is the Axis of a Plane, and P a point on the Plane, this returns the signed Point to Plane distance (negative is away from V). Quicker if you know that V is Normalised!
  g3Vector    VectorTo(const g3Point& Point, bool Normalised=false) const {return (Point-P).ProjectedTo(V,Normalised);} // If V is the Axis of a Plane, and P a point on the Plane, this returns the vector from Point to the Plane. Quicker if you know that V is Normalised!
  ///g3Point PointToPlane(const g3Point& Point, bool Normalised=false) const {return (Point==P) ? P : Point-VectorTo(Point,Normalised);} // If V is the Axis of a Plane, and P a point on the plane: Returns the closest Point on this Plane to the given Point. Quicker if you know that V is Normalised!
  g3Point PointToPlane(const g3Point& Point, bool Normalised=false) const {return (Point==P) ? P : P; printf("g3.h 387 hack\n");} // If V is the Axis of a Plane, and P a point on the plane: Returns the closest Point on this Plane to the given Point. Quicker if you know that V is Normalised!

  bool  IntersectPlane(const g3PV& Plane, g3Point& Intersection) const { // Consider this is an infinite Line and the g3PV Parameter is a Plane. Return true if possible and set the Intersection point.
    double t=V.Dot(Plane.V);
    if(t==0) return false; // Parallel Vector check (Catches Division by zero) here.
    t=(Plane.P-P).Dot(Plane.V)/t;
    Intersection=P+V*t;
    return true;
  }
  bool IntersectLine(const g3PV& Line) const {return ClosestDistance(Line)==0;}
  gCoord ClosestDistance(const g3PV& Line) const {
    g3Vector x(V.Cross(Line.V).Normalised()); // Connecting line is perpendicular to both
    return x.IsNull() ? (Line.P-P).GetLength() : (Line.P-P).Dot(x).abs(); // Parallel lines just use point separation
  }
  g3Point PointToLine(const g3Point& Point, bool Normalised=false) const {return P+(Point-P).ProjectedTo(V,Normalised);} // returns a the closest point on this Infinite Line to the given Point (Perpendicular Intersection). Quicker if you know that V is Normalised!
  gCoord MinimumDistanceFromLineTo(const g3Point& Point, bool Normalised=false) const {return (Point-PointToLine(Point,Normalised)).GetLength();} // Quicker if you know that V is Normalised!

  // For 2 infinite 3D lines, ClosestPoints returns: 0 if parallel; 1 if coincident; 2 if generic (i.e. skew) and can be filleted; 3 if generic (i.e. skew) and cannot be filleted
  // Creates PV; the line between the Closest Points.
  int ClosestPoints(const g3PV& Line, g3PV& PV) const { // PV is the Connecting line from this line to Line
    if(HitInfiniteLine(Line.P) && Line.V==V) { // Coincident: lines intersect
      PV.Set(P,g3Vector(0,0,0));               // Just returns Start Point
      return 1;
    }
    PV.V=V.Cross(Line.V); // Connecting line is perpendicular to both.
    if(PV.V.IsNull()) {   // Degenerate: lines are parallel
      // PV.P=PointToLine(Line.P); ~~Can't do with Null V...
      PV.V=Line.P-PV.P;
      return 0;
    }
    g3PV Plane(Line.P, Line.V.Cross(PV.V.Normalise())); // Form a Plane containing Line and the Connecting Line
    IntersectPlane(Plane, PV.P); // Find the intersection Point of this and the Plane
    PV.V*=(Line.P-P).Dot(PV.V); // Use the Distance to extend PV.V
    return ((PV.V.GetL2()==0) // They intersect (connecting line is zero length)
         && (PV.P!=P) // May intersect exactly with the end (which can't be filleted)
         && ((V.Dot(PV.P-P)<0) == (Line.V.Dot(PV.GetEnd()-Line.P)<0)) // see if the intersection can be filleted (is away from both points in the same direction)
         ) ? 2 : 3; // Distinct (lines skew) (Can be filleted ? 2 : 3);
  } // The above is true if the intersection is ahead of both end points or behind both end points.

  int Compare(const g3PV& PV) const { // For Sorting
    int    Result=           P.Compare(PV.P);
    return Result ? Result : V.Compare(PV.V);
  }
  bool  HitPlane(const g3Point& Point) const {return PointToPlane(Point)==Point;}
  gCoord HitLine(const g3Point& Point) const { // return -1 if not a hit; the parameter t from the start point otherwise: (0=Start, 0.5=Mid, 1=End):
    if(Point== P )   return  0;
    if(V.IsNull())   return -1;
    if(Point==(P+V)) return  1;
    gCoord t(ParameterTo(Point)); // The length from line start to the point
    //return ((t.Between(0,1)) && (Point==P+V*t)) ? t : -1; // The dot product gives answers too close to 1 to use within a tolerance.
    return ((t.Between(0,1)) && (Point==P+V*t)) ? t : t;
    //printf("g3.h 387 hack\n");
  }

  g3Point PointAtParameter(double t, bool Normalize=false) const {return P+t*(Normalize ? V.Normalised() : V);}
  gCoord ParameterTo(const g3Point& Point) const { // return the parameter (t) from the start point: (0=Start, 0.5=Mid, 1=End):
    double L2=V.GetL2();
    if((L2==0) || V.IsNull()) return 0;
    g3Vector SP(Point-P);
    SP=SP.ProjectedTo(V);
    gCoord t(sqrt(SP.GetL2() / L2)); // The length from line start to the point
    return SP.Dot(V).Coord<0 ? -t : t;
  }
// true if The closest point to Point on the infinite line is within the Finite segment from P to P+V
  bool IsNearestPointOnFiniteLine(const g3Point& Point) const {
    g3Vector SP(Point-P); // To visualise, go to http://www.falstad.com/dotproduct/ set B to an angle with length 2, then move A around watching the projected closest point. The dot product crosses 4 when the closest point crosses the end of B.
    return SP.IsNull() ? true : SP.Dot(V).Between(0,V.GetL2());
  }
  bool HitInfiniteLine(const g3Point& Point) const {
    g3Vector SP(Point-P);
    return SP.IsNull() ? true : SP.Normalised().Colinear(V.Normalised());
  }
  bool IsOnBox(const g3Point& Point) const {
    return IsInBox(Point)
        && (((P.x==Point.x) || (Point.x==(P.x+V.x)))
         || ((P.y==Point.y) || (Point.y==(P.x+V.y)))
         || ((P.z==Point.z) || (Point.z==(P.x+V.z))));
  }
  bool IsInBox(const g3Point& Point) const {
    return (P.x<=Point.x) && (Point.x<=(P.x+V.x))
        && (P.y<=Point.y) && (Point.y<=(P.y+V.y))
        && (P.z<=Point.z) && (Point.z<=(P.z+V.z));
  }
  //CString Report() const {return P.Report()+V.Report();}
};

#ifdef Assert
static struct g3PVTester : Tester {
  g3PVTester() {
    { //Scope:
      g3PV Line1(1,2, 2, 4, 3, 2);
      g3PV Line2(1,0,-3, 4,-6,-1);
      Assert(Line1.ClosestDistance(Line2)==4);
      g3PV Line3;
      Assert(Line1.ClosestPoints(Line2,Line3)==3);
      Assert(Line3.P==g3Point(-41./39,18./39,38./39));
      Assert(Line3.V.GetLength()==4);
      Assert(Line3.GetEnd()==g3Point(-5./39,66./39,-106./39));
    }
    { //Scope: IntersectPlane
      g3PV  Line(1,1,1, 1,1,1);
      g3PV Plane(0,0,0, 0,0,1);
      g3Point Point;
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point.IsOrigin());
      Line.V.Set(-1,-1,-1);
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point.IsOrigin());
      Line.Set(3,0,4, 3,0,-4);
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point==g3Point(6,0,0));
      Line.Set(0,0,0, 3,0,-4);
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point.IsOrigin());
      Line.Set(1,2,0, 0,0,1);
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point==g3Point(1,2,0));
      Plane.Set(10,10,10, 1,1,1);
      Line.Set(10,0,0, 0,0,1);
      Assert(Line.IntersectPlane(Plane, Point));
      Assert(Point==g3Point(10,0,20));
    }
    { //Scope:
      g3PV Line1(2,1,1, 6,8,8);
      g3PV Line2(1,8,8, 8,-6,-6);
      Assert(Line1.ClosestDistance(Line2)==0);
      Assert(Line1.IntersectLine(Line2));
      g3PV Line3;
      Assert(Line1.ClosestPoints(Line2,Line3)==2);
      Assert(Line3.P==g3Point(5,5,5));
      Assert(Line3.V.IsNull());
    }
    { //Scope:
      g3PV Line1(0,0,10, 0,1,0);
      g3PV Line2(0,0, 0, 1,0,0);
      Assert( Line1.ClosestDistance(Line2)==10);
      Assert(!Line1.IntersectLine(Line2));
      g3PV Line3;
      Assert(Line1.ClosestPoints(Line2,Line3)==3);
      Assert(Line3.P==g3Point(0,0,10));
      Assert(Line3.V.GetLength()==10);
      Assert(Line3.GetEnd().IsOrigin());
    }
    { //Scope:
      g3PV Line1(0,3,0, 1,1,-1);
      g3PV Line2(5,8,2, 3,7,-1);
      Assert(Line1.ClosestDistance(Line2)==sqrt(14.));
      g3PV Line3;
      Assert(Line1.ClosestPoints(Line2,Line3)==3);
      Assert(Line3.P==g3Point(-1,2,1));
      Assert(Line3.V.GetLength()==sqrt(14.));
      Assert(Line3.GetEnd()==g3Point(2,1,3));
    }
    { //Scope:
      g3PV Line(3,3,3, 2,2,2);
      Assert( Line.PointToLine(g3Point(3,4,5))==g3Point(4,4,4));
      Assert( Line.MinimumDistanceFromLineTo(g3Point(3,4,5))==sqrt(2.));
      Assert( Line.HitLine(g3Point(3,3,3))==0);
      Assert( Line.HitLine(g3Point(4,4,4))==0.5);
      Assert( Line.HitLine(g3Point(5,5,5))==1);
      Assert( Line.HitLine(g3Point(6,6,6))==-1);
      Assert( Line.HitLine(g3Point(0,0,0))==-1);
      Assert( Line.HitLine(g3Point(3,4,5))==-1);
      Assert( Line.HitInfiniteLine(g3Point(3,3,3)));
      Assert( Line.HitInfiniteLine(g3Point(4,4,4)));
      Assert( Line.HitInfiniteLine(g3Point(5,5,5)));
      Assert( Line.HitInfiniteLine(g3Point(6,6,6)));
      Assert( Line.HitInfiniteLine(g3Point(0,0,0)));
      Assert(!Line.HitInfiniteLine(g3Point(3,4,5)));
      Assert( Line.IsNearestPointOnFiniteLine(g3Point(3,3,3)));
      Assert( Line.IsNearestPointOnFiniteLine(g3Point(4,4,4)));
      Assert( Line.IsNearestPointOnFiniteLine(g3Point(5,5,5)));
      Assert(!Line.IsNearestPointOnFiniteLine(g3Point(6,6,6)));
      Assert(!Line.IsNearestPointOnFiniteLine(g3Point(0,0,0)));
      Assert( Line.IsNearestPointOnFiniteLine(g3Point(3,4,5)));
      Assert( Line.IsOnBox(g3Point(3,3,3)));
      Assert(!Line.IsOnBox(g3Point(4,4,4)));
      Assert( Line.IsOnBox(g3Point(5,5,5)));
      Assert(!Line.IsOnBox(g3Point(6,6,6)));
      Assert(!Line.IsOnBox(g3Point(0,0,0)));
      Assert( Line.IsOnBox(g3Point(3,4,5)));
      Assert( Line.IsInBox(g3Point(3,3,3)));
      Assert( Line.IsInBox(g3Point(4,4,4)));
      Assert( Line.IsInBox(g3Point(5,5,5)));
      Assert(!Line.IsInBox(g3Point(6,6,6)));
      Assert(!Line.IsInBox(g3Point(0,0,0)));
      Assert( Line.IsInBox(g3Point(3,4,5)));
      Assert( Line.ParameterTo(g3Point(3,3,3))==0);
      Assert( Line.ParameterTo(g3Point(4,4,4))==0.5);
      Assert( Line.ParameterTo(g3Point(5,5,5))==1);
      Assert( Line.ParameterTo(g3Point(6,6,6))== 1.5);
      Assert( Line.ParameterTo(g3Point(0,0,0))==-1.5);
      //Assert( Line.Parameter2(g3Point(3,4,5))==-1);
    }
  }
} g3PVTester;
#endif // def Assert

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3PP <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g3PP {  // 2 3D Points
  g3Point S,E; // Start End
  g3PP() {}
  g3PP(const double* PP)   : S(  PP), E(PP+3) {}
  g3PP(double sx, double sy, double sz,
       double ex, double ey, double ez) : S(sx,sy,sz), E(ex,ey,ez) {}
  g3PP(const g3Point& S, const g3Point& E) : S(S), E(E) {}
  void Set(double sx, double sy, double sz, double ex, double ey, double ez) {S.Set(sx,sy,sz); E.Set(ex,ey,ez);}
  void Set(const g3Point& _S, const g3Point&  _E) {S=_S; E=_E;}
  void Set(const g3Point&  P, const g3Vector&  V) {S=P; E=P+V;}
  void Set(const double*  _S, const double*   _E) {S.Set(_S); E.Set(_E);}
  void Set(const double* SE) {S.Set(SE); E.Set(SE+sizeof(g3Point)/sizeof(double));}
  void Set(const g3PP& PP) {S=PP.S; E=PP.E;}
  bool IsZero   () const {return S.IsZero() && E.IsZero();}
  void Reverse  () {g3Point T(S); S=E; E=T;}
  g3PP Reversed () const {return g3PP(E,S);}
  void Normalise() {gCoord t; // temporary variable for swaps
    if(S.x>E.x) {t=S.x; S.x=E.x; E.x=t;}
    if(S.y>E.y) {t=S.y; S.y=E.y; E.y=t;}
    if(S.z>E.z) {t=S.z; S.z=E.z; E.z=t;}
  }
  g3Point  Eval(double t) const {return S+GetVector()*t;}
  g3Point  GetStart () const {return S;}
  g3Point  GetEnd   () const {return E;}
  g3Vector GetVector() const {return E-S;}
  g3Point  GetCenter() const {return Eval(0.5);}
  g3Vector GetAxis  () const {return GetVector().Normalise();}
  gCoord   GetLength() const {return GetVector().GetLength();}
  gCoord   GetL2    () const {return GetVector().GetL2();}
  bool     SameX    () const {return S.SameX(E);}
  bool     SameY    () const {return S.SameY(E);}
  bool     SameZ    () const {return S.SameZ(E);}
  bool     IsValid  () const {return !(SameX() && SameY() && SameZ());}
  g3PP& operator =(const g3PP& PP) {Set(PP); return *this;}
  g3PP& operator*=(double d) {              S*=d; E*=d;      return *this;}
  g3PP& operator/=(double d) { S/=d; E/=d;      return *this;}
  g3PP  operator* (double d) {              return g3PP(S*d, E*d);}
  g3PP  operator/ (double d) { return g3PP(S/d, E/d);}
  gCoord Hit(const g3Point& P) const { // return -1 if not a hit;  the parameter t from the start point otherwise: (0=Start, 0.5=Mid, 1=End):
    if(P==S) return 0;
    if(P==E) return 1; // Check equality with start/end - fast check because it will happen a lot.
    g3Vector V(E-S);
    gCoord  t((P-S).GetLength()); // The length from line start to the point
    if(V.IsNull()) return t;
    t/=V.GetLength();
    ///hack
    return ((t<=1.) && (P==S+V*t)) ? t : t; // The dot product gives answers too close to 1 to use within a tolerance.
  }
  bool operator==(const g3PP& PP) const {return Compare(PP)==0;}
  bool operator!=(const g3PP& PP) const {return Compare(PP)!=0;}
  int Compare(const g3PP& PP) const { // For Sorting
    int    Result=           S.Compare(PP.S);
    return Result ? Result : E.Compare(PP.E);
  }
  //CString Report() const {return S.Report()+E.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g3Box <<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Intended for use as an AABB (Axially Aligned Bounding Box) or Frustum

struct g3Box : public g3PP { // Start, End: Diagonally Opposite Corners
  g3Box()                              {}
  g3Box(const g3PP& SE)   : g3PP(SE)   {}
  g3Box(const double* PP) : g3PP(PP)   {}
  g3Box(double sx, double sy, double sz,
        double ex, double ey, double ez) : g3PP(sx,sy,sz, ex,ey,ez) {}
  g3Box(const g3Point& S, const g3Point& E) : g3PP(S,E) {}
  g3Box& operator*=(double d) {return (*this)*=d;}
  g3Box& operator/=(double d) {return (*this)/=d;}
  g3Box  operator* (double d) {              return g3Box(S*d, E*d);}
  g3Box  operator/ (double d) { return g3Box(S/d, E/d);}
  void ToArray(double* D6)      const {memcpy(D6,this,sizeof(*this));}
  bool Contains(const g3Box& B) const {return Hit(B.S) && Hit(B.E);}
  bool Hit(const g3Point& P) const {
    return (P.x>=S.x) && (P.x<=E.x)
        && (P.y>=S.y) && (P.y<=E.y)
        && (P.z>=S.z) && (P.z<=E.z);
  }
  double GetExtents(const g3Vector& V) const {
    g3Point P(S);
    double zMin,zMax,dz; //  For each corner of the Box, project the vector (from the global origin to the corner point) onto V.
    zMin=zMax=V.Dot(P); /* Set up the Min and Max with the first corner, then work around the box corners:
   | Next  |  Project    |       Update       |       Update       |
   |Corner:|P-Origin to V|        ZMin        |        zMax        | */
    P.x=E.x; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.y=E.y; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.x=S.x; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.z=E.z; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.y=S.y; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.x=E.x; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    P.y=E.y; dz=V.Dot(P); if(zMin>dz) zMin=dz; if(zMax<dz) zMax=dz;
    return zMax-zMin;
  }
};

#endif // ndef g3h
