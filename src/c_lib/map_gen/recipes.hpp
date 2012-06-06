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
    int tile = t_map::get_cube_id((char*)"regolith");
    generator->tile(tile);
    generator->interpolate(4,4,1);
    generator->scale(4.0f, 4.0f, 1.0f);
    //generator->enable_heightmap(20, 35);
    generator->enable_heightmap(20, 35);
    generator->repeat(512,512,128);

    generator->p2();

    int octaves = 6;
    float persistence = 0.35f;
    float amplitude = 1.0f;
    float frequency = 0.1f;
    float lacunarity = 2.0f;
    
    generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);

    generator->start();

    //octaves = 1;
    persistence = 0.9f;
    frequency = 2.0f;
    generator->enable_heightmap(20,27);
    generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);
    generator->scale(16.0f,8.0f,1.0f);
    generator->start();
    
    MapGen::reset();


    //void erosion(const int x, const int y, const int passes, const int h_diff, const int max_iters);
    //map_gen::erosion(512,512, 1, 3, 10);

    //int x = 512, y = 512, z = 128;
    //float* noisemap = create_noisemap(x,y,1);
    //_oct = 1;
    //perlin2_fill(noisemap, x,y, 512, 512, tile);
    //set_terrain_height(noisemap, x,y,z, 0, z, tile);
    //destroy_noisemap(noisemap);
}

//void simple_map()
//{
    //using MapGen::generator;
    
    ////generator->max_size();
    //generator->size(128,128,128);
    ////generator->tile((char*)"holy_stone");
    //generator->tile(1);
    //generator->interpolate(8,8,4);
    //generator->scale(2.0f, 2.0f, 4.0f);
    //generator->p3();
    //generator->density(0.95);
    //generator->rmf();
    //generator->gradient(-1,1,-1,1,-0.5,0);

    //int octaves = 8;
    //float persistence = 0.9f;
    //float amplitude = 1.3f;
    //float frequency = 2.1f;
    //float lacunarity = 1.0f;

    //generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);
    //generator->start();
    //generator->start();
    //generator->start();
    
    //////generator->max_size();
    ////generator->size(128,128,128);
    //////generator->tile((char*)"holy_stone");
    ////generator->tile(1);
    ////generator->interpolate(8,8,4);
    ////generator->scale(2.0f, 2.0f, 4.0f);
    ////generator->p3();
    ////generator->density(0.95);
    ////generator->rmf();
    ////generator->gradient(1,1,1,1,-0.5,0);

    ////octaves = 8;
    ////persistence = 0.4f;
    ////amplitude = 1.1f;
    ////frequency = 2.8f;
    ////lacunarity = 1.5f;

    ////generator->set_noise_parameters(octaves, persistence, amplitude, frequency, lacunarity);
    ////generator->start();
    
    //MapGen::reset();
//}



} // MapRecipes
