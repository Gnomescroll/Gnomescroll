#pragma once

#include "perlin.h"
#include "simplex.h"
#include "ridged_mf.h"

/* 1d */

void apply_interp1_perlin(int x, int x_interval,
                            int repeat, int base);
                            
void apply_interp1_rmf_perlin(int x, int x_interval,
                            int repeat, int base);
/* 2d */

void apply_interp2_perlin(int x, int y, int x_interval, int y_interval,
                            int rep_x, int rep_y,
                            int base);

void apply_interp2_rmf_perlin(int x, int y, int x_interval, int y_interval,
                            int rep_x, int rep_y,
                            int base);

void apply_interp2_simplex(int x, int y, int x_interval, int y_interval);
                            
void apply_interp2_rmf_simplex(int x, int y, int x_interval, int y_interval);
/* 3d */

void apply_interp3_perlin(int x, int y, int z,
                            int x_interval, int y_interval, int z_interval,
                            int rep_x, int rep_y, int rep_z,
                            int base);
                            
void apply_interp3_rmf_perlin(int x, int y, int z,
                            int x_interval, int y_interval, int z_interval,
                            int rep_x, int rep_y, int rep_z,
                            int base);

void apply_interp3_simplex(int x, int y, int z,
                            int x_interval, int y_interval, int z_interval);
                            
void apply_interp3_rmf_simplex(int x, int y, int z,
                            int x_interval, int y_interval, int z_interval);
