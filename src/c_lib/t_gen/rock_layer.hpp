#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{

void create_tree(int x, int y, int z)
{
    
}

void XXtree_generatorXX()
{
    static int regolith = t_map::get_cube_id("regolith");
    GS_ASSERT(regolith >= 0);
    if (regolith < 0) return;   // invalid cube id

    const int x = XMAX;
    const int y = YMAX;

    // PARAMETERS
    
    // we probably dont want to use the same base seed as the map generator, or we'll get the same heightmap. maybe
    // this should really be abstacted to a function get_next_seed() but whatever
    const int seed = Options::seed * 2;
    const float persistence = 0.5f; // tweak
    const int octaves = 6;  // tweak

    const float tree_perlin_threshold = 0.5f;  // tweak this. move it to a config file maybe
    const float tree_threshold = 0.95f;  // this is inside a tree zone
    
    float* noise = t_gen::create_2d_noise_array(seed, persistence, octaves, x, y);  // must free return value
    GS_ASSERT(noise != NULL);
    if (noise == NULL) return;

    for (int i=0; i<x; i++)
    for (int j=0; j<y; j++)
        if (noise[x + i*y] > tree_perlin_threshold
         && genrand_real1() > tree_threshold) // genrand_real1 uses the mersenne twister instead of whatever randf() uses
        {   // we're in tree land
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith)
                create_tree(x,y,z); // you replace this with yours
        }

    free(noise);
}

void generate_rock_layer()
{
    float* _2d_noise_array = new float [512*512];
    int* regolith_depth_array = new int[512*512]; 

    const int seed = 5812;
    const float persistance = 0.99f;
    const int octaves = 8;

    t_gen::populate_2d_noise_array(_2d_noise_array, seed, persistance, octaves);

    for(int i=0; i<512; i++)
    for(int j=0; j<512; j++)
    {
        regolith_depth_array[512*j+i] = 9.0f + 9.0f*(0.5+_2d_noise_array[512*j+i]);
    }

    int regolith = t_map::dat_get_cube_id("regolith");
    //int rock_layer = t_map::dat_get_cube_id("rock");
    //int rock_layer = t_map::dat_get_cube_id("space_tree_trunk");
    int rock_layer = t_map::dat_get_cube_id("rock");

    struct t_map::MAP_ELEMENT rock_layer_element = {{{(unsigned char)rock_layer, 0,0,0}}};

    class t_map::MAP_CHUNK* c;
    for(int i=0; i < t_map::MAP_CHUNK_XDIM*t_map::MAP_CHUNK_YDIM; i++)
    {
        //int _i = i % 32;
        //int _j = i / 32;

        c = t_map::main_map->chunk[i];
        if(c == NULL) continue;
        
        int _x = c->xpos;
        int _y = c->ypos;

        for(int x=0; x<16; x++)
        for(int y=0; y<16; y++)
        {


            //int depth = regolith_depth_array[ 512*(_j+y) + (_i+x)];

            int run = 0;
            for(int z=t_map::TERRAIN_MAP_HEIGHT-1; z>0; z--)
            {
                int regolith_depth = regolith_depth_array[512*(_y+y) + (_x+x)];

                struct t_map::MAP_ELEMENT e1;
                //struct MAP_ELEMENT e2;

                e1 = c->get_element(x,y,z);

                if(e1.block != 0)
                {
                    run++;
                }
                else
                {
                    run = 0;
                }


                if(run > regolith_depth && e1.block == regolith)
                //if(run > depth && e1.block == regolith)
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
