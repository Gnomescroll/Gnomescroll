#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_gen/_interface.hpp>
#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>

#include <physics/vec3.hpp>
#include <physics/mat3.hpp>

namespace t_gen 
{

const float DOUBLE_PI = PI * 2;
const float HALF_PI = PI / 2;
const float QUARTER_PI = HALF_PI / 2;
const float EIGHTH_PI = QUARTER_PI / 2;

const size_t NUM_LEAVES = 3;     
const size_t NUM_TRUNKS = 3;     
const size_t NUM_SHROOMCAPS = 3; 
const size_t NUM_SHROOMSTEMS = 2;

//const int _MAX_NUM = 8;
/*
CubeType leaves[NUM_LEAVES] = {NULL_CUBE};
CubeType trunks[NUM_TRUNKS] = {NULL_CUBE};
CubeType shroom_caps [NUM_SHROOMCAPS]  = {NULL_CUBE};
CubeType shroom_stems[NUM_SHROOMSTEMS] = {NULL_CUBE};
*/

/*
CubeType leaves[_MAX_NUM] = {NULL_CUBE};
CubeType trunks[_MAX_NUM] = {NULL_CUBE};
CubeType shroom_caps [_MAX_NUM]  = {NULL_CUBE};
CubeType shroom_stems[_MAX_NUM] = {NULL_CUBE};
*/

CubeType* leaves = NULL;
CubeType* trunks = NULL;
CubeType* shroom_caps = NULL;
CubeType* shroom_stems = NULL;

const float persistence = 0.5f; // tweak
const size_t octaves = 6;  // tweak

const float tree_zone_threshold = 0.3f;  // move to a config file maybe
const float tree_threshold = 0.997f;
const float shroom_zone_threshold = 0.3f;  // move to a config file maybe
const float shroom_threshold = 0.997f;

typedef enum 
{
    TREE_RANDOM,    // Dr. Suess style!
    TREE_INVERSIVE, // opposite of earth leaf layers being broadest at base
    TREE_MAX        // for iteration
} TreeType;

void set_me_maybe(int x, int y, int z, CubeType ct,
                  CubeType an_overwriteable = NULL_CUBE)
{
    if (t_map::get(x, y, z) == EMPTY_CUBE || 
        (an_overwriteable != NULL_CUBE && 
         t_map::get(x, y, z) == an_overwriteable))
    {
        t_map::set(x, y, z, ct);
    }
}

void make_circle(int x, int y, int z, float dist, CubeType ct,
                 CubeType an_overwriteable = NULL_CUBE) 
{ // instead of from the center of given block
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI)
    {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        set_me_maybe(x + (int)fx, y + (int)fy, z, ct, an_overwriteable);
        angle += PI / 32;
    }
}

bool corner_origin_heart(int x, int y, int z, float dist,
                         CubeType new_block = NULL_CUBE,
                         bool extend_down = false)
{
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI)
    {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        int fi_x = x + (int)(fx+.5); // final x
        int fi_y = y + (int)(fy+.5);
        angle += PI / 64;

        if (new_block == NULL_CUBE)
        {   // we're just asking this fuction a question
            if (t_map::get(fi_x, fi_y, z) != EMPTY_CUBE)
                return false;
        }
        else
            set_me_maybe(fi_x, fi_y, z, new_block);

        if (extend_down)
        {
            // make sure it extends all the way to the ground
            int z_extender = z - 1;
            while (t_map::get(fi_x, fi_y, z_extender) == EMPTY_CUBE &&
                   z_extender >= 0)
            {
                t_map::set(fi_x, fi_y, z_extender, new_block);
                z_extender--;
            }
        }
    }

    return true;
}

bool corner_origin_circle_untouched(int x, int y, int z, float dist)
{
    return (!corner_origin_heart(x, y, z, dist));
}

void corner_origin_make_circle(int x, int y, int z, float dist,
                               CubeType block, bool extend_down = false)
{ // instead of from the center of given block
    corner_origin_heart(x, y, z, dist, block, extend_down);  
}

bool blocks_are_invalid(CubeType arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        GS_ASSERT(t_map::isValidCube(arr[i]));
        if (!t_map::isValidCube(arr[i]))
        {
            printf("*** invalid cube %d ***", arr[i]); 
            return true; 
        }
    }
    return false;
}

