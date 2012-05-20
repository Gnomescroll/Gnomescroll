/* Simplex.cpp
Adapted to C++ from original code sdnoise1234.c
Copyright © 2003-2008, Stefan Gustavson
Contact: stefan.gustavson@gmail.com
_____________________________________________________________
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
_________________________________________________________________________

This code was used to find the range-correction values (the return value equations):
  double mn= DBL_MAX;
  double mx=-DBL_MAX;
  for(WORD X=100; X--;) {
//            double d=Simplex::Noise1(Twister::GetDouble()*X);
    for(WORD Y=100; Y--;) {
//            double d=Simplex::Noise2(Twister::GetDouble()*X,Twister::GetDouble()*Y);
      for(WORD Z=100; Z--;) {
//            double d=Simplex::Noise3(Twister::GetDouble()*X,Twister::GetDouble()*Y,Twister::GetDouble()*Z);
        for(WORD W=100; W--;) {
          double d=Simplex::Noise4(Twister::GetDouble()*X,Twister::GetDouble()*Y,Twister::GetDouble()*Z,Twister::GetDouble()*W);
          if(mn>d) mn=d;
          if(mx<d) mx=d;
  } } } }
  TRACE("Noise*%g-%g\r\n[%g, %g]\r\n" ,2/(mx-mn),mn*2/(mx-mn)+1, mn*2/(mx-mn)-(mn*2/(mx-mn)+1), mx*2/(mx-mn)-(mn*2/(mx-mn)+1));
*/

//#include "stdafx.h"
#include "Simplex.h"
#include <math.h>
#include <float.h>

// Permutation table. This is just a random jumble of all numbers 0-255,
// repeated twice to avoid wrapping the index at 255 for each lookup.

