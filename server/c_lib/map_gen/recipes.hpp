#pragma once

#include <map_gen/map_generator.hpp>

namespace MapRecipes
{

void simple_map()
{
    using MapGen::generator;
    
    generator->max_size();
    //generator->size(512, 512, 128);
    //generator->tile((char*)"holy_stone");
    generator->tile(54);
    generator->interpolate(4,4,1);
    generator->scale(4.0f, 4.0f, 1.0f);
    generator->enable_heightmap(20, 30);
    generator->p2();
    generator->set_noise_parameters(6, 0.35f, 1.0f, 0.1f, 2.0f);
    generator->start();
    
    MapGen::reset();
}

} // MapRecipes
