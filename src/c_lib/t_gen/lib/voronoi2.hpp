#pragma once

double Vo(double x, double y, double z, int a, int b);

void init_voronoi_noise_maps();
void dump_voronoi_to_disc();


unsigned char voronoi_char(float x, float y, float z);
float voronoi_float_fast(int x, int y, int z);
float voronoi_float(float x, float y, float z);