unsigned char Simplex::Mutate[512]={
  151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
  35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
  55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
  250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
  43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,
  239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
  151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
  35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
  55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
  250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,
  43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,
  239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// Helper function to compute gradients.
void Simplex::Grad1(int Hash, double* gx) {
  int h=Hash & 15;
  *gx=1+(h&7);      // Gradient value is one of 1.0, 2.0, ..., 8.0
  if(h&8) *gx=-*gx; // Make half of the gradients negative
}

// 1D simplex noise with derivative.
// If the last argument is not null, the analytic derivative is also calculated.
double Simplex::Noise1(double x, double* dx/*=0*/) {
  int i0=Floor(x); // Standard floor flushes the processor pipeline.
  int i1=i0+1;
  double x0=x-i0;
  if(x0==0) x0+=DBL_EPSILON; // Otherwise it's always zero at integer x!
  double x1=x0-1;
  double x20=x0*x0;
  double t0=1-x20;
//if(t0<0) t0=0; // Never happens for 1D: x0<=1 always
  double t20=t0*t0;
  double t40=t20*t20;
  double gx0;
  Grad1(Mutate[i0 & 0xFF], &gx0);
  double Noise=t40*gx0*x0;
  double x21=x1*x1;
  double t1=1-x21;
//if(t1<0) t1=0; // Never happens for 1D: |x1|<=1 always
  double t21=t1*t1;
  double t41=t21*t21;
  double gx1;
  Grad1(Mutate[i1 & 0xFF], &gx1);
  Noise+=t41*gx1*x1;
  if(dx) { // Compute derivative according to:
    *dx =t20*t0*gx0*x20; // *dx =-8 * t20 * t0 * x0 * (gx0 * x0) + t40 * gx0;
    *dx+=t21*t1*gx1*x21; // *dx+=-8 * t21 * t1 * x1 * (gx1 * x1) + t41 * gx1;
    *dx*=-8;
    *dx+=t40*gx0+t41*gx1;
    *dx*=0.407461; // Scale derivative to match the noise scaling:
  }
  return MinMax(-1.0, Noise*0.407461+0.030914, 1.0); // The maximum value of this noise is 8*pow(3/4,4)=2.53125
}

// Helper function to compute gradients and gradients-dot-residualvectors.
void Simplex::Grad2(int Hash, double* gx, double* gy) {
  static double grad2lut[8][2]={ // Gradient tables. These could be programmed the Ken Perlin way with some clever bit-twiddling, but this is clearer.
    {-1, -1}, {1,  0}, {-1, 0}, {1,  1},
    {-1,  1}, {0, -1}, { 0, 1}, {1, -1}
  };
  int h=Hash & 7;
  *gx=grad2lut[h][0];
  *gy=grad2lut[h][1];
}

// 2D simplex noise with derivatives.
// If the last two arguments are not null, the analytic derivative (the 2D gradient of the scalar noise field) is also calculated.
double Simplex::Noise2(double x, double y, double* dx, double* dy) {
  // Skew the input space to determine the current simplex cell:
  double s=(x+y)*0.366025403; // (sqrt(3)-1)/2
  double xs=x+s;
  double ys=y+s;
  int i=Floor(xs); // Standard floor flushes the processor pipeline.
  int j=Floor(ys);
  const double G2=0.211324865; // (3-sqrt(3))/6
  double t=double(i+j)*G2;
  double X0=i-t; // Unskew the cell origin back to (x,y) space
  double Y0=j-t;
  double x0=x-X0; // The x,y distances from the cell origin
  double y0=y-Y0;
  // For the 2D case, the simplex shape is an equilateral triangle. Determine which simplex we are in.
  int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
  if(x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
  else      {i1=0; j1=1;} // upper triangle, YX order: (0,0)->(0,1)->(1,1)
// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where c=(3-sqrt(3))/6
  double x1=x0-i1+G2; // Offsets for middle corner in (x,y) unskewed coords
  double y1=y0-j1+G2;
  double x2=x0-1+2*G2; // Offsets for last corner in (x,y) unskewed coords
  double y2=y0-1+2*G2;
  // Wrap the integer indices at 256, to avoid indexing Mutate[] out of bounds
  int ii=i & 0xFF;
  int jj=j & 0xFF;
  // Calculate the contribution from the three corners
  double n0, n1, n2; // Noise contributions from the three simplex corners
  double gx0, gy0, gx1, gy1, gx2, gy2; // Gradients at simplex corners
  double t0=0.5f-x0*x0-y0*y0;
  double t20, t40;
  if(t0<0) t40=t20=t0=n0=gx0=gy0=0; // No influence
  else {
    Grad2(Mutate[ii+Mutate[jj]], &gx0, &gy0);
    t20=t0*t0;
    t40=t20*t20;
    n0=t40*(gx0*x0+gy0*y0);
  }
  double t1=0.5f-x1 * x1-y1 * y1;
  double t21, t41;
  if(t1<0) t21=t41=t1=n1=gx1=gy1=0; // No influence
  else {
    Grad2(Mutate[ii+i1+Mutate[jj+j1]], &gx1, &gy1);
    t21=t1*t1;
    t41=t21*t21;
    n1=t41*(gx1*x1+gy1*y1);
  }
  double t2=0.5f-x2*x2-y2*y2;
  double t22, t42;
  if(t2<0) t42=t22=t2=n2=gx2=gy2=0; // No influence
  else {
    Grad2(Mutate[ii+1+Mutate[jj+1]], &gx2, &gy2);
    t22=t2*t2;
    t42=t22*t22;
    n2=t42*(gx2*x2+gy2*y2);
  }
  double Noise=n0+n1+n2; // Add contributions from each corner to get the final noise value.
  if(dx && dy) { /* Compute derivative:
    *dx =-8 * t20 * t0 * x0 * (gx0 * x0 + gy0 * y0) + t40 * gx0;
    *dy =-8 * t20 * t0 * y0 * (gx0 * x0 + gy0 * y0) + t40 * gy0;
    *dx+=-8 * t21 * t1 * x1 * (gx1 * x1 + gy1 * y1) + t41 * gx1;
    *dy+=-8 * t21 * t1 * y1 * (gx1 * x1 + gy1 * y1) + t41 * gy1;
    *dx+=-8 * t22 * t2 * x2 * (gx2 * x2 + gy2 * y2) + t42 * gx2;
    *dy+=-8 * t22 * t2 * y2 * (gx2 * x2 + gy2 * y2) + t42 * gy2; Optimises to: */
    double temp0=t20*t0*(gx0* x0 + gy0*y0);
    *dx=temp0*x0;
    *dy=temp0*y0;
    double temp1=t21*t1*(gx1*x1 + gy1*y1);
    *dx+=temp1*x1;
    *dy+=temp1*y1;
    double temp2=t22*t2*(gx2* x2 + gy2*y2);
    *dx+=temp2*x2;
    *dy+=temp2*y2;
    *dx*=-8;
    *dy*=-8;
    *dx+=t40*gx0 + t41*gx1 + t42*gx2;
    *dy+=t40*gy0 + t41*gy1 + t42*gy2;
    *dx*=70.1605; /* Scale derivative to match the noise scaling */
    *dy*=70.1605;
  }
  return MinMax(-1.0, Noise*70.1605-0.000142584, 1.0); // The result is scaled to return values in the interval [-1,1].
}

// Helper function to compute gradients.
void Simplex::Grad3(int Hash, double* gx, double* gy, double* gz) {
  /* Gradient directions for 3D.
  These vectors are based on the midpoints of the 12 edges of a cube.
  A larger array of random unit length vectors would also do the job, but these 12
  (including 4 repeats to make the array length a power of two) work better.
  They are not random, they are carefully chosen to represent a small, isotropic set of directions. */
  static double grad3lut[16][3]={
    { 1,  0,  1}, { 0,  1,  1}, {-1,  0,  1}, { 0, -1,  1}, // 12 cube edges
    { 1,  0, -1}, { 0,  1, -1}, {-1,  0, -1}, { 0, -1, -1},
    { 1, -1,  0}, { 1,  1,  0}, {-1,  1,  0}, {-1, -1,  0},
    { 1,  0,  1}, {-1,  0,  1}, { 0,  1, -1}, { 0, -1, -1} // 4 repeats to make 16
  };
  int h=Hash & 15;
  *gx=grad3lut[h][0];
  *gy=grad3lut[h][1];
  *gz=grad3lut[h][2];
}

// 3D simplex noise with derivatives.
// If the last tthree arguments are not null, the analytic derivative (the 3D gradient of the scalar noise field) is also calculated.
double Simplex::Noise3(double x, double y, double z, double* dx/*=0*/, double* dy/*=0*/, double* dz/*=0*/) {
  // Skew the input space to determine which simplex cell we're in:
  double s=(x+y+z)/3; // Simple 1/3 skew factor for 3D
  double xs=x+s;
  double ys=y+s;
  double zs=z+s;
  int i=Floor(xs); // Standard floor flushes the processor pipeline.
  int j=Floor(ys);
  int k=Floor(zs);
  double t=(i+j+k)/6.0; // Simple 1/6 skew factor for 3D
  double X0=i-t; // Unskew the cell origin back to (x,y,z) space:
  double Y0=j-t;
  double Z0=k-t;
  double x0=x-X0; // The x,y,z distances from the cell origin
  double y0=y-Y0;
  double z0=z-Z0;
  // For the 3D case, the simplex shape is a slightly irregular tetrahedron. Determine which simplex we are in.
  int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
  int i2, j2, k2; // Offsets for  third corner of simplex in (i,j,k) coords
  if(x0>=y0) { // TODO: This code would benefit from a backport from the GLSL version!
         if(y0>=z0) {i1=1; j1=0; k1=0; i2=1; j2=1; k2=0;} // X Y Z order
    else if(x0>=z0) {i1=1; j1=0; k1=0; i2=1; j2=0; k2=1;} // X Z Y order
    else            {i1=0; j1=0; k1=1; i2=1; j2=0; k2=1;} // Z X Y order
   }else{ // x0<y0
         if(y0<z0) {i1=0; j1=0; k1=1; i2=0; j2=1; k2=1;} // Z Y X order
    else if(x0<z0) {i1=0; j1=1; k1=0; i2=0; j2=1; k2=1;} // Y Z X order
    else           {i1=0; j1=1; k1=0; i2=1; j2=1; k2=0;} // Y X Z order
  }
/* A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
   a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
   a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where c=1/6. */
  double x1=x0-i1+1/6.; // Offsets for second corner in (x,y,z) coords
  double y1=y0-j1+1/6.;
  double z1=z0-k1+1/6.;
  double x2=x0-i2+2/6.; // Offsets for third corner in (x,y,z) coords
  double y2=y0-j2+2/6.;
  double z2=z0-k2+2/6.;
  double x3=x0-1 +3/6.; // Offsets for last corner in (x,y,z) coords
  double y3=y0-1 +3/6.;
  double z3=z0-1 +3/6.;
  // Wrap the integer indices at 256, to avoid indexing Mutate[] out of bounds
  int ii=i & 0xFF;
  int jj=j & 0xFF;
  int kk=k & 0xFF;
  // Calculate the contribution from the four corners
  double gx0, gy0, gz0; // Gradients at simplex corners
  double t0=0.6f-x0*x0-y0*y0-z0*z0;
  double t20, t40;
  double n0, n1, n2, n3; // Noise contributions from the four simplex corners
  if(t0<0) n0=t0=t20=t40=gx0=gy0=gz0=0;
  else {
    Grad3(Mutate[ii+Mutate[jj+Mutate[kk]]], &gx0, &gy0, &gz0);
    t20=t0*t0;
    t40=t20*t20;
    n0=t40*(gx0*x0 + gy0*y0 + gz0*z0);
  }
  double gx1, gy1, gz1; // Gradients at simplex corners
  double t1=0.6f-x1*x1-y1*y1-z1*z1;
  double t21, t41;
  if(t1<0) n1=t1=t21=t41=gx1=gy1=gz1=0;
  else {
    Grad3(Mutate[ii+i1+Mutate[jj+j1+Mutate[kk+k1]]], &gx1, &gy1, &gz1);
    t21=t1*t1;
    t41=t21*t21;
    n1=t41*(gx1*x1 + gy1*y1 + gz1*z1);
  }
  double gx2, gy2, gz2; // Gradients at simplex corners
  double t2=0.6f-x2*x2-y2*y2-z2*z2;
  double t22, t42;
  if(t2<0) n2=t2=t22=t42=gx2=gy2=gz2=0;
  else {
    Grad3(Mutate[ii+i2+Mutate[jj+j2+Mutate[kk+k2]]], &gx2, &gy2, &gz2);
    t22=t2*t2;
    t42=t22*t22;
    n2=t42*(gx2*x2+gy2*y2+gz2*z2);
  }
  double gx3, gy3, gz3; // Gradients at simplex corners
  double t3=0.6f-x3*x3-y3*y3-z3*z3;
  double t23, t43;
  if(t3<0) n3=t3=t23=t43=gx3=gy3=gz3=0;
  else {
    Grad3(Mutate[ii+1+Mutate[jj+1+Mutate[kk+1]]], &gx3, &gy3, &gz3);
    t23=t3*t3;
    t43=t23*t23;
    n3=t43*(gx3*x3 + gy3*y3 + gz3*z3);
  }
  double Noise=n0+n1+n2+n3; // Add contributions from each corner to get the final noise value.
  if(dx && dy && dz) { /* Compute derivative:
      *dx=-8 * t20 * t0 * x0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gx0;
      *dy=-8 * t20 * t0 * y0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gy0;
      *dz=-8 * t20 * t0 * z0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gz0;
      *dx += -8 * t21 * t1 * x1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gx1;
      *dy += -8 * t21 * t1 * y1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gy1;
      *dz += -8 * t21 * t1 * z1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gz1;
      *dx += -8 * t22 * t2 * x2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gx2;
      *dy += -8 * t22 * t2 * y2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gy2;
      *dz += -8 * t22 * t2 * z2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gz2;
      *dx += -8 * t23 * t3 * x3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gx3;
      *dy += -8 * t23 * t3 * y3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gy3;
      *dz += -8 * t23 * t3 * z3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gz3; Optimises to: */
    double temp0=t20*t0*(gx0*x0 + gy0*y0 + gz0*z0);
    *dx=temp0*x0;
    *dy=temp0*y0;
    *dz=temp0*z0;
    double temp1=t21*t1*(gx1*x1 + gy1*y1 + gz1*z1);
    *dx+=temp1*x1;
    *dy+=temp1*y1;
    *dz+=temp1*z1;
    double temp2=t22*t2*(gx2*x2 + gy2*y2 + gz2*z2);
    *dx+=temp2*x2;
    *dy+=temp2*y2;
    *dz+=temp2*z2;
    double temp3=t23*t3*(gx3*x3 + gy3*y3 + gz3*z3);
    *dx+=temp3*x3;
    *dy+=temp3*y3;
    *dz+=temp3*z3;
    *dx*=-8;
    *dy*=-8;
    *dz*=-8;
    *dx+=t40*gx0 + t41*gx1 + t42*gx2 + t43*gx3;
    *dy+=t40*gy0 + t41*gy1 + t42*gy2 + t43*gy3;
    *dz+=t40*gz0 + t41*gz1 + t42*gz2 + t43*gz3;
    *dx*=16.9446; // Scale derivative to match the noise scaling:
    *dy*=16.9446;
    *dz*=16.9446;
  }
  return MinMax(-1.0, Noise*32.741+0.00104006, 1.0); // The result is scaled to return values in the range [-1,1]
}

// Helper function to compute gradients and gradients-dot-residualvectors.
void Simplex::Grad4(int Hash, double* gx, double* gy, double* gz, double* gw) {
  static double grad4lut[32][4]={ // 32 tesseract edges
    { 0,  1, 1, 1}, { 0,  1,  1, -1}, { 0,  1, -1, 1}, { 0,  1, -1, -1},
    { 0, -1, 1, 1}, { 0, -1,  1, -1}, { 0, -1, -1, 1}, { 0, -1, -1, -1},
    { 1,  0, 1, 1}, { 1,  0,  1, -1}, { 1,  0, -1, 1}, { 1,  0, -1, -1},
    {-1,  0, 1, 1}, {-1,  0,  1, -1}, {-1,  0, -1, 1}, {-1,  0, -1, -1},
    { 1,  1, 0, 1}, { 1,  1,  0, -1}, { 1, -1,  0, 1}, { 1, -1,  0, -1},
    {-1,  1, 0, 1}, {-1,  1,  0, -1}, {-1, -1,  0, 1}, {-1, -1,  0, -1},
    { 1,  1, 1, 0}, { 1,  1, -1,  0}, { 1, -1,  1, 0}, { 1, -1, -1,  0},
    {-1,  1, 1, 0}, {-1,  1, -1,  0}, {-1, -1,  1, 0}, {-1, -1, -1,  0}
  };
  int h=Hash & 31;
  *gx=grad4lut[h][0];
  *gy=grad4lut[h][1];
  *gz=grad4lut[h][2];
  *gw=grad4lut[h][3];
}

// 4D simplex noise with derivatives.
// If the last four arguments are not null, the analytic derivative (the 4D gradient of the scalar noise field) is also calculated.
double Simplex::Noise4(double x, double y, double z, double w, double* dx/*=0*/, double* dy/*=0*/, double* dz/*=0*/, double* dw/*=0*/) {
  // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
  double s=(x+y+z+w)*0.30901699437494742410229341718282; // (sqrt(5)-1)/4 Factor for 4D skewing
  double xs=x+s;
  double ys=y+s;
  double zs=z+s;
  double ws=w+s;
  int i=Floor(xs); // Standard floor flushes the processor pipeline.
  int j=Floor(ys);
  int k=Floor(zs);
  int l=Floor(ws);
  const double G4=0.13819660112501051517954131656344; // G4=(5-sqrt(5))/20
  double t=(i+j+k+l)*G4; // Factor for 4D unskewing
  double X0=i-t; // Unskew the cell origin back to (x,y,z,w) space
  double Y0=j-t;
  double Z0=k-t;
  double W0=l-t;
  double x0=x-X0;  // The x,y,z,w distances from the cell origin
  double y0=y-Y0;
  double z0=z-Z0;
  double w0=w-W0;
/* For the 4D case, the simplex is a tesseract (see 'Dimension' on Wikipedia http://en.wikipedia.org/wiki/Dimension).
To find out which of the 24 possible simplices is current, determine the magnitude ordering of x0, y0, z0 and w0.
The method below is a reasonable way of finding the ordering of x,y,z,w and then finding the correct traversal order for the simplex we’re in.
First, six pair-wise comparisons are performed between each possible pair of the four coordinates,
and then the results are used to add up binary bits for an integer index into a precomputed lookup table, simplex[]. */
  int c1=(x0>y0) ? 32 : 0;
  int c2=(x0>z0) ? 16 : 0;
  int c3=(y0>z0) ?  8 : 0;
  int c4=(x0>w0) ?  4 : 0;
  int c5=(y0>w0) ?  2 : 0;
  int c6=(z0>w0) ?  1 : 0;
  int c=c1 & c2 & c3 & c4 & c5 & c6; /* '&' is mostly faster than '+'
simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w impossible.
Only the 24 indices which have non-zero entries make any sense. */
// A lookup table to traverse the simplex around a given point in 4D.
// Details can be found where this table is used, in the 4D noise method.
  static unsigned char Simplex[64][4]={
    {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
    {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
    {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
    {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
  };
/* We use a thresholding to set the coordinates in turn from the largest magnitude.
The number 3 in the "simplex" array is at the position of the largest coordinate. */
  int i1=Simplex[c][0]>=3 ? 1 : 0; // The integer offsets for the second simplex corner
  int j1=Simplex[c][1]>=3 ? 1 : 0;
  int k1=Simplex[c][2]>=3 ? 1 : 0;
  int l1=Simplex[c][3]>=3 ? 1 : 0;
  // The number 2 in the "simplex" array is at the second largest coordinate.
  int i2=Simplex[c][0]>=2 ? 1 : 0; // The integer offsets for the third simplex corner
  int j2=Simplex[c][1]>=2 ? 1 : 0;
  int k2=Simplex[c][2]>=2 ? 1 : 0;
  int l2=Simplex[c][3]>=2 ? 1 : 0;
  // The number 1 in the "simplex" array is at the second smallest coordinate.
  int i3=Simplex[c][0]>=1 ? 1 : 0; // The integer offsets for the fourth simplex corner
  int j3=Simplex[c][1]>=1 ? 1 : 0;
  int k3=Simplex[c][2]>=1 ? 1 : 0;
  int l3=Simplex[c][3]>=1 ? 1 : 0;
  // The fifth corner has all coordinate offsets=1, so no need to look that up.
  double x1=x0-i1+  G4; // Offsets for second corner in (x,y,z,w) coords
  double y1=y0-j1+  G4;
  double z1=z0-k1+  G4;
  double w1=w0-l1+  G4;
  double x2=x0-i2+2*G4; // Offsets for third corner in (x,y,z,w) coords
  double y2=y0-j2+2*G4;
  double z2=z0-k2+2*G4;
  double w2=w0-l2+2*G4;
  double x3=x0-i3+3*G4; // Offsets for fourth corner in (x,y,z,w) coords
  double y3=y0-j3+3*G4;
  double z3=z0-k3+3*G4;
  double w3=w0-l3+3*G4;
  double x4=x0- 1+4*G4; // Offsets for last corner in (x,y,z,w) coords
  double y4=y0- 1+4*G4;
  double z4=z0- 1+4*G4;
  double w4=w0- 1+4*G4;
  // Wrap the integer indices at 256, to avoid indexing Mutate[] out of bounds
  int ii=i & 0xFF;
  int jj=j & 0xFF;
  int kk=k & 0xFF;
  int ll=l & 0xFF;
  // Calculate the contribution from the five corners
  double n0, t20, t40, gx0, gy0, gz0, gw0; // Noise contribution, Gradients at simplex corners
  double t0=0.6f-x0*x0-y0*y0-z0*z0-w0*w0;
  if(t0<0) n0=t0=t20=t40=gx0=gy0=gz0=gw0=0;
  else {
    t20=t0*t0;
    t40=t20*t20;
    Grad4(Mutate[ii+Mutate[jj+Mutate[kk+Mutate[ll]]]], &gx0, &gy0, &gz0, &gw0);
    n0=t40*(gx0*x0 + gy0*y0 + gz0*z0 + gw0*w0);
  }
  double n1, t21, t41, gx1, gy1, gz1, gw1; // Noise contribution, Gradients at simplex corners
  double t1=0.6f-x1*x1-y1*y1-z1*z1-w1*w1;
  if(t1<0) n1=t1=t21=t41=gx1=gy1=gz1=gw1=0;
  else {
    t21=t1*t1;
    t41=t21*t21;
    Grad4(Mutate[ii+i1+Mutate[jj+j1+Mutate[kk+k1+Mutate[ll+l1]]]], &gx1, &gy1, &gz1, &gw1);
    n1=t41*(gx1*x1 + gy1*y1 + gz1*z1 + gw1*w1);
  }
  double n2, t22, t42, gx2, gy2, gz2, gw2; // Noise contribution, Gradients at simplex corners
  double t2=0.6f-x2*x2-y2*y2-z2*z2-w2*w2;
  if(t2<0) n2=t2=t22=t42=gx2=gy2=gz2=gw2=0;
  else {
    t22=t2*t2;
    t42=t22*t22;
    Grad4(Mutate[ii+i2+Mutate[jj+j2+Mutate[kk+k2+Mutate[ll+l2]]]], &gx2, &gy2, &gz2, &gw2);
    n2=t42*(gx2*x2 + gy2*y2 + gz2*z2 + gw2*w2);
  }
  double n3, t23, t43, gx3, gy3, gz3, gw3; // Noise contribution, Gradients at simplex corners
  double t3=0.6f-x3*x3-y3*y3-z3*z3-w3*w3;
  if(t3<0) n3=t3=t23=t43=gx3=gy3=gz3=gw3=0;
  else {
    t23=t3*t3;
    t43=t23*t23;
    Grad4(Mutate[ii+i3+Mutate[jj+j3+Mutate[kk+k3+Mutate[ll+l3]]]], &gx3, &gy3, &gz3, &gw3);
    n3=t43*(gx3*x3 + gy3*y3 + gz3*z3 + gw3*w3);
  }
  double n4, t24, t44, gx4, gy4, gz4, gw4; // Noise contribution, Gradients at simplex corners
  double t4=0.6f-x4*x4-y4*y4-z4*z4-w4*w4;
  if(t4<0) n4=t4=t24=t44=gx4=gy4=gz4=gw4=0;
  else {
    t24=t4*t4;
    t44=t24*t24;
    Grad4(Mutate[ii+1+Mutate[jj+1+Mutate[kk+1+Mutate[ll+1]]]], &gx4, &gy4, &gz4, &gw4);
    n4=t44*(gx4*x4 + gy4*y4 + gz4*z4 + gw4*w4);
  }
  double Noise=n0+n1+n2+n3+n4; // Add contributions from each corner to get the final noise value.
  if(dx && dy && dz && dw) { /* Compute derivative:
      *dx=-8 * t20 * t0 * x0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gx0;
      *dy=-8 * t20 * t0 * y0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gy0;
      *dz=-8 * t20 * t0 * z0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gz0;
      *dw=-8 * t20 * t0 * w0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gw0;
      *dx += -8 * t21 * t1 * x1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gx1;
      *dy += -8 * t21 * t1 * y1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gy1;
      *dz += -8 * t21 * t1 * z1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gz1;
      *dw=-8 * t21 * t1 * w1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gw1;
      *dx += -8 * t22 * t2 * x2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gx2;
      *dy += -8 * t22 * t2 * y2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gy2;
      *dz += -8 * t22 * t2 * z2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gz2;
      *dw += -8 * t22 * t2 * w2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gw2;
      *dx += -8 * t23 * t3 * x3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gx3;
      *dy += -8 * t23 * t3 * y3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gy3;
      *dz += -8 * t23 * t3 * z3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gz3;
      *dw += -8 * t23 * t3 * w3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gw3;
      *dx += -8 * t24 * t4 * x4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gx4;
      *dy += -8 * t24 * t4 * y4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gy4;
      *dz += -8 * t24 * t4 * z4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gz4;
      *dw += -8 * t24 * t4 * w4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gw4; Optimises to: */
    double temp0=t20*t0*(gx0*x0+gy0*y0+gz0*z0+gw0*w0);
    *dx=temp0*x0;
    *dy=temp0*y0;
    *dz=temp0*z0;
    *dw=temp0*w0;
    double temp1=t21*t1*(gx1*x1+gy1*y1+gz1*z1+gw1*w1);
    *dx+=temp1*x1;
    *dy+=temp1*y1;
    *dz+=temp1*z1;
    *dw+=temp1*w1;
    double temp2=t22*t2*(gx2*x2+gy2*y2+gz2*z2+gw2*w2);
    *dx+=temp2*x2;
    *dy+=temp2*y2;
    *dz+=temp2*z2;
    *dw+=temp2*w2;
    double temp3=t23*t3*(gx3*x3+gy3*y3+gz3*z3+gw3*w3);
    *dx+=temp3*x3;
    *dy+=temp3*y3;
    *dz+=temp3*z3;
    *dw+=temp3*w3;
    double temp4=t24*t4*(gx4*x4+gy4*y4+gz4*z4+gw4*w4);
    *dx+=temp4*x4;
    *dy+=temp4*y4;
    *dz+=temp4*z4;
    *dw+=temp4*w4;
    *dx*=-8;
    *dy*=-8;
    *dz*=-8;
    *dw*=-8;
    *dx+=t40*gx0+t41*gx1+t42*gx2+t43*gx3+t44*gx4;
    *dy+=t40*gy0+t41*gy1+t42*gy2+t43*gy3+t44*gy4;
    *dz+=t40*gz0+t41*gz1+t42*gz2+t43*gz3+t44*gz4;
    *dw+=t40*gw0+t41*gw1+t42*gw2+t43*gw3+t44*gw4;
    *dx*=27.2568; // Scale derivative to match the noise scaling:
    *dy*=27.2568;
    *dz*=27.2568;
    *dw*=27.2568;
  }
  return MinMax(-1.0, Noise*27.2568+0.000252695, 1.0); // The result is scaled to return values in the range [-1,1]
}
