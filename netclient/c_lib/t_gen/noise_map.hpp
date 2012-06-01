#pragma once


namespace t_gen
{

extern float noise_map_2d_float[512*512 + 1];
extern unsigned char noise_map_2d_char[512*512 + 1];

void save_png(const char* filename, float*in, int xres, int yres);

void test();

void gen_map();

}