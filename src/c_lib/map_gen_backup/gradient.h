#pragma once

void apply_grad3(float* noisemap, int x, int y, int z, float x0, float x1, float y0, float y1, float z0, float z1);
void apply_grad2(float* noisemap, int x, int y, float x0, float x1, float y0, float y1);
void apply_grad1(float* noisemap, int x, float x0, float x1);

void apply_grad3_falloff(float* noisemap, int x, int y, int z, int x_dir, int y_dir, int z_dir);
