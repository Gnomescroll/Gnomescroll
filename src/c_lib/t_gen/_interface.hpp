#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void populate_2d_noise_array(float* _2d_noise_array, float persistence,
                             int octaves, bool change_seed=true);

// NOTE: caller must free() the return value if not NULL
float* create_2d_noise_array(float persistence, int octaves, size_t x, size_t y,
                             bool change_seed=true);

void init();
void teardown();

}   // t_gen
