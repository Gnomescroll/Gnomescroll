#pragma once

float pnoise1(float x, const int repeat, const int base);
float pnoise2(float x, float y, const float repeatx, const float repeaty, const int base);
float pnoise3(float x, float y, float z, const int repeatx, const int repeaty, const int repeatz, const int base);

float perlin1(float x, int repeat, int base);
float perlin2(float x, float y, int repeatx, int repeaty, int base);
float perlin3(float x, float y, float z, int repeatx, int repeaty, int repeatz, int base);

void perlin1_fill(float* noisemap, int x, int repeat, int base);
void perlin2_fill(float* noisemap, int x, int y, int repeatx, int repeaty, int base);
void perlin3_fill(float* noisemap, int x, int y, int z, int repeatx, int repeaty, int repeatz, int base);

