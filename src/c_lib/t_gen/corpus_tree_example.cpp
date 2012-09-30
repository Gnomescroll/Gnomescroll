#include <t_gen/_interface.hpp>

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
