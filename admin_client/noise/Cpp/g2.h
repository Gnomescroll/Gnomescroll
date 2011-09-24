// g2.h

#ifndef g2h
#define g2h // Used in gPlane.cpp

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gCoord.h"

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2D <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2D { // 2 Doubles (Used as x 2 Dimensional Point or Vector)
  gCoord x,y;
  g2D() {}
  g2D(double x, double y) : x(x), y(y) {}
  g2D(const double* DD)                {x=*DD++; y=*DD++;}
#ifdef d3h
  g2D(const g2D& DDD) : x(DDD.x), y(DDD.y) {} // Truncate 3D to 2D
#endif // def d3h
  void Set(double X, double Y)         {x=X; y=Y;}
  void Set(const g2D& DD)              {x=DD.x; y=DD.y;}
  bool IsZero()                  const {return !x && !y;}
  void ToArray(double* DD)       const {*DD++=x; *DD=y;}
  void ToFloatArray(float* FF)   const {*FF++=static_cast<float>(x.Coord); *FF=static_cast<float>(y.Coord);}
  g2D& operator =(const g2D& DD)       {Set(DD);                  return *this;}
  g2D& operator*=(double d)            {              x*=d; y*=d; return *this;}
  g2D& operator/=(double d)            {ASSERT(d!=0); x/=d; y/=d; return *this;}
  bool operator==(const g2D& DD) const {return Compare(DD)==0;}
  bool operator!=(const g2D& DD) const {return Compare(DD)!=0;}
  bool operator<=(const g2D& DD) const {return Compare(DD)<=0;}
  bool operator>=(const g2D& DD) const {return Compare(DD)>=0;}
  bool operator< (const g2D& DD) const {return Compare(DD)< 0;}
  bool operator> (const g2D& DD) const {return Compare(DD)> 0;}
  bool SameX     (const g2D& DD) const {return DD.x==x;}
  bool SameY     (const g2D& DD) const {return DD.y==y;}
  operator double*()                   {return &x.Coord;}
  operator const double*()       const {return &x.Coord;}
  int Compare(const g2D& DD)     const { // For Sorting
    int    Result=           x.Compare(DD.x);
    return Result ? Result : y.Compare(DD.y);
  }
  CString Report() const {CString S; S.Format("(%g, %g)",x.RawReport(),y.RawReport()); return S;}
};

