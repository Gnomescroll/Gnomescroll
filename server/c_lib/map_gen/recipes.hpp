#pragma once

#include <map_gen/map_generator.hpp>

namespace MapRecipes
{

//void simple_map()
//{
    //using MapGen::generator;
    
    //generator->max_size();
    ////generator->size(512, 512, 128);
    ////generator->tile((char*)"holy_stone");
    //generator->tile(54);
    //generator->interpolate(4,4,1);
    //generator->scale(4.0f, 4.0f, 1.0f);
    //generator->enable_heightmap(20, 30);
    //generator->p2();

    //int octaves = 6;
    //float persistence = 0.35f;
    //float amplitude = 1.0f;
    //float frequency = 0.1f;
    //float lacunarity = 2.0f;
    
    //generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);

    //generator->start();
    
    //MapGen::reset();
//}

void simple_map()
{
    using MapGen::generator;
    
    //generator->max_size();
    generator->size(128,128,128);
    //generator->tile((char*)"holy_stone");
    generator->tile(20);
    generator->interpolate(2,2,2);
    generator->scale(1.0f, 1.0f, 1.0f);
    generator->p3();
    generator->density(0.1);
    generator->rmf();

    int octaves = 6;
    float persistence = 0.9f;
    float amplitude = 1.2f;
    float frequency = 2.0f;
    float lacunarity = 2.0f;

    generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);
    generator->start();
    
    MapGen::reset();
}



} // MapRecipes
