#include "_interface.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_gen/noise_map2.hpp>
#include <t_gen/explosives.hpp>
#include <t_gen/city_generator.hpp>

typedef unsigned char Uint8;

namespace t_gen
{

static void measure_noise_range(size_t size)
{
    float highest = -100000.0f;
    float lowest = 100000.0f;
    float* noise = create_2d_noise_array(1.0f, 1, size, size, true);
    for (size_t i=0; i<size; i++)
    for (size_t j=0; j<size; j++)
    {
        float n = noise[i + size*j];
        if (n > highest)
            highest = n;
        if (n < lowest)
            lowest = n;
    }
    printf("Lowest: %0.2f\n", lowest);
    printf("Highest: %0.2f\n", highest);
    free(noise);
}

void init()
{
    init_explosives();
    init_cities();

    //int tries = 10;
    //for (int i=0; i<tries; i++)
        //measure_noise_range(4096);
}

void teardown()
{

}

void populate_2d_noise_array(float* _2d_noise_array, float persistence,
                             int octaves, bool change_seed)
{
    class PerlinOctave2D* p2d = new class PerlinOctave2D(octaves, change_seed);

    float sum = 0.0f;
    for (int i=0; i<map_dim.x; i++)
    for (int j=0; j<map_dim.y; j++)
    {
        float _x = i*(1.0f/float(map_dim.x)); // + (0.5/map_dim.x);
        float _y = j*(1.0f/float(map_dim.y)); // + (0.5/map_dim.y);

        float tmp = p2d->sample(_x,_y, persistence);
        _2d_noise_array[map_dim.x*j + i] = tmp;
        sum += tmp;
    }

    delete p2d;
}

// NOTE: caller must free() the return value if not NULL
float* create_2d_noise_array(float persistence, int octaves, size_t x, size_t y,
                             bool change_seed)
{
    int size = x*y;
    IF_ASSERT(size <= 0) return NULL;

    float* noise = (float*)malloc(size * sizeof(float));
    IF_ASSERT(noise == NULL) return NULL;

    const float xscale = 1.0f/float(x);
    const float yscale = 1.0f/float(y);

    class PerlinOctave2D* pgen = new class PerlinOctave2D(octaves, change_seed);

    for (size_t i=0; i<x; i++)
    for (size_t j=0; j<y; j++)
        noise[i + x*j] = pgen->sample(i * xscale, j * yscale, persistence);

    delete pgen;

    return noise;
}

}   // t_gen
