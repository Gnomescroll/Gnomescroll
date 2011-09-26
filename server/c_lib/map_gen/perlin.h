#pragma once

#include "_noise.h"

float pnoise1(float x, const int repeat, const int base);
float pnoise2(float x, float y, const float repeatx, const float repeaty, const int base);
float pnoise3(float x, float y, float z, const int repeatx, const int repeaty, const int repeatz, const int base);
