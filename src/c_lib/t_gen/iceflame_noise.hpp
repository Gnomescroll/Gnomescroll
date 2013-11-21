/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/random.hpp>

#if DC_CLIENT
#error DO NOT INCLUDE THIS FILE IN CLIENT
#endif

namespace t_gen
{

const size_t N_WAVES = 20;

struct WaveSetup
{
    float amp;
    float length;
    float shift;
};

struct WaveData2D
{
    WaveSetup x[N_WAVES];
    WaveSetup y[N_WAVES];
};

WaveData2D wave_data;

int wave_divisor[] = {2, 4, 8, 16, 32, 64, 128, 512};

static float* iceflame_noise = NULL;

void iceflame_noise_init()
{
    GS_ASSERT(iceflame_noise == NULL); // this makes sure we don't init() twice
     iceflame_noise = (float*)calloc(map_dim.x * map_dim.y, sizeof(float));
}

void iceflame_noise_teardown()
{
    free(iceflame_noise);
}

inline float get_iceflame_noise(int x, int y)
{
    int index = x + map_dim.x * y;
    IF_ASSERT(index < 0 || index >= map_dim.x * map_dim.y)
        return 0;
    return iceflame_noise[index];
}

void make_iceflame_noise()
{
    for(size_t n = 0; n < N_WAVES; n++)
    {
        wave_data.x[n].length = wave_divisor[randrange(0, sizeof(wave_divisor) / sizeof(*wave_divisor) - 1)];
        wave_data.x[n].amp = randrange(1, wave_data.x[n].length);
        wave_data.x[n].shift = randrange(-10, 10);
        wave_data.y[n].length = wave_divisor[randrange(0, sizeof(wave_divisor) / sizeof(*wave_divisor) - 1)];
        wave_data.y[n].amp = randrange(1, wave_data.y[n].length);
        wave_data.y[n].shift = randrange(-10, 10);
    }
    for(int x = 0; x < map_dim.x; x++)
    for(int y = 0; y < map_dim.y; y++)
    {
        iceflame_noise[x + y * map_dim.x] = 0;
        for(size_t times = 0; times < N_WAVES; times++)
        {
            iceflame_noise[x + y * map_dim.x] += sinf(x * 360 / wave_data.x[times].length + wave_data.x[times].shift) * wave_data.x[times].amp;
            iceflame_noise[x + y * map_dim.x] += sinf(y * 360 / wave_data.y[times].length + wave_data.y[times].shift) * wave_data.y[times].amp;
        }
    }
}

void shuffle_iceflame_noise(int amount)
{
    for(size_t done = 0; done < N_WAVES; done++)
    {
        wave_data.x[done].shift += randrange(0 - amount, amount);
        wave_data.y[done].shift += randrange(0 - amount, amount);
    }
    for(int x = 0; x < map_dim.x; x++)
    for(int y = 0; y < map_dim.y; y++)
    {
        iceflame_noise[x + y * map_dim.x] = 0;
        for(size_t times = 0; times < N_WAVES; times++)
        {
            iceflame_noise[x + y * map_dim.x] += sinf(x * 360 / wave_data.x[times].length + wave_data.x[times].shift) * wave_data.x[times].amp;
            iceflame_noise[x + y * map_dim.x] += sinf(y * 360 / wave_data.y[times].length + wave_data.y[times].shift) * wave_data.y[times].amp;
        }
    }
}

}//no longer t_gen
