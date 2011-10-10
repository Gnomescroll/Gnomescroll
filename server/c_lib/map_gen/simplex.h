#pragma once

#include "noise.h"

float snoise2(float x, float y);
float snoise3(float x, float y, float z);

float simplex2(float x, float y);
float simplex3(float x, float y, float z);

void simplex2_fill(int x, int y);
void simplex3_fill(int x, int y, int z);
