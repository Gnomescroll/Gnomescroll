#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void populate_2d_noise_array(float* _2d_noise_array, float persistence, int octaves);

// NOTE: caller must free() the return value if not NULL
float* create_2d_noise_array(const float persistence, const int octaves, const unsigned int x, const unsigned int y);

void init();
void teardown();

}   // t_gen
