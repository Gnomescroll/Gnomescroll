#pragma once



namespace t_gen
{

void populate_2d_noise_array(float* _2d_noise_array, unsigned long seed, float persistance, int octaves);

void save_png(const char* filename, float*in, int xres, int yres);
void save_perlin(const char* filename, float* in, int xres, int yres);

}