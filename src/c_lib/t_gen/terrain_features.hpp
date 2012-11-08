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
CubeID leaves[NUM_LEAVES];
const int NUM_TRUNKS = 2;
CubeID trunks[NUM_TRUNKS];
const size_t NUM_SHROOMS = 2;
CubeID shrooms[NUM_SHROOMS] = {NULL_CUBE};

// we probably dont want to use the same base seed as the map generator, or we'll get the same heightmap. maybe
// this should really be abstacted to a function get_next_seed() but whatever
const int seed = Options::seed * 2;
const float persistence = 0.5f; // tweak
const int octaves = 6;  // tweak

const float tree_zone_threshold = 0.3f;  // move to a config file maybe
const float tree_threshold = 0.997f;
const float shroom_zone_threshold = 0.5f;  // move to a config file maybe
const float shroom_threshold = 0.997f;



void set_me_maybe(int x, int y, int z, CubeID block, CubeID an_overwriteable = NULL_CUBE) {
    if (t_map::get(x, y, z) == EMPTY_CUBE
    || (an_overwriteable != NULL_CUBE && t_map::get(x, y, z) == an_overwriteable))
        t_map::set(x, y, z, block);
}

void make_circle(int x, int y, int z, float dist, CubeID block, CubeID an_overwriteable = NULL_CUBE) { // instead of from the center of given block
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI) {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        set_me_maybe(x + (int)fx, y + (int)fy, z, block, an_overwriteable);
        angle += PI / 32;
    }
}



bool corner_origin_heart(int x, int y, int z, float dist, CubeID new_block = NULL_CUBE, bool extend_down = false) {
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI) {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        int fi_x = x + (int)(fx+.5); // final x
        int fi_y = y + (int)(fy+.5);
        angle += PI / 64;

        if (new_block == NULL_CUBE) { // we're just asking this fuction a question
            if (t_map::get(fi_x, fi_y, z) != EMPTY_CUBE)
                return false;
        } else
            set_me_maybe(fi_x, fi_y, z, new_block);

        if (extend_down) {
            // make sure it extends all the way to the ground
            int z_extender = z - 1;
            while (t_map::get(fi_x, fi_y, z_extender) == EMPTY_CUBE) {
                t_map::set(fi_x, fi_y, z_extender, new_block);
                z_extender--;
            }
        }
    }

    return true;
}



bool corner_origin_circle_untouched(int x, int y, int z, float dist) {
    if (corner_origin_heart(x, y, z, dist) ) return false;
    return true;
}



void corner_origin_make_circle(int x, int y, int z, float dist, CubeID block, bool extend_down = false) { // instead of from the center of given block
    corner_origin_heart(x, y, z, dist, block, extend_down);  
}



bool blocks_are_invalid(CubeID arr[], int len) {
    for (int i = 0; i < len; i++) { 
        GS_ASSERT(t_map::isValidCube(arr[i]));
        
        if (!t_map::isValidCube(arr[i])) {
            printf("*** invalid cube %d ***", arr[i]); 
            return true; 
        }
    }

    return false;
}



void make_shroom(int x, int y, int z) {
    CubeID shroom = shrooms[randrange(0, NUM_SHROOMS - 1)];

    int cap_height = randrange(4, 15);
    int hei = 0;
    float t_rad = randrange(1, 4); // trunk/stem radius
    float cap_rad = t_rad * 4;
    int cap_rad_changer = 0;

    while (cap_rad > 0) {
        if (cap_rad > t_rad) 
            corner_origin_make_circle(x, y, z+hei, t_rad, shroom, hei == 0);

        if (hei >= cap_height) {
            if (corner_origin_circle_untouched(x, y, z+hei, cap_rad) ) 
                corner_origin_make_circle(     x, y, z+hei, cap_rad, shroom);
            
            //
            int targ_cap_rad = cap_rad + cap_rad_changer;
            while (targ_cap_rad > cap_rad) { cap_rad++; corner_origin_make_circle(x, y, z+hei, cap_rad, shroom); }
            while (targ_cap_rad < cap_rad) { cap_rad--; corner_origin_make_circle(x, y, z+hei, cap_rad, shroom); }

            cap_rad_changer--;
        }

        hei++;
    }
}