void make_shroom(int x, int y, int z)
{
    CubeType shroom_cap = shroom_caps[randrange(0, NUM_SHROOMCAPS - 1)];
    //CubeType shroom_stem = shroom_stems[randrange(0, NUM_SHROOMSTEMS - 1)];
    // (need to make some stem textures)
    int cap_height = randrange(4, 15);
    int hei = 0;
    float stem_rad = randrange(16/*10*/, 40) / 10.0f; // stem radius
    float cap_rad = stem_rad * 4;
    int cap_rad_changer = 0;

    while (cap_rad > 0)
    {
        if (cap_rad > stem_rad)
            corner_origin_make_circle(x, y, z+hei, stem_rad, shroom_cap,
                                      (hei == 0));
        if (hei >= cap_height)
        {
            if (corner_origin_circle_untouched(x, y, z+hei, cap_rad))
                corner_origin_make_circle(x, y, z+hei, cap_rad, shroom_cap);
            int targ_cap_rad = cap_rad + cap_rad_changer;
            while (targ_cap_rad > cap_rad)
            {
                cap_rad++;
                corner_origin_make_circle(x, y, z+hei, cap_rad, shroom_cap);
            }
            while (targ_cap_rad < cap_rad)
            {
                cap_rad--;
                corner_origin_make_circle(x, y, z+hei, cap_rad, shroom_cap);
            }
            cap_rad_changer--;
        }

        hei++;
    }
}

void make_tree(int x, int y, int z)
{   // set bark/leaf textures
    CubeType leaf = leaves[randrange(0, NUM_LEAVES - 1)];
    CubeType trunk = trunks[randrange(0, NUM_TRUNKS - 1)];

    int segs = randrange(2, 6);
    float rad = 2.0f; // radius of leaf layer from center voxel
    TreeType tt = (TreeType)randrange(0, (int)TREE_MAX-1);
    
    // make tree, segment by segment
    for (int seg = 0; seg < segs; seg++)
    {   // height of current trunk segment
        int height = randrange(6, 11);
        if (seg == 0) height = randrange(12, 21); // ensure trunk goes up aways
        if (height+z+2 >= t_map::map_dim.z) break;

        // make segment
        for (int j = 0; j < height; j++)
        {
            t_map::set(x, y, z + j, trunk);

            if (j == height - 1)
            {
                switch (tt)
                {
                    case TREE_RANDOM:
                        rad = randrange(2, 11);
                        break;
                    case TREE_INVERSIVE:
                        rad += 1.7f;
                        break;
                    case TREE_MAX:
                        GS_ASSERT(false);
                        break;
                }

                // limbs
                for (int i = -rad; i < rad; i++)
                    set_me_maybe(x+i, y, z+j, trunk); 
                for (int i = -rad; i < rad; i++)
                    set_me_maybe(x, y+i, z+j, trunk);

                float ring_rad = rad; // current ring radius
                while (ring_rad > 0)
                {
                    make_circle(x, y, z+j,   ring_rad,    leaf, trunk);
                    make_circle(x, y, z+j+1, ring_rad+1,  leaf, trunk);
                    make_circle(x, y, z+j+2, ring_rad+0.5f, leaf, trunk);
                    ring_rad -= 2.5;
                }
            }
        }

        z += height;
    }
}

bool strip_of_solid_blocks_underneath(int x, int y, int z, int num)
{
    for (int i = 1; i <= num; i++) 
        if (z-i < 0 || t_map::get(x, y, z-i) == EMPTY_CUBE)
            return false;
    return true;
}

void carve_aligned_gorge_slice(int x_, int y_, int z_)
{
    int port      = x_;
    int starboard = x_;
    int countdown_til_widening = 1;
    int max_ups_per_width = 1; // iterations upwards

    for (int z = z_; z <= t_map::map_dim.z-1; z++) 
    {
        for (int x = port; x < starboard + 1; x++) 
        {
            //fx = * starboard;
            //fy = * starboard;
            t_map::set(x_ + x, y_, z, EMPTY_CUBE);
            //  t_map::set((int)tx.x, (int)tx.y, (int)tx.z, EMPTY_CUBE);
            //  curr_angle += 0.01f;
        }

        countdown_til_widening--;
        if (countdown_til_widening == 0)
        {
            countdown_til_widening = max_ups_per_width;
            max_ups_per_width += 2; // ++ made it more like canyons
            port--;
            starboard++;
        }
    }
}

