#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

void generate_rock_layer()
{
    float* _2d_noise_array = new float [512*512];
    int* regolith_depth_array = new int[512*512];

    const float persistence = 0.99f;
    const int octaves = 8;

    t_gen::populate_2d_noise_array(_2d_noise_array, persistence, octaves);

    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
    {
        regolith_depth_array[512*j+i] = 9.0f + 9.0f*(0.5+_2d_noise_array[512*j+i]);
    }

    int regolith = t_map::get_cube_type("regolith");
    //int rock_layer = t_map::get_cube_type("rock");
    //int rock_layer = t_map::get_cube_type("space_tree_trunk");
    int rock_layer = t_map::get_cube_type("rock");

    struct t_map::MapElement rock_layer_element = {{{(unsigned char)rock_layer, 0,0,0}}};

    class t_map::MapChunk* c;
    for (int i=0; i < MAP_CHUNK_XDIM*MAP_CHUNK_YDIM; i++)
    {
        //int _i = i % 32;
        //int _j = i / 32;

        c = t_map::main_map->chunk[i];
        if (c == NULL) continue;

        int _x = c->xpos;
        int _y = c->ypos;

        for (int x=0; x<16; x++)
        for (int y=0; y<16; y++)
        {


            //int depth = regolith_depth_array[ 512*(_j+y) + (_i+x)];

            int run = 0;
            for (int z=TERRAIN_MAP_HEIGHT-1; z>0; z--)
            {
                int regolith_depth = regolith_depth_array[512*(_y+y) + (_x+x)];

                struct t_map::MapElement e1;
                //struct MapElement e2;

                e1 = c->get_element(x, y, z);

                if (e1.block != 0)
                {
                    run++;
                }
                else
                {
                    run = 0;
                }


                if (run > regolith_depth && e1.block == regolith)
                //if (run > depth && e1.block == regolith)
                {
                    c->set_element(x,y,z, rock_layer_element);
                }
            }
        }
    }
    //main_map->chunk[]
    delete[] regolith_depth_array;
    delete[] _2d_noise_array;
}

}   // t_gen
