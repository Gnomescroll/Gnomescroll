#pragma once

#include "noise.h"

float snoise2(float x, float y);
float snoise3(float x, float y, float z);

float simplex2(float x, float y, int octaves, float persistence, float frequency, float amplitude);
float simplex3(float x, float y, float z, int octaves, float persistence, float frequency, float amplitude);

void simplex2_fill(int x, int y, int octaves, float persistence, float frequency, float amplitude);
void simplex3_fill(int x, int y, int z, int octaves, float persistence, float frequency, float amplitude);