inline g2D operator* (gCoord c, const g2D& DD) {return g2D(DD.x*c, DD.y*c);}
inline g2D operator* (double d, const g2D& DD) {return g2D(DD.x*d, DD.y*d);}
inline g2D operator* (int    i, const g2D& DD) {return g2D(DD.x*i, DD.y*i);}

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Vector <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2Vector : public g2D { // 2D Vector
  g2Vector() {}
#ifdef d3h
  g2Vector(const g3Vector& V)  : g2D(V.x,V.y) {} // 3D to 2D truncation
#endif // def d3h
  g2Vector(const g2D& V)       : g2D(V)       {}
  g2Vector(double x, double y) : g2D(x,y)     {}
  g2Vector(const double* DD)   : g2D(DD)      {}
  gCoord Dot(const g2Vector& V) const {return x*V.x+y*V.y;} // Magnitude=|a||b|cos(t)
  int  Cross(const g2Vector& V) const {return ((x*V.y) - (y*V.x)).sgn();} // -1,0,1 =the sign of the normal Vector (Right hand rule: -1=cw from this to V)=|a||b| sin(t) n
  bool cwTo (const g2Vector& V) const {return (x*V.y).Coord < (y*V.x).Coord;} // The .Coord makes sure the gCoord Tolerance isn't used.
  bool IsHorizontal()           const {return !y;}
  bool IsVertical  ()           const {return !x;}
  bool IsNull      ()           const {return IsZero();}
  gCoord GetLength()            const {return _hypot(x,y);} // The length (always positive) Euclidean (shortest line).
  gCoord GetL2    ()            const {return x*x+y*y;} // The length squared. Saves the slow Square Root for some calculations.
  gCoord GetManhattan() const {double d=x.abs()+y.abs(); return d*d;} // The length in axial directions (as if travelling around a city).
  gCoord GetCityBlock() const {double d=Max(x.abs(),y.abs()); return d*d;} // The length of the longest Axial journey.
  gCoord GetQuadratic() const {return GetL2()+x*y;} // Everything multiplied by everything else!
  gCoord GetArea  ()            const {return x*y;} // The Surface area of the axially-aligned rectangle this vector fills.
  g2Vector& SetLength(double Length)  {Normalise(); return operator*=(Length);}
  g2Vector Normalised()         const {
    g2Vector V(0,0);
    double Length=GetLength();
    if(Length) V=operator/(Length);
    return V;
  }
  g2Vector& Normalise  ()                        {Set(Normalised());          return *this;}
  g2Vector& Reverse    ()                        {Set(-x,-y);                 return *this;}
  g2Vector  Reversed   ()                  const {return g2Vector(-x,-y);}
  g2Vector& operator*= (double d)                {              x*=d;  y*=d;  return *this;}
  g2Vector& operator/= (double d)                {ASSERT(d!=0); x/=d;  y/=d;  return *this;}
  g2Vector& operator=  (const g2Vector& V)       {x=V.x; y=V.y; return *this;}
  g2Vector  operator+  (const g2Vector& V) const {return g2Vector(x+V.x, y+V.y);}
  g2Vector  operator-  (const g2Vector& V) const {return g2Vector(x-V.x, y-V.y);}
  g2Vector  operator-  ()                  const {return g2Vector(-x, -y);}
  g2Vector  operator*  (double d)          const {              return g2Vector(x*d, y*d);}
  g2Vector  operator/  (double d)          const {ASSERT(d!=0); return g2Vector(x/d, y/d);}
  g2Vector  ProjectedTo(const g2Vector& V, bool Normalised=false) const {ASSERT(V.GetL2()!=0); return Normalised ? V*Dot(V) : V*(Dot(V)/V.GetL2());} // Quicker if you know that V is Normalised!
  bool      SameX      (const g2Vector& V) const {              return V.x==x;}
  bool      SameY      (const g2Vector& V) const {              return V.y==y;}
  bool      Colinear   (const g2Vector& V) const {              return ((V.x==x) && (V.y==y)) || ((V.x==-x) && (V.y==-y));} // Assumes both vectors are Normalised
};


