#pragma once

#include <t_gen/lib/voronoi.hpp>


double Vo(double x, double y, double z, int a, int b);
unsigned char voronoi_char(double x, double y, double z);


void dump_voronoi_to_disc();
