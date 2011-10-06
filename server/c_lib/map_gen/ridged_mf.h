#pragma once

#include "perlin.h"
#include "simplex.h"

float rmf_perlin1(float rmf_x, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeat, int base);
float rmf_perlin2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int base);
float rmf_perlin3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int repeatz, int base);

float rmf_simplex2(float rmf_x, float rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude);
float rmf_simplex3(float rmf_x, float rmf_y, float rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude);

void rmf_perlin1_fill(int rmf_x, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeat, int base);
void rmf_perlin2_fill(int rmf_x, int rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int base);
void rmf_perlin3_fill(int rmf_x, int rmf_y, int rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude, int repeatx, int repeaty, int repeatz, int base);
void rmf_simplex2_fill(int rmf_x, int rmf_y, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude);
void rmf_simplex3_fill(int rmf_x, int rmf_y, int rmf_z, int octaves, float rmf_persistence, float rmf_frequency, float rmf_amplitude);
