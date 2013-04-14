#include "_interface.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/noise_map2.hpp>
#include <t_gen/explosives.hpp>
#include <t_gen/city_generator.hpp>

typedef unsigned char Uint8;

namespace t_gen
{

void init()
{
    init_explosives();
    init_cities();
}

void teardown()
{

}

void populate_2d_noise_array(float* _2d_noise_array, float persistence, int octaves)
{
    seed_twister(rand());
    class PerlinOctave2D* p2d = new class PerlinOctave2D(octaves);

    float sum = 0.0f;
    for (int i=0; i<map_dim.x; i++)
    for (int j=0; j<map_dim.y; j++)
    {
        float _x = i*(1.0f/float(map_dim.x)); // + (0.5/512.0);
        float _y = j*(1.0f/float(map_dim.y)); // + (0.5/512.0);

        float tmp = p2d->sample(_x,_y,persistence);
        _2d_noise_array[XMAX*j + i] = tmp;
        sum += tmp;
    }

    delete p2d;
}

// NOTE: caller must free() the return value if not NULL
float* create_2d_noise_array(const float persistence, const int octaves, const unsigned int x, const unsigned int y)
{
    int size = x*y;
    IF_ASSERT(size <= 0) return NULL;

    float* noise = (float*)malloc(size * sizeof(float));
    IF_ASSERT(noise == NULL) return NULL;

    seed_twister(rand());

    const float xscale = 1.0f/float(x);
    const float yscale = 1.0f/float(y);

    class PerlinOctave2D* pgen = new class PerlinOctave2D(octaves);

    for (unsigned int i=0; i<x; i++)
    for (unsigned int j=0; j<y; j++)
        noise[i + x*j] = pgen->sample(i * xscale, j * yscale, persistence);

    delete pgen;

    return noise;
}

}   // t_gen