void add_gorges(float* noise, int xnoise, int ynoise, int n_gorges)
{
    printf("\tgorges\n");
    for (int i = 0; i < n_gorges; i++)
    {
        int x_ = randrange(0, t_map::map_dim.x - 1);
        int y_ = randrange(0, t_map::map_dim.y - 1);
        //CubeType ct = shroom_caps[randrange(0, NUM_SHROOMCAPS - 1)];

        int length = 305; // randrange(30, 160);
        int rand_idx = randrange(0, xnoise - 1);

        float fx = 0;
        float fy = 0;
        float angle = 0;

        while (length > 0) 
        {
            fx += sinf(angle);
            fy += cosf(angle);

            carve_aligned_gorge_slice(x_ + (int)fx, y_ + (int)fy, 6);
            angle = noise[rand_idx + length * xnoise];  //PI / 32;
            length--;
        }
    }
}

void add_trees(float* noise, int xnoise, int ynoise)
{
    printf("\ttrees\n");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;
    for (int x=0; x<xnoise; x++)
    for (int y=0; y<ynoise; y++)
    {
        if (noise[x + y*xnoise] > tree_zone_threshold &&
            genrand_real1() > tree_threshold)
        {
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith &&
                strip_of_solid_blocks_underneath(x,y,z, 6))
                {
                    make_tree(x,y,z);
                    continue;
                }
        }
    }
}

void add_shrooms(float* noise, int xnoise, int ynoise)
{
    printf("\tshrooms\n");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;
    for (int x=0; x<xnoise; x++)
    for (int y=0; y<ynoise; y++)
    {
        if (noise[x + y*xnoise] > shroom_zone_threshold &&
            genrand_real1() > shroom_threshold)
        {
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith &&
                strip_of_solid_blocks_underneath(x,y,z, 6))
                    make_shroom(x,y,z);
        }
    }
}

void add_terrain_features() 
{
    printf("Adding terrain features\n");

    using t_map::map_dim;
    /*
    CubeType leaves[NUM_LEAVES] = {NULL_CUBE};
    CubeType trunks[NUM_TRUNKS] = {NULL_CUBE};
    CubeType shroom_caps [NUM_SHROOMCAPS]  = {NULL_CUBE};
    CubeType shroom_stems[NUM_SHROOMSTEMS] = {NULL_CUBE};
    */
    leaves = new CubeType[NUM_LEAVES];
    trunks = new CubeType[NUM_TRUNKS];
    shroom_caps = new CubeType[NUM_SHROOMCAPS];
    shroom_stems = new CubeType[NUM_SHROOMSTEMS];

    // setup cube sets
    leaves[0] = t_map::get_cube_type("leaves1");
    leaves[1] = t_map::get_cube_type("leaves2"); 
    leaves[2] = t_map::get_cube_type("leaves3"); 
    
    trunks[0] = t_map::get_cube_type("space_tree_trunk1"); 
    trunks[1] = t_map::get_cube_type("space_tree_trunk2");
    trunks[2] = t_map::get_cube_type("space_tree_trunk3");

    shroom_caps[0] = t_map::get_cube_type("mushroom_cap1");
    shroom_caps[1] = t_map::get_cube_type("mushroom_cap2");
    shroom_caps[2] = t_map::get_cube_type("mushroom_cap3");

    shroom_stems[0] = t_map::get_cube_type("mushroom_stem1");
    shroom_stems[1] = t_map::get_cube_type("mushroom_stem2");

    if (blocks_are_invalid(leaves, NUM_LEAVES)) return;
    if (blocks_are_invalid(trunks, NUM_TRUNKS)) return;
    if (blocks_are_invalid(shroom_caps, NUM_SHROOMCAPS)) return;
    if (blocks_are_invalid(shroom_stems, NUM_SHROOMSTEMS)) return;

    // setup perlin array
    float* noise = t_gen::create_2d_noise_array(persistence, octaves,
                                                 map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;

    //add_gorges(noise, map_dim.x, map_dim.y, 20);
    add_trees(noise, map_dim.x, map_dim.y);
    add_shrooms(noise, map_dim.x, map_dim.y);

    free(noise);

    delete[] leaves;
    delete[] trunks;
    delete[] shroom_caps;
    delete[] shroom_stems;
}
    
}   // t_gen
