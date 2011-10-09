#pragma once

#include "noise.h"

float pnoise1(float x, const int repeat, const int base);
float pnoise2(float x, float y, const float repeatx, const float repeaty, const int base);
float pnoise3(float x, float y, float z, const int repeatx, const int repeaty, const int repeatz, const int base);

float perlin1(float x, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeat, int base);
float perlin2(float x, float y, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeatx, int repeaty, int base);
float perlin3(float x, float y, float z, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeatx, int repeaty, int repeatz, int base);

void perlin1_fill(int x, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeat, int base);
void perlin2_fill(int x, int y, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeatx, int repeaty, int base);
void perlin3_fill(int x, int y, int z, int octaves, float persistence, float amplitude, float lacunarity, float frequency, int repeatx, int repeaty, int repeatz, int base);