#ifdef Assert
static struct g2VectorTester : Tester {
  g2VectorTester() {
    OperatorTester();
    CrossTester();
  }
  void OperatorTester() {
    g2Vector v1(2,3);
    g2Vector v2(2,4);
    Assert(v1==v1);
    Assert(v1<=v1);
    Assert(v1>=v1);
    Assert(v1==g2Vector(2,3));
    Assert(v1!=v2);
    Assert(v1< v2);
    Assert(v1<=v2);
    Assert(v2> v1);
    Assert(v2>=v1);
  }
  void CrossTester() {
    g2Vector v1(3, 4);
    g2Vector v2(3, 0);
    Assert(v1.Cross(v1)==0);
    Assert(v2.Cross(v2)==0);
    Assert(v1.Cross(v2)==-1);
    Assert(v1.cwTo(v2)==true);
    Assert(v2.Cross(v1)==1);
    Assert(v2.cwTo(v1)==false);
  }
} g2VectorTester;
#endif

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Point <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2Point : public g2D { // 2D Point
  g2Point() {}
#ifdef d3h
  g2Point(const g3Point& P)   : g2D(P.x,P.y) {} // 3D to 2D truncation
#endif // def d3h
  g2Point(const g2D& P)       : g2D(P)    {}
  g2Point(double x, double y) : g2D(x,y)  {}
  g2Point(const double* DD)   : g2D(DD)   {}
  g2Point& operator+=(const g2Vector& V)       {x+=V.x; y+=V.y; return *this;}
  g2Point& operator-=(const g2Vector& V)       {x-=V.x; y-=V.y; return *this;}
  g2Point& operator*=(const double    d)       {              x*=d; y*=d; return *this;}
  g2Point& operator/=(const double    d)       {ASSERT(d!=0); x/=d; y/=d; return *this;}
  g2Point& operator= (const g2Point&  P)       {x =P.x; y =P.y; return *this;}
  g2Point  operator+ (const g2Vector& V) const {return g2Point (x+V.x, y+V.y);}
  g2Vector operator- (const g2Point&  P) const {return g2Vector(x-P.x, y-P.y);}
  g2Point  operator* (const double    d) const {              return g2Point(x*d, y*d);}
  g2Point  operator/ (const double    d) const {ASSERT(d!=0); return g2Point(x/d, y/d);}
  bool     SameX     (const g2Point&  P) const {return P.x==x;}
  bool     SameY     (const g2Point&  P) const {return P.y==y;}
  bool     IsOrigin  ()                  const {return IsZero();}
  void     Move(double dx, double dy)          {x+=dx; y+=dy;}
  double   DistanceTo(const g2Point& E)  const {return (E-*this).GetLength();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2PV <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2PV { // Position Vector (or "Point and Vector"). May describe a Rectangle.
  g2Point  P;
  g2Vector V;
  g2PV() {}
#ifdef d3h
  g2PV(const g3PV& PV)     : P(PV.P), V( PV.V) {} // 3D to 2D truncation
#endif // def d3h
  g2PV(const double* PV)   : P(  PV), V(PV+2) {}
  g2PV(double x, double y, double dx, double dy) : P(x,y), V(dx,dy) {}
  g2PV(const g2Point& S, const g2Point&  E) : P(S), V(E-S) {}
  g2PV(const g2Point& P, const g2Vector& V) : P(P), V(V)   {}
  g2PV(const double*  P, const double*   V) : P(P), V(V)   {}
  g2PV& operator =(const g2PV&  PV) {P=PV.P; V=PV.V;     return *this;}
  g2PV& operator*=(double d)        {              V*=d; return *this;}
  g2PV& operator/=(double d)        {ASSERT(d!=0); V/=d; return *this;}
  bool IsZero()               const {return P.IsZero() && V.IsZero();}
  void Set(double x, double y, double dx, double dy) {P.Set(x,y); V.Set(dx,dy);}
  void Set(const g2Point&  S, const g2Point&   E) {P= S; V=E-S;}
  void Set(const g2Point& _P, const g2Vector& _V) {P=_P; V=_V;}
  void Set(const double*  _P, const double*   _V) {P=_P; V=_V;}
  g2PV&   Reverse  ()       {V.Reverse(); return *this;}
  g2PV    Reversed () const {return g2PV(GetEnd(),V.Reversed());}
  g2Point GetCenter() const {return P+V/2;}
  g2Point GetEnd   () const {return P+V;}
  gCoord  GetArea  () const {return V.GetArea();}
  gCoord  Hit(const g2Point& Point) const { // return -1 if not a hit; the parameter t from the start point otherwise (0=Start, 0.5=Mid, 1=End):
    if(Point== P )   return  0;
    if(V.IsNull())   return -1;
    if(Point==(P+V)) return  1;
    gCoord t(sqrt((Point-P).GetL2() / V.GetL2())); // The length from line start to the point
    return ((t<=1.) && (Point==P+V*t)) ? t : -1;    // The dot product gives answers too close to 1 to use within a tolerance.
  }
  int Compare(const g2PV& PV) const { // For Sorting
    int    Result=           P.Compare(PV.P);
    return Result ? Result : V.Compare(PV.V);
  }
  CString Report() const {return P.Report()+V.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2PVPV <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2PVPV { // Two Position Vectors (a good way to specify curves)
  g2PV  S,E; // Start, Start Tangent, End and End Tangent
  g2PVPV() {}
#ifdef d3h
  g2PVPV(const g3PVPV& Curve) : S(Curve.S), E(Curve.E) {} // 3D to 2D truncation
#endif // def d3h
  g2PVPV(const double* PVPV)  : S(   PVPV), E(PVPV+4) {}
  g2PVPV(const double* P1, const double* V1, const double* P2, const double* V2) : S(P1,V1), E(P2,V2) {}
  g2PVPV(double Sx, double Sy, double Sdx, double Sdy, double Ex, double Ey, double Edx, double Edy) : S(Sx,Sy, Sdx,Sdy), E(Ex,Ey, Edx,Edy) {}
  g2PVPV(const g2PV& S, const g2PV& E) : S(S), E(E) {}
  bool    IsZero() const {return S.IsZero() && E.IsZero();}
  g2PVPV& Reverse()      {g2PV T(S); S=E; E=T; S.V.Reverse(); E.V.Reverse(); return *this;} // If this is a Curve, Reverse gives the Gap Curve
  int     Compare(const g2PVPV& PVPV) const { // For Sorting
    int    Result=           S.Compare(PVPV.S);
    return Result ? Result : E.Compare(PVPV.E);
  }
  CString Report() const {return S.Report()+E.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Cr <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2Cr { // Center and radius (Circle)
  g2Point C;
  gCoord  r;
  g2Cr() {}
#ifdef d3h
  g2Cr(const g3Cr& Cr)               : C(Cr.C), r(Cr.r) {} // 3D to 2D truncation
#endif // def d3h
  g2Cr(const double* Cr)             : C(  Cr), r(Cr[2]) {}
  g2Cr(double x, double y, double r) : C(x, y), r(r) {}
  g2Cr(const g2Point& C,   double r) : C(   C), r(r) {}
  g2Cr(const g2Point& S, const g2Point& M, const g2Point& E)     {Set(S,M,E);}
  bool IsZero() const {return C.IsZero() && !r;}
  bool Set(const g2Point& S, const g2Point& M, const g2Point& E) { // 3 Point Arc. Returns true if AntiClockwise
    g2Vector MS(S-M);
    g2Vector ME(E-M);
    double MS2=MS.GetL2();
    double ME2=ME.GetL2();
    double MSE=MS.Dot(ME);
    MS*=ME2*(MS2-MSE);
    ME*=MS2*(ME2-MSE);
    double d=2*(MS2*ME2-MSE*MSE);
    if(d) {
      C=M+(MS+ME)/d;
      r=sqrt((MS2*ME2*(MS2+ME2-2*MSE))/(2*d));
    }else{C=M; r=0;} // Points are co-linear.
    return MS.cwTo(ME);
  }
  g2Cr&   operator*=(double d)             {              r*=d; return *this;}
  g2Cr&   operator/=(double d)             {ASSERT(d!=0); r/=d; return *this;}
  bool    operator==(const g2Cr& Cr) const {return Compare(Cr)==0;}
  bool    operator!=(const g2Cr& Cr) const {return Compare(Cr)!=0;}
  bool    IsValid  ()                const {return r;}
  g2Point GetTop   ()                const {return g2Point(C.x  ,C.y+r);}
  g2Point GetRight ()                const {return g2Point(C.x+r,C.y  );}
  g2Point GetBottom()                const {return g2Point(C.x  ,C.y-r);}
  g2Point GetLeft  ()                const {return g2Point(C.x-r,C.y  );}
  gCoord  GetArea  ()                const {return M_PI*r*r;}
  int Compare(const g2Cr& Cr)        const { // For Sorting
    int    Result=           C.Compare(Cr.C);
    return Result ? Result : r.Compare(Cr.r);
  }
  CString Report() const {return C.Report()+", "+r.Report();}
};
/* From comp graphics algorithms
  Set(g2Point S, g2Point M, g2Point E) { // 3 Point Arc.
    double A=M.x-S.x;
    double B=M.y-S.y;
    double C=E.x-S.x;
    double D=E.y-S.y;
    double E=A*(S.x+M.x)+B*(S.y+M.y);
    double F=C*(S.x+E.x)+D*(S.y+E.y);
    double G=2*(A*(E.y-M.y)-B*(E.x-M.x));
    C.x=(D*E-B*F)/G;
    C.y=(A*F-C*E)/G;
    r=hypot(S.x-C.x, S.y-C.y);
  } */

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2PP <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2PP {  // 2 2D Points
  g2Point S,E; // Start and End
  g2PP() {}
#ifdef d3h
  g2PP(const g3PP& SE)       : S(SE.S), E( SE.E) {} // 3D to 2D truncation
#endif // def d3h
  g2PP(g2Point S, g2Point E) : S(   S), E(    E) {}
  g2PP(const double* PP)     : S(  PP), E(PP+2) {}
  g2PP(double sx, double sy,
       double ex, double ey) : S(sx,sy), E(ex,ey) {}
  bool IsZero   () const {return S.IsZero() && E.IsZero();}
  void Reverse  () {g2Point T(S); S=E; E=T;}
  void Normalize() {
    gCoord t;
    if(S.x>E.x) {t=S.x; S.x=E.x; E.x=t;}
    if(S.y>E.y) {t=S.y; S.y=E.y; E.y=t;}
  }
  g2Point  Eval(double t) const {return S+GetVector()*t;}
  g2Point  GetStart () const {return S;}
  g2Point  GetEnd   () const {return E;}
  g2Vector GetVector() const {return E-S;}
  g2Point  GetCenter() const {return Eval(0.5);}
  g2Vector GetAxis  () const {return GetVector().Normalise();}
  gCoord   GetLength() const {return GetVector().GetLength();}
  gCoord   GetL2    () const {return GetVector().GetL2();}
  bool     SameX    () const {return S.SameX(E);}
  bool     SameY    () const {return S.SameY(E);}
  bool     IsValid  () const {return !(SameX() && SameY());}
  g2PP& operator =(const g2PP& PP) {        S=PP.S; E=PP.E;  return *this;}
  g2PP& operator*=(double d) {              S*=d; E*=d;      return *this;}
  g2PP& operator/=(double d) {ASSERT(d!=0); S/=d; E/=d;      return *this;}
  g2PP  operator* (double d) {              return g2PP(S*d, E*d);}
  g2PP  operator/ (double d) {ASSERT(d!=0); return g2PP(S/d, E/d);}
  gCoord Hit(const g2Point& P) const { // return -1 if not a hit; the parameter t from the start point otherwise (0=Start, 0.5=Mid, 1=End):
    if(P==S) return  0;
    if(P==E) return  1;
    g2Vector V(GetVector());
    if(V.IsNull()) return -1;
    gCoord t(sqrt((P-S).GetL2()/V.GetL2())); // The length from line start to the point
    return ((t<=1.) && (P==S+V*t)) ? t : -1; // The dot product gives answers too close to 1 to use within a tolerance.
  }
  bool operator==(const g2PP& PP) const {return Compare(PP)==0;}
  bool operator!=(const g2PP& PP) const {return Compare(PP)!=0;}
  int Compare(const g2PP& PP) const { // For Sorting
    int    Result=           S.Compare(PP.S);
    return Result ? Result : E.Compare(PP.E);
  }
  CString Report() const {return S.Report()+E.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2PPP <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2PPP { // 3 2D Points
  g2Point S,M,E; // Start Middle End (of perhaps Start, Center, End points of a 2D Arc)
  g2PPP() {}
#ifdef d3h
  g2PPP(const g3PPP& SME)   : S(SME.S), M( SME.M), E( SME.E) {} // 3D to 2D truncation
#endif // def d3h
  g2PPP(const g2Point& S, const g2Point& M, const g2Point& E) : S(S), M(M), E(E) {}
  g2PPP(const double* PPP)  : S(  PPP), M(PPP+2), E(PPP+4) {}
  g2PPP(double sx, double sy,
        double mx, double my,
        double ex, double ey) : S(sx,sy), M(mx,my), E(ex,ey) {}
  bool     IsZero   () const {return S.IsZero() && M.IsZero() && E.IsZero();}
  void     Reverse  ()       {g2Point T(S); S=E; E=T;}
  g2Vector GetSE    () const {return E-S;}
  g2Vector GetMS    () const {return M-S;}
  g2Vector GetME    () const {return M-E;}
  g2Point  GetStart () const {return S;}
  g2Point  GetMiddle() const {return M;}
  g2Point  GetEnd   () const {return E;}
  bool     SameX    () const {return S.x==M.x && M.x==E.x;}
  bool     SameY    () const {return S.y==M.y && M.y==E.y;}
  bool     IsValid  () const {return !(SameX() && SameY());}
  g2Cr GetCircle(bool* Anticlockwise) {
    g2Cr Cr;
    *Anticlockwise=Cr.Set(S,M,E);
    return Cr;
  }
  g2PPP& operator =(const g2PPP& PPP) {S=PPP.S; M=PPP.M; E=PPP.E; return *this;}
  bool   operator==(const g2PPP& PPP) const {return Compare(PPP)==0;}
  bool   operator!=(const g2PPP& PPP) const {return Compare(PPP)!=0;}
  int Compare(const g2PPP& PPP) const { // For Sorting
    int Result=S.Compare(PPP.S); if(Result) return Result;
        Result=M.Compare(PPP.M); if(Result) return Result;
    return     E.Compare(PPP.E);
  }
  CString Report() const {return S.Report()+M.Report()+E.Report();}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Line <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2Line : public g2PP { // Start, End points
  g2Line()                   : g2PP(0,0,0,0) {}
  g2Line(const g2PP&     SE) : g2PP(SE)   {}
  g2Line(const double*   PP) : g2PP(PP)   {}
#ifdef d3h
  g2Line(const g3Line& Line) : g2PP(Line) {} // 3D to 2D truncation
#endif // def d3h
  g2Line(const g2Point& S, const g2Point& E) : g2PP(S,E) {}
  void Set(g2PP SE) {S=SE.S; E=SE.E;}
  virtual ~g2Line() {}
  g2Point GetMidPoint() const {return GetCenter();}
  void Move(const g2Vector& Tx) {S+=Tx; E+=Tx;}
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Box <<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct g2Box : public g2PP { // Start, End points
  g2Box() : g2PP(0,0,0,0) {}
  g2Box(const g2PP&   SE) : g2PP(SE) {}
  g2Box(const double* PP) : g2PP(PP) {}
  g2Box(double sx, double sy,
        double ex, double ey) : g2PP(sx,sy, ex,ey) {}
#ifdef d3h
  g2Box(const g3Box& Box) : g2PP(Box) {} // 3D to 2D truncation
#endif // def d3h
  g2Box(const g2Point& S, const g2Point& E) : g2PP(S,E) {}
  void Set(g2PP SE) {S=SE.S; E=SE.E;}
  g2Box& operator*=(double d)  {              return (*this)*=d;}
  g2Box& operator/=(double d)  {ASSERT(d!=0); return (*this)/=d;}
  g2Box  operator* (double d)  {              return g2Box(S*d, E*d);}
  g2Box  operator/ (double d)  {ASSERT(d!=0); return g2Box(S/d, E/d);}
  bool Contains(const g2Box& B) const {return Hit(B.S) && Hit(B.E);}
  bool Hit(const g2Point& P) const {
    return (P.x>=S.x) && (P.x<=E.x)
        && (P.y>=S.y) && (P.y<=E.y);
  }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>> g2Arc <<<<<<<<<<<<<<<<<<<<<<<<<<<<

class g2Arc {
protected:
  g2Cr Cr; // Center, and Radius of a 2D Sphere
  g2PP SE; // Start, End points of a 2D Arc - Anticlockwise from Start to End
public:
  g2Arc(const g2Point& C, const gCoord& r, const g2Point& S, const g2Point& E) : Cr(C,r), SE(S,E) {}
  g2Arc(const g2PPP& SME) : SE(SME.S,SME.E) {Cr.C=SME.M; SetRadius();}
  g2Arc(const g2Cr& Cr, const g2PP& SE) : Cr(Cr), SE(SE) {}
  g2Arc(const g2Arc& Arc) : Cr(Arc.Cr), SE(Arc.SE) {} // Copy Constructor
  g2Arc(const g2PVPV& PVPV) : Cr(0,0,0), SE(PVPV.S.P, PVPV.E.P) {SetRadius();} // Currently looses the vector info
  void    Close      ()       {SE.E=SE.S;} // Closes an Arc to become a circle
  bool    IsValid    () const {return Cr.r;}
  bool    IsCircle   () const {return IsValid() && SE.IsValid();}
  g2Point GetCenter  () const {return Cr.C;}
  gCoord  GetRadius  () const {return Cr.r;}
protected:
  gCoord  SetRadius  ()       {return Cr.r=SE.GetLength();}
public:
  g2Point GetStart   () const {return SE.S;}
  g2Point GetMidPoint() const {
    if(!SE.IsValid()) return Cr.C-(SE.S-Cr.C); // Full Circle
    g2Vector CM(SE.GetCenter()-Cr.C);
    g2Vector CS(SE.S-Cr.C);
    if(CM.IsNull()) return Cr.C-g2Vector(CS.y,-CS.x); // Semi-Circle Axis.Cross(CS) reduces to (CS.y,-CS.x)
    return Cr.C+((CM.Normalised())*(IsMinor() ? Cr.r : -Cr.r));
  }
  g2Point GetEnd     () const {return SE.E;}
  void    SetStart   (  const g2Point& S) {SE.S=S;}
  void    SetEnd     (  const g2Point& E) {SE.E=E;}
  bool    SameX      () const {return SE.SameX();}
  bool    SameY      () const {return SE.SameY();}
  bool    IsMinor    () const {return (SE.E-Cr.C).cwTo(SE.S-Cr.C);} // (Right hand rule from S to E) YOU need to account for Full and Semi-circles!!!
  bool    IsMajor    () const {return !IsMinor();}                  // (Right hand rule from S to E) YOU need to account for Full and Semi-circles!!!
  double GetMinAngle () const {double a=1.-g2Vector(SE.E-SE.S).GetL2()/(2.*Cr.r*Cr.r); return acos(min(max(-1., a), 1.));} // Get angle with cosine rule. Requires a VERY slow arc cosine.
  double GetMaxAngle () const {return 2*M_PI-GetMinAngle();}
  double GetAngle    () const {return SE.IsValid() ? (IsMinor() ? GetMinAngle() : GetMaxAngle()) : 2*M_PI;} // returns the included angle when sweeping from Start to End ANTICLOCKWISE (if S==E it assumes it's a full circle).
  double GetLength   () const {return GetAngle()*GetRadius();} // returns the length when sweeping from Start to End ANTICLOCKWISE
  bool Congruent(const g2Arc& Arc) const {return Arc.Cr==Cr;}
  void Move(const g2Vector& Tx) {Cr.C+=Tx;}
  void Reverse() {SE.Reverse();}
  gCoord Hit(const g2Point& P) const {       // return -1 if not a hit; 0=StartPoint, 1=EndPoint, 0.5=mid, or the Angle from the start point otherwise:
    if(P==SE.S) return 0;
    if(P==SE.E) return 1;                  // Check equality with start/end
    g2Vector Vp(P-Cr.C);                  // Vector from the Center to the Point
    if(Vp.GetL2()!=GetRadius()*GetRadius()) return -1; // Check distance from Center
    double L2=g2Vector(P-SE.S).GetL2(); // Distance from Start Point to the Point all squared.
    g2Vector Vs(SE.S-Cr.C);            // Vector from the Center to the Start Point
    bool VpMajor=Vs.cwTo(Vp);         // Direction from Start Point to Point
    if(SE.IsValid()) {               // if S==E assume it's a Circle
      if(SE.GetCenter()==Cr.C) {    // Semi-Circle
        if(VpMajor) return -1;     // Wrong side
      }else{                      // Normal Arc:
        if(VpMajor!=Vs.cwTo(SE.E-Cr.C)) { // P and E are on different sides of the line CS:
          if(VpMajor) return -1;         // Wrong side
        }else{                          // P and E are on the same side of the line CS:
          bool SPE=((SE.E-SE.S).GetL2() > L2); // Distance SE is more than SP (E is further from S than P).
          if(VpMajor==SPE) return -1;         // "Both Major and SPE" is a Miss and so is "both Minor and not SPE".
    } } }                                    // Go on to calculate and return the
    double Angle=1.-L2/(2.*Cr.r*Cr.r);     // cos Angle from Start Point to Point
    Angle=acos(min(max(-1., Angle), 1.)); // Angle from Start Point to Point
    return (VpMajor ? 2*M_PI-Angle : Angle) / GetAngle(); // SCP Angle ANTICLOCKWISE
  }
  CString Report() const {return Cr.Report()+SE.Report();}
};

#ifdef Assert
static struct g2ArcTester : Tester {
  g2ArcTester() { //   S      E      cw        Angle
    TestArc(10,10,5, 10,15, 13,14, false, 2*PI-0.643501108793284); // |/ u
    TestArc(10,10,5, 13,14, 10,15,  true, 2*PI-0.643501108793284); // |/ u
    TestArc(10,10,5, 10,15, 13,14,  true,      0.643501108793284); // |/ n
    TestArc(10,10,5, 13,14, 10,15, false,      0.643501108793284); // |/ n
    TestArc(10,10,5,  7,14, 13,14, false, 2*PI-1.287002217586568); // \/ u
    TestArc(10,10,5, 13,14,  7,14,  true, 2*PI-1.287002217586568); // \/ u
    TestArc(10,10,5,  7,14, 13,14,  true,      1.287002217586568); // \/ n
    TestArc(10,10,5, 13,14,  7,14, false,      1.287002217586568); // \/ n
    TestArc(10,10,5,  7, 6, 13,14, false,      3.141592653589793); // /  >
    TestArc(10,10,5,  7, 6, 13,14,  true,      3.141592653589793); // /  <
    TestArc(10,10,5, 13,14,  7, 6,  true,      3.141592653589793); // /  >
    TestArc(10,10,5, 13,14,  7, 6, false,      3.141592653589793); // /  <
  }
  void TestArc(double Cx, double Cy, double r, double Sx, double Sy, double Ex, double Ey, bool cw, double Angle) {
    g2Point C(Cx,Cy);
    g2Point S(Sx,Sy);
    g2Point E(Ex,Ey);
    g2Arc Arc(g2Cr(C,r), g2PP(S,E));
    Assert(Arc.GetCenter()==C);
    Assert(Arc.GetRadius()==r);
    Assert(Arc.GetStart ()==S);
    Assert(Arc.GetEnd   ()==E);
    Assert(Arc.GetAngle ()-(cw ? 2*PI-Angle : Angle) < 1e-15); // GetAngle() relies on the arc being clockwise
    g2Point M(Arc.GetMidPoint());
    g2Point O(C-(M-C)); // Opposite Mid-Point
    Assert(Arc.Hit(S)==  0);
    Assert(Arc.Hit(E)==  1);
    Assert(Arc.Hit(M)==0.5); // GetMidPoint() relies on the arc being clockwise
    Assert(Arc.Hit(O)== -1);  // The opposite midpoint will too.
    Assert(Arc.Hit(C)== -1);
  }
} g2ArcTester;
#endif // def Assert

#endif // ndef g2h