void make_tree(int x, int y, int z) {
    // set bark/leaf textures
    CubeID leaf = leaves[randrange(0, NUM_LEAVES - 1)];
    CubeID trunk = trunks[randrange(0, NUM_TRUNKS - 1)];

    int segs = randrange(2, 9);
    for (int seg = 0; seg < segs; seg++) {
        // height of current trunk segment
        int height = randrange(5, 11);
        if (seg == 0) 
            height = randrange(12, 21); // ensure trunk goes up aways
        if (height+z+2 >= ZMAX) break;

        float dist = .0f; // from center voxel
        for (int j = 0; j < height; j++) {
            t_map::set(x, y, z + j, trunk);

            if (j == height - 1) {
                if (randrange(0,1) == 00) 
                    dist = randrange(2, 11);
                else dist += .5f;
                for (int i = -dist; i < dist; i++)  set_me_maybe(x+i, y, z+j, trunk); // limbs
                for (int i = -dist; i < dist; i++)  set_me_maybe(x, y+i, z+j, trunk); // limbs
                while (dist > 0) {
                    make_circle(x, y, z+j,   dist, leaf, trunk);
                    make_circle(x, y, z+j+1, dist+1, leaf, trunk);
                    make_circle(x, y, z+j+2, dist+.5, leaf, trunk);
                    dist -= 2.5;
                }
            }
        }

        z += height;
    }
}



bool strip_of_solid_blocks_underneath(int x, int y, int z, int num) {
    for (int i = 1; i <= num; i++) 
        if /* negspace or air */ (z-i < 0 || t_map::get(x, y, z-i) == EMPTY_CUBE) return false;
    return true;
}





namespace t_gen {
    void add_terrain_features() {
        printf("Adding terrain features\n");
        printf("    trees\n");
        printf("    shrooms\n");

        // setup blocks
        leaves[0] = t_map::get_cube_id("leaves1");
        leaves[1] = t_map::get_cube_id("leaves2"); 
        leaves[2] = t_map::get_cube_id("leaves3"); 
        
        trunks[0] = t_map::get_cube_id("space_tree_trunk1"); 
        trunks[1] = t_map::get_cube_id("space_tree_trunk2");

        shrooms[0] = t_map::get_cube_id("mushroom1");
        shrooms[1] = t_map::get_cube_id("mushroom2");

        CubeID regolith = t_map::get_cube_id("regolith");
        GS_ASSERT(t_map::isValidCube(regolith));
        if (!t_map::isValidCube(regolith)) return;

        if (blocks_are_invalid(leaves, NUM_LEAVES) ) return;
        if (blocks_are_invalid(trunks, NUM_TRUNKS) ) return;
        if (blocks_are_invalid(shrooms, NUM_SHROOMS) ) return;

        // setup perlin array
        float* noise = t_gen::create_2d_noise_array(persistence, octaves, XMAX, YMAX);  // must free return value
        GS_ASSERT(noise != NULL);
        if (noise == NULL) return;

        // make groves
        for (int x=0; x<XMAX; x++)
        for (int y=0; y<YMAX; y++) {
            if (noise[x + y*XMAX] > tree_zone_threshold
             && genrand_real1() > tree_threshold) // genrand_real1 uses the mersenne twister instead of whatever randf() uses
            {   // we're in tree land
                int z = t_map::get_highest_solid_block(x,y);

                if (z >= 1 && t_map::get(x,y,z) == regolith)
                    if (strip_of_solid_blocks_underneath(x,y,z, 6) ) {
                        make_tree(x,y,z);
                        continue;
                    }
            }

            if (noise[x + y*XMAX] > shroom_zone_threshold
             && genrand_real1() > shroom_threshold) // genrand_real1 uses the mersenne twister instead of whatever randf() uses
            {   // we're in shroom land
                int z = t_map::get_highest_solid_block(x,y);

                if (z >= 1 && t_map::get(x,y,z) == regolith)
                    if (strip_of_solid_blocks_underneath(x,y,z, 6) )
                        make_shroom(x,y,z);
            }
        }

        free(noise);
    }
}   // t_gen
