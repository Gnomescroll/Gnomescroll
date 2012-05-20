// Voronoi.h

#pragma once

// The speed of this depends on the DistanceMethod which is called 27 times per call to Get

struct Voronoi {
  enum VoronoiType {First, Second, Third, Fourth, Difference21, Difference32, Crackle};
  enum DistanceMethod {Length, Length2, Manhattan, Chebychev, Quadratic, Minkowski4, Minkowski5};
  static double Get(double x, double y, double z, VoronoiType V, DistanceMethod Distance); // Returns in the interval [0,1] but may exceed this!
private:
  static void Calculate(double x, double y, double z, double da[4], double pa[12], DistanceMethod Distance);
  static const float* GetPoint(int x, int y, int z);
};

