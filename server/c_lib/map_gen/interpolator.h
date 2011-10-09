#pragma once

#include "perlin.h"
#include "simplex.h"
#include "ridged_mf.h"

void apply_interp3(int x, int y, int z,
                    int x_interval, int y_interval, int z_interval,
                    int oct,                  // noise func args
                    float pers, float amp,
                    float lac, float freq,
                    int rep_x, int rep_y, int rep_z,
                    int base);

void apply_interp2_perlin(int x, int y, int x_interval, int y_interval,
                                                    int oct,    // noise func args
                                                    float pers, float amp,
                                                    float lac, float freq,
                                                    int rep_x, int rep_y,
                                                    int base);

void apply_interp2_rmf_perlin(int x, int y, int x_interval, int y_interval,
                                                    int oct,    // noise func args
                                                    float pers, float amp,
                                                    float lac, float freq,
                                                    int rep_x, int rep_y,
                                                    int base);

void apply_interp1(int x, int x_interval,
                    int oct, //noise args
                    float pers, float amp,
                    float lac, float freq,
                    int repeat, int base);

