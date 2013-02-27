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

const size_t NUM_LEAVES = 3;
const size_t NUM_TRUNKS = 3;
const size_t NUM_SHROOMCAPS = 3;
const size_t NUM_SHROOMSTEMS = 2;

// perlin noise
const float persistence = 0.5f; // tweak
const size_t octaves = 6;  // tweak
const float tree_zone_threshold = 0.3f;  // move to a config file maybe
const float tree_threshold = 0.997f;
const float shroom_zone_threshold = 0.3f;  // move to a config file maybe
const float shroom_threshold = 0.997f;

const int NUM_LOOKUP_ANGLES = 32;
float* sin_lookup_table = NULL;
float* cos_lookup_table = NULL;

CubeType* leaves = NULL;
CubeType* trunks = NULL;
CubeType* shroom_caps = NULL;
CubeType* shroom_stems = NULL;

typedef enum
{
    TREE_RANDOM,    // Dr. Suess style!
    TREE_INVERSIVE, // opposite of earth leaf layers being broadest at base
    TREE_MAX        // for iteration
} TreeType;

void set_me_maybe(int x, int y, int z, CubeType ct, CubeType an_overwriteable = NULL_CUBE)
{
    if (t_map::get(x, y, z) == EMPTY_CUBE ||
        (an_overwriteable != NULL_CUBE &&
         t_map::get(x, y, z) == an_overwriteable))
    {
        t_map::set_fast(x, y, z, ct);
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
                t_map::set_fast(fi_x, fi_y, z_extender, new_block);
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
            t_map::set_fast(x, y, z + j, trunk);

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

void add_trees(float* noise)
{
    printf("\ttrees\n");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;
    for (int x=0; x < t_map::map_dim.x; x++)
    for (int y=0; y < t_map::map_dim.y; y++)
    {
        if (noise[x + y * t_map::map_dim.x] > tree_zone_threshold &&
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

void add_shrooms(float* noise)
{
    printf("\tshrooms\n");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;

    for (int x=0; x < t_map::map_dim.x; x++)
    for (int y=0; y < t_map::map_dim.y; y++)
    {
        if (noise[x + y * t_map::map_dim.x] > shroom_zone_threshold &&
            genrand_real1() > shroom_threshold)
        {
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith &&
                strip_of_solid_blocks_underneath(x,y,z, 6))
                    make_shroom(x,y,z);
        }
    }
}

//int highest_z = 0;
void carve_ray(float x_, float y_, float z_, int tiny_angle, int distance)
{
    //if (highest_z < z_) 
    //{
    //    highest_z = z_;
    //    printf("highest_z is now: %d  ", highest_z);
    //}

    GS_ASSERT(z_ >= 0);
    GS_ASSERT(tiny_angle >= 0 && tiny_angle < NUM_LOOKUP_ANGLES);

    if(z_ < 1 || z_ > 127)
    {
        printf("ERROR: t_gen carve_ray, z= %d \n", z_);
        return;
    }

    for (int i = 0; i < distance; i++)
    {
        int x = x_ + (int)(i * sin_lookup_table[tiny_angle]);
        int y = y_ + (int)(i * cos_lookup_table[tiny_angle]);
        int z = z_;

        t_map::set_fast(x,y,z, EMPTY_CUBE);
    }
}

void carve_angled_gorge_slice(float x_, float y_, float z_, int tiny_angle)
{
    int countdown_til_widening = 1;
    int max_ups_per_width = 1; // iterations upwards
    int ray_length = 0;

    for (int z = z_; z < t_map::map_dim.z; z++)
    {
        int ta = (tiny_angle + 8) % NUM_LOOKUP_ANGLES;
        carve_ray(x_, y_, z, ta, ray_length);

        ta = (tiny_angle + 24) % NUM_LOOKUP_ANGLES;;
        carve_ray(x_, y_, z, ta, ray_length);

        countdown_til_widening--;
        if (countdown_til_widening == 0)
        {
            countdown_til_widening = max_ups_per_width;
            max_ups_per_width *= 2; //+= 2; canyons // ++ made it more like valleys
            ray_length++;
        }
    }
}

int old_quant_angle = 0;
void add_gorges(float* noise, int num_gorges, int length_)
{
    using t_map::map_dim;
    printf("\tgorges\n");

    // make a 2D map of the topsoil heights
    int* peaks = (int*)malloc(map_dim.x * map_dim.y * sizeof(int)); 
    for (int i=0; i < map_dim.x; i++) 
    for (int j=0; j < map_dim.y; j++) 
    { 
        int k = map_dim.x * j + i; 
        peaks[k] = t_map::get_highest_solid_block(i, j); 
    }
    
    // carve all gorges
    for (int i = 0; i < num_gorges; i++)
    {
        int ox = randrange(0, map_dim.x - 1);
        int oy = randrange(0, map_dim.y - 1);

        int steer_angle_idx = randrange(0, map_dim.x - 1);
        int height_idx = randrange(0, map_dim.x - 1);

        float fx = 0;
        float fy = 0;
        float rand_angle = DOUBLE_PI / NUM_LOOKUP_ANGLES * randrange(0, NUM_LOOKUP_ANGLES - 1);
        float curr_angle;
        float curr_perlin_hei; // height

        // find the limits of spectrum for this 1D (out of a 2D map) strip
        float farthest_from_zero = 0;
        for (int i = 0; i < map_dim.x; i++)
        {
            float curr = abs(noise[height_idx + i * map_dim.x]);
            //printf("noise[height_idx + i * map_dim.x]: %.1f\n", noise[height_idx + i * map_dim.x]);
            //printf("curr: %.1f\n", curr);
            if (farthest_from_zero < curr)
                farthest_from_zero = curr;
        }

        float tot_hei_of_poss = farthest_from_zero * 2; // total height of possibility space for random perlin strip

        float tri_shaped_hei = 1; // for gradualizing endpoints easily.  maximum value in the middle, tapering to 1 at the endpoints
        int len_remain = length_ * 2; // length remaining before we're done with this gorge (* 2 cuz we travel in half cube increments
        while (len_remain > 0)
        {
            // get full res angle
            curr_angle = rand_angle + noise[steer_angle_idx + len_remain * map_dim.x];
            // keep angle in a range that maps to an array
			while (curr_angle < 0)          curr_angle += DOUBLE_PI;
            while (curr_angle >= DOUBLE_PI) curr_angle -= DOUBLE_PI;

            // get a valid possibility space
            int x = ox + (int)fx;   x %= map_dim.x;    while (x < 0) x += map_dim.x;
            int y = oy + (int)fy;   y %= map_dim.y;    while (y < 0) y += map_dim.y;
            float highest = peaks[map_dim.x * y + x];
            float lowest = highest - tri_shaped_hei;
            //if (i == 0)     printf("_____________________________________________________\n", highest);
            //if (i == 0)     printf("tri_shaped_hei: %.1f\n", tri_shaped_hei);
            if (lowest < 6) // keep from getting too close to bedrock
                lowest = 6;
            float edge_to_center_dist = (highest - lowest) / 2; // from center of possibility space 
            //// use perlin to map to that space
            curr_perlin_hei = noise[height_idx + len_remain * map_dim.x];
            if (curr_perlin_hei < 0)
                curr_perlin_hei /= -farthest_from_zero; // turn it into a 0.0 - 1.0 range
            else
                curr_perlin_hei /= farthest_from_zero; // turn it into a 0.0 - 1.0 range
            curr_perlin_hei *= edge_to_center_dist; // apply it to possibility space
            float fz = lowest + edge_to_center_dist + curr_perlin_hei;
            
            // get tiny/quantized angle for lookup table index
            int quant_angle = 0;
            while (curr_angle > 0.0f)
            {
                curr_angle -= (DOUBLE_PI / NUM_LOOKUP_ANGLES);
                quant_angle++;
                quant_angle %= NUM_LOOKUP_ANGLES;
            }

			// will carve a slice for every change from previous quantized angle
			if (quant_angle != old_quant_angle)
			{
				int n = quant_angle; // new/current angle
				int o = old_quant_angle;
				
				while (n > o)
				{
		            carve_angled_gorge_slice(ox + fx, oy + fy, fz, n);
					n--;
				}
				while (o > n)
				{
		            carve_angled_gorge_slice(ox + fx, oy + fy, fz, n);
					n++;
				}
			}

            carve_angled_gorge_slice(ox + fx, oy + fy, fz, quant_angle);
			
			// move half unit forward
            fx += sin_lookup_table[quant_angle] / 2;
            fy += cos_lookup_table[quant_angle] / 2;

            carve_angled_gorge_slice(ox + fx, oy + fy, fz, quant_angle);
			
            if (len_remain < length_) // half to match the * 2 of len_remain
                tri_shaped_hei -= 0.45;
            else
                tri_shaped_hei += 0.45;

            //if (i == 0) printf("tri_shaped_hei: %f\n", tri_shaped_hei);

            len_remain--;
            old_quant_angle = quant_angle;
        }

        //old_quant_angle = ;  FIXME?  shouldn't start with an angle that was last used in previous gorge?
    }

    free(peaks);
}

void add_terrain_features()
{
    using t_map::map_dim;
    printf("Adding terrain features\n");

    GS_ASSERT(sin_lookup_table == NULL);
    GS_ASSERT(cos_lookup_table == NULL);
    sin_lookup_table = new float[NUM_LOOKUP_ANGLES];
    for (int i = 0; i < NUM_LOOKUP_ANGLES; i++)
        sin_lookup_table[i] = sinf(DOUBLE_PI / NUM_LOOKUP_ANGLES * i);
    cos_lookup_table = new float[NUM_LOOKUP_ANGLES];
    for (int i = 0; i < NUM_LOOKUP_ANGLES; i++)
        cos_lookup_table[i] = cosf(DOUBLE_PI / NUM_LOOKUP_ANGLES * i);

    // setup cube sets
    leaves = new CubeType[NUM_LEAVES];
    trunks = new CubeType[NUM_TRUNKS];
    shroom_caps = new CubeType[NUM_SHROOMCAPS];
    shroom_stems = new CubeType[NUM_SHROOMSTEMS];

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
    float* noise = t_gen::create_2d_noise_array(persistence, octaves, map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;

    // add the features
    add_gorges(noise, 45, 170);
    add_trees(noise);
    add_shrooms(noise);

    free(noise);

    delete[] sin_lookup_table;
    delete[] cos_lookup_table;
    delete[] leaves;
    delete[] trunks;
    delete[] shroom_caps;
    delete[] shroom_stems;
}

}   // t_gen
