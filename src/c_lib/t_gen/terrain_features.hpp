#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/_interface.hpp>
#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>

#include <physics/vec3.hpp>



const float DOUBLE_PI = PI * 2;
const float HALF_PI = PI / 2;
const float QUARTER_PI = HALF_PI / 2;
const float EIGHTH_PI = QUARTER_PI / 2;

const int NUM_LEAVES = 3; // pics == textures.   abbreviating "textures" always looks wrong to me.... suggesting its "text"
int leaves[NUM_LEAVES];
const int NUM_TRUNKS = 2;
int trunks[NUM_TRUNKS];

// we probably dont want to use the same base seed as the map generator, or we'll get the same heightmap. maybe
// this should really be abstacted to a function get_next_seed() but whatever
const int seed = Options::seed * 2;
const float persistence = 0.5f; // tweak
const int octaves = 6;  // tweak

const float tree_perlin_threshold = 0.5f;  // tweak this. move it to a config file maybe
const float tree_threshold = 0.99f;  // this is inside a tree zone



void make_circle(int x, int y, int z, float dist, int block) {
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI) {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        t_map::set(x + (int)fx, y + (int)fy, z, block);
        angle += EIGHTH_PI / 8;
    }
}



bool blocks_are_invalid(int arr[], int len) {
	for (int i = 0; i < len; i++) { 
		GS_ASSERT(arr[i] >= 0);

		if (arr[i] < 0) { 
			printf("*** block id LESS THAN ZERO ***"); 
			return true; 
		}
	}

	return false;
}



void make_tree(int x, int y, int z) {
    // set bark/leaf textures
    int leaf = leaves[randrange(0, NUM_LEAVES - 1)];
    int trunk = trunks[randrange(0, NUM_TRUNKS - 1)];

    int segs = randrange(2, 9);
    for (int seg = 0; seg < segs; seg++) {
        // height of current trunk segment
        int height = randrange(6, 12);
        if (seg == 0) 
            height = randrange(12, 21); // ensure trunk goes up aways
        if (height+z+2 >= ZMAX) break;

        for (int j = 0; j < height; j++) {
            t_map::set(x, y, z + j, trunk);

            if (j == height - 1) {
                float dist = randrange(2, 11);
                for (int i = -dist; i < dist; i++)  t_map::set(x+i, y, z+j, trunk); // limbs
                for (int i = -dist; i < dist; i++)  t_map::set(x, y+i, z+j, trunk); // limbs
                while (dist > 0) {
                    make_circle(x, y, z+j, dist, leaf);
                    make_circle(x, y, z+j+1, dist+1, leaf);
                    make_circle(x, y, z+j+2, dist+.5, leaf);
                    dist -= 2.5;
                }
            }
        }

        z += height;
    }
}






namespace t_gen {
	void add_terrain_features() {
        printf("Adding terrain features\n");

		// setup blocks
		leaves[0] = t_map::get_cube_id("leaves");
		leaves[1] = t_map::get_cube_id("leaves2"); 
		leaves[2] = t_map::get_cube_id("leaves_red"); 
		
		trunks[0] = t_map::get_cube_id("space_tree_trunk"); 
		trunks[1] = t_map::get_cube_id("space_tree_trunk2"); 

		if (blocks_are_invalid(leaves, NUM_LEAVES) ) return;
		if (blocks_are_invalid(trunks, NUM_TRUNKS) ) return;

		// setup perlin array
		float* noise = t_gen::create_2d_noise_array(seed, persistence, octaves, XMAX, YMAX);  // must free return value
		GS_ASSERT(noise != NULL);
		if (noise == NULL) return;






		for (int x=0; x<XMAX; x++)
		for (int y=0; y<YMAX; y++)
			if (noise[x + y*XMAX] > tree_perlin_threshold
			 && genrand_real1() > tree_threshold) // genrand_real1 uses the mersenne twister instead of whatever randf() uses
			{   // we're in tree land
				int z = t_map::get_highest_solid_block(x,y);
				if (z >= 1 && t_map::get(x,y,z) == t_map::get_cube_id("regolith") )
					make_tree(x,y,z);
			}







		// make trees
		//for (int trees = 0; trees < 200; trees++) {
  //          int x, y, z;
  //          
  //          // find appropriate ground for trunk
  //          do {
  //              x = randrange(0, XMAX);
  //              y = randrange(0, YMAX);
  //              z = ZMAX;

  //              while (t_map::get(x, y, z) == 0) z--;
  //          } while (t_map::get(x, y, z) != t_map::get_cube_id("regolith") );

//make_tree(x,y,z);
  //      }






	    free(noise);
    }
}   // t_gen
