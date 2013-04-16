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

#if PRODUCTION
const int GORGE_COUNT = 10;
const int GORGE_LENGTH = 170;
#else
const int GORGE_COUNT = 45;
const int GORGE_LENGTH = 170;
#endif

const size_t NUM_LEAVES = 3;
const size_t NUM_TRUNKS = 3;
const size_t NUM_SHROOMCAPS = 3;
const size_t NUM_SHROOMSTEMS = 2;

// perlin noise
const float PERSISTENCE = 0.5f; // tweak
const size_t OCTAVES = 6;  // tweak
const float TREE_ZONE_THRESHOLD = 0.3f;  // move to a config file maybe
const float TREE_THRESHOLD = 0.997f;
const float SHROOM_ZONE_THRESHOLD = 0.3f;  // move to a config file maybe
const float SHROOM_THRESHOLD = 0.997f;

const int NUM_LOOKUP_ANGLES = 32;
float* sin_lookup_table = NULL;
float* cos_lookup_table = NULL;

CubeType* leaves = NULL;
CubeType* trunks = NULL;
CubeType* shroom_caps = NULL;
CubeType* shroom_stems = NULL;
CubeType terrain_features_curr = EMPTY_CUBE;
int      terrain_features_goal = 0;

typedef enum
{
    TREE_RANDOM,    // Dr. Suess style!
    TREE_INVERSIVE, // opposite of earthling trees' leaf layers being broadest at base
    TREE_MAX        // for iteration
} TreeType;


bool blocks_are_invalid(CubeType arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        GS_ASSERT(t_map::isValidCube(arr[i]));
        if (!t_map::isValidCube(arr[i]))
            return true;
    }
    return false;
}

void set_if_empty(int x, int y, int z, CubeType cube)
{
    if (t_map::get(x, y, z) == EMPTY_CUBE)
        t_map::set_fast(x, y, z, cube);
}

void set_if_empty_or_type(int x, int y, int z, CubeType cube, CubeType overwrite)
{
    CubeType existing = t_map::get(x, y, z);
    if (existing == EMPTY_CUBE || existing == overwrite)
        t_map::set_fast(x, y, z, cube);
}

void make_circle(int x, int y, int z, float dist, CubeType ct, CubeType overwrite)
{ // instead of from the center of given block
    float fx = 0;
    float fy = 0;
    float angle = 0;

    while (angle < DOUBLE_PI)
    {
        fx = sinf(angle) * dist;
        fy = cosf(angle) * dist;
        set_if_empty_or_type(x+fx, y+fy, z, ct, overwrite);
        angle += PI / 32.0f;
    }
}

bool corner_origin_make_circle(int x, int y, int z, float dist, CubeType new_block, bool extend_down)
{
    if (dist < 0) return false;
    const int precision = 256;
    for (int i=0; i<=precision; i++)
    {
        float angle = 2 * PI * (float(i) / float(precision));
        float fx = sinf(angle) * dist;
        float fy = cosf(angle) * dist;
        int fi_x = translate_point(int(x + 0.5f + fx));
        int fi_y = translate_point(int(y + 0.5f + fy));

        set_if_empty(fi_x, fi_y, z, new_block);

        if (extend_down)
        {   // make sure it extends all the way to the ground
            int z_extender = z - 1;
            while (t_map::get(fi_x, fi_y, z_extender) == EMPTY_CUBE &&
                   z_extender >= 0)
            {
                IF_ASSERT(x < 0 || x >= map_dim.x) return false;
                IF_ASSERT(y < 0 || y >= map_dim.y) return false;
                IF_ASSERT(z < 1 || z >= map_dim.z) return false;  // gonna want bedrock always at least 1 layer high?

                t_map::set_fast(fi_x, fi_y, z_extender, new_block);
                z_extender--;
            }
        }
    }

    return true;
}

void make_shroom(int x, int y, int z)
{
    CubeType shroom_cap = shroom_caps[randrange(0, NUM_SHROOMCAPS - 1)];
    //CubeType shroom_stem = shroom_stems[randrange(0, NUM_SHROOMSTEMS - 1)];
    // (need to make some stem textures)
    int cap_height = randrange(4, 15);
    int h = 0;
    float stem_rad = randrange(16, 40) / 10.0f; // stem radius
    float cap_rad = stem_rad * 4;
    int torus_span = 1;

    corner_origin_make_circle(x, y, z, stem_rad, shroom_cap, true);
    while (cap_rad >= 0)
    {
        if (cap_rad > stem_rad)
            corner_origin_make_circle(x, y, z+h, stem_rad, shroom_cap, false);
        if (h > cap_height)
        {
            for (int i=0; i<torus_span; i++, cap_rad--)
                corner_origin_make_circle(x, y, z+h, cap_rad, shroom_cap, false);
            torus_span++;
        }
        h++;
    }
}

void make_tree(int x, int y, int z)
{   // set bark/leaf textures
    CubeType leaf = leaves[randrange(0, NUM_LEAVES - 1)];
    CubeType trunk = trunks[randrange(0, NUM_TRUNKS - 1)];

    int segs = randrange(2, 6);
    float rad = 2.0f; // radius of leaf layer from center voxel
    TreeType tt = (TreeType)randrange(0, int(TREE_MAX)-1);

    // make tree, segment by segment
    for (int seg = 0; seg < segs; seg++)
    {   // height of current trunk segment
        int height = randrange(6, 11);
        if (seg == 0) height = randrange(12, 21); // ensure trunk goes up aways
        if (height+z+2 >= map_dim.z) break;

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
                    set_if_empty(x+i, y, z+j, trunk);
                for (int i = -rad; i < rad; i++)
                    set_if_empty(x, y+i, z+j, trunk);

                float ring_rad = rad; // current ring radius
                while (ring_rad > 0)
                {
                    make_circle(x, y, z + j,     ring_rad,        leaf, trunk);
                    make_circle(x, y, z + j + 1, ring_rad + 1.0f, leaf, trunk);
                    make_circle(x, y, z + j + 2, ring_rad + 0.5f, leaf, trunk);
                    ring_rad -= 2.5;
                }
            }
        }

        z += height;
    }
}

bool vertical_strip_of_solids_underneath(int num, int x, int y, int z)
{
    for (int i = 1; i <= num; i++)
        if (z-i < 0 || t_map::get(x, y, z-i) == EMPTY_CUBE)
            return false;
    return true;
}

void add_trees()
{
    int t = _GET_MS_TIME();
    printf("\ttrees......");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;
    float* noise = t_gen::create_2d_noise_array(PERSISTENCE, OCTAVES, map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;

    for (int x=0; x < map_dim.x; x++)
    for (int y=0; y < map_dim.y; y++)
    {
        if (noise[x + y * map_dim.x] > TREE_ZONE_THRESHOLD &&
            mrandf() > TREE_THRESHOLD)
        {
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith &&
                vertical_strip_of_solids_underneath(6, x,y,z))
                {
                    make_tree(x,y,z);
                    continue;
                }
        }
    }

    free(noise);
    printf(" %i ms\n", _GET_MS_TIME() - t);
}

void add_shrooms()
{
    int t = _GET_MS_TIME();
    printf("\tshrooms......");
    static CubeType regolith = t_map::get_cube_type("regolith");
    IF_ASSERT(!t_map::isValidCube(regolith)) return;
    float* noise = t_gen::create_2d_noise_array(PERSISTENCE, OCTAVES, map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;

    for (int x=0; x < map_dim.x; x++)
    for (int y=0; y < map_dim.y; y++)
    {
        if (noise[x + y * map_dim.x] > SHROOM_ZONE_THRESHOLD &&
            mrandf() > SHROOM_THRESHOLD)
        {
            int z = t_map::get_highest_solid_block(x,y);
            if (z >= 1 && t_map::get(x,y,z) == regolith &&
                vertical_strip_of_solids_underneath(6, x,y,z))
            {
                make_shroom(x,y,z);
            }
        }
    }

    free(noise);
    printf(" %i ms\n", _GET_MS_TIME() - t);
}

bool carve_ray(float x, float y, float z, int tiny_angle, int distance)
{
    IF_ASSERT(z < 0 || z >= map_dim.z) return false;
    IF_ASSERT(tiny_angle < 0 || tiny_angle >= NUM_LOOKUP_ANGLES) return false;

    int cubes_changed = 0;
    for (float f = 0; f <= distance; f += 0.33f)
    {
        int m = x + int(f * sin_lookup_table[tiny_angle]);
        int n = y + int(f * cos_lookup_table[tiny_angle]);

        if (t_map::get(m, n, z) != terrain_features_curr)
        {
            t_map::set_fast(m, n, z, terrain_features_curr);
            cubes_changed++;
        }
    }

    if (cubes_changed > 0) return true;
    else                   return false;
}

void carve_angled_gorge_slice(float x, float y, float z, int quant_angle) // quantized angle
{
    IF_ASSERT(z < 0 || z >= map_dim.z) return;

    int countdown_til_widening = 1;
    int max_ups_per_width = 1; // iterations upwards
    int ray_length = 0;

    for (int k = z; k < map_dim.z; k++)
    {
        int qa1 = (quant_angle + 8 ) % NUM_LOOKUP_ANGLES;
        int qa2 = (quant_angle + 24) % NUM_LOOKUP_ANGLES;
        //if (rays at both sides already set to desired cube)    FIXME
        carve_ray(x, y, k, qa1, ray_length);
        carve_ray(x, y, k, qa2, ray_length);
            //break;

        countdown_til_widening--;
        if (countdown_til_widening == 0)
        {
            countdown_til_widening = max_ups_per_width;
            max_ups_per_width *= 2; //+= 2; canyons // ++ made it more like valleys
            ray_length++;
        }
    }
}

void add_gorge(int length, int* peaks, float* noise)
{
    int ox = randrange(0, map_dim.x - 1);
    int oy = randrange(0, map_dim.y - 1);
    int x = ox;
    int y = oy;

    int steer_angle_idx = randrange(0, map_dim.x - 1);
    int height_idx = randrange(0, map_dim.x - 1);

    float fx = 0.0f;
    float fy = 0.0f;
    float rand_angle = (DOUBLE_PI / NUM_LOOKUP_ANGLES) * randrange(0, NUM_LOOKUP_ANGLES - 1);
    float curr_angle = 0.0f;
    float curr_perlin_hei = 0.0f; // height

    // find the limits of spectrum for this 1D strip (out of a 2D map)
    float farthest_from_zero = 0.0f;
    for (int i = 0; i < map_dim.x; i++)
    {
        float curr = abs(noise[height_idx + i * map_dim.x]);
        farthest_from_zero = GS_MAX(farthest_from_zero, curr);
    }
    if (farthest_from_zero != 0.0f)
        farthest_from_zero = 1.0f / farthest_from_zero;

    //float tot_hei_of_poss = farthest_from_zero * 2; // total height of possibility space for random perlin strip

    float tri_shaped_hei = 1; // for gradualizing endpoints easily.  maximum value in the middle, tapering to 1 at the endpoints
    int len_remain = length * 2; // length remaining before we're done with this gorge (* 2 cuz we travel in half cube increments
    int old_quant_angle = 0;
    while (len_remain > 0)
    {
        // get full res angle
        int noise_index = steer_angle_idx + len_remain * map_dim.x;
        IF_ASSERT(noise_index < 0 || noise_index >= map_dim.x * map_dim.y) break;
        curr_angle = rand_angle + noise[noise_index];
        // keep angle in a range that maps to an array
        curr_angle = fmodf(curr_angle, DOUBLE_PI);
        if (curr_angle < 0)
            curr_angle += DOUBLE_PI;

        float highest = peaks[map_dim.x * y + x];
        float lowest = highest - tri_shaped_hei;
        //if (i == 0)     printf("_____________________________________________________\n", highest);
        //if (i == 0)     printf("tri_shaped_hei: %.1f\n", tri_shaped_hei);
        lowest = GS_MAX(lowest, 6);  // keep from getting too close to bedrock
        float edge_to_center_dist = (highest - lowest) / 2.0f; // from center of possibility space
        //// use perlin to map to that space
        noise_index = height_idx + len_remain * map_dim.x;
        IF_ASSERT(noise_index < 0 || noise_index >= map_dim.x * map_dim.y) break;
        curr_perlin_hei = noise[noise_index];
        if (curr_perlin_hei < 0)
            curr_perlin_hei *= -farthest_from_zero; // turn it into a 0.0 - 1.0 range
        else
            curr_perlin_hei *=  farthest_from_zero; // turn it into a 0.0 - 1.0 range
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
        int span = abs(quant_angle - old_quant_angle);
        for (int i=0; i<span; ++i)
            carve_angled_gorge_slice(x, y, fz, i);

        carve_angled_gorge_slice(x, y, fz, quant_angle);

        // move half unit forward
        fx += sin_lookup_table[quant_angle] / 2;
        fy += cos_lookup_table[quant_angle] / 2;

        x = translate_point(ox + fx);
        y = translate_point(oy + fy);

        carve_angled_gorge_slice(x, y, fz, quant_angle);

        if (len_remain < length) // half to match the * 2 of len_remain
            tri_shaped_hei -= 0.45f;
        else
            tri_shaped_hei += 0.45f;

        //if (i == 0) printf("tri_shaped_hei: %f\n", tri_shaped_hei);

        len_remain--;
        old_quant_angle = quant_angle;
    }
}

void add_gorges(int num_gorges, int length)
{
    int t = _GET_MS_TIME();
    printf("\tgorges......");
    float* noise = t_gen::create_2d_noise_array(PERSISTENCE, OCTAVES, map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;

    int* peaks = (int*)malloc(map_dim.x * map_dim.y * sizeof(int));
    IF_ASSERT(peaks == NULL) return;

    for (int i=0; i < map_dim.x; i++)
    for (int j=0; j < map_dim.y; j++)
    {   // build height cache
        int k = map_dim.x * j + i;
        peaks[k] = t_map::get_highest_solid_block(i, j);
    }

    for (int i = 0; i < num_gorges; i++)
        add_gorge(length, peaks, noise);

    free(peaks);
    free(noise);
    printf(" %i ms\n", _GET_MS_TIME() - t);
}

void add_flora()
{
    int t = _GET_MS_TIME();
    printf("\tflora......");
    const float FLORA_ZONE_THRESHOLD = 0.3f;
    const float FLORA_THRESHOLD = 0.8f;
    const CubeType reg = t_map::get_cube_type("regolith");
    const MechType gr1 = t_mech::get_mech_type_dat("grass1");
    const MechType gr2 = t_mech::get_mech_type_dat("grass2");
    const MechType gr3 = t_mech::get_mech_type_dat("grass3");
    const int FLORA_TYPE_MAX = 3;

    float* noise = t_gen::create_2d_noise_array(PERSISTENCE, OCTAVES, map_dim.x, map_dim.y);
    IF_ASSERT(noise == NULL) return;
    float farthest_from_zero = 0.0f;
    // find the limits of the perlin values (atm, it isn't -1.0f to 1.0f)
    for (int x = 0; x < map_dim.x; x++)
    for (int y = 0; y < map_dim.y; y++)
    {
        float curr = abs(noise[x + y*map_dim.x]);
        farthest_from_zero = GS_MAX(farthest_from_zero, curr);
    }

    const float FLORA_SUBREGION_SPAN = (farthest_from_zero - FLORA_ZONE_THRESHOLD) / (FLORA_TYPE_MAX + 1); // + 1 cuz those most extreme numbers are too rare

    int lowest_x = 0;
    int highest_x = 0;
    int lowest_y = 0;
    int highest_y = 0;
    int lowest_z = 0;
    int highest_z = 0;

    // visit every cube column
    for (int x=0; x < map_dim.x; x++)
    for (int y=0; y < map_dim.y; y++)
    {
        float curr_per = noise[x + y*map_dim.x];  // current perlin

        if (curr_per > FLORA_ZONE_THRESHOLD &&
            mrandf() > FLORA_THRESHOLD)
        {
            int z = t_map::get_highest_solid_block(x, y);
            IF_ASSERT(x < 0 || x >= map_dim.x) return;
            IF_ASSERT(y < 0 || y >= map_dim.y) return;
            IF_ASSERT(z < 1 || z >= map_dim.z) return;  // gonna want bedrock always at least 1 layer high?

            if (t_map::get(x, y, z) == reg &&
                t_map::get(x, y, z+1) == EMPTY_CUBE)
            {
                MechType mt;
                float curr_thresh = FLORA_ZONE_THRESHOLD;

                if (curr_per < (curr_thresh += FLORA_SUBREGION_SPAN)) mt = gr1;
                else
                if (curr_per < (curr_thresh += FLORA_SUBREGION_SPAN)) mt = gr2;
                else                                                  mt = gr3;

                t_mech::create_mech(x, y, z+1, mt);

                lowest_x = GS_MIN(x, lowest_x);
                highest_x = GS_MAX(x, highest_x);
                lowest_y = GS_MIN(y, lowest_y);
                highest_y = GS_MAX(y, highest_y);
                lowest_z = GS_MIN(z + 1, lowest_z);
                highest_z = GS_MAX(z + 1, highest_z);
            }
        }
    }
    free(noise);
    printf(" %i ms\n", _GET_MS_TIME() - t);
    printf("FLORA lowest_x: %d  highest_x: %d \n", lowest_x, highest_x);
    printf("FLORA lowest_y: %d  highest_y: %d \n", lowest_y, highest_y);
    printf("FLORA lowest_z: %d  highest_z: %d \n", lowest_z, highest_z);
}

void add_terrain_features()
{
    printf("add_terrain_features: \n");

    GS_ASSERT(sin_lookup_table == NULL);
    GS_ASSERT(cos_lookup_table == NULL);

    sin_lookup_table = new float[NUM_LOOKUP_ANGLES];
    for (int i = 0; i < NUM_LOOKUP_ANGLES; i++)
        sin_lookup_table[i] = sinf((DOUBLE_PI / NUM_LOOKUP_ANGLES) * i);
    cos_lookup_table = new float[NUM_LOOKUP_ANGLES];
    for (int i = 0; i < NUM_LOOKUP_ANGLES; i++)
        cos_lookup_table[i] = cosf((DOUBLE_PI / NUM_LOOKUP_ANGLES) * i);

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

    // add the features
#if PRODUCTION
    add_gorges(GORGE_COUNT, GORGE_LENGTH);
#endif

    add_shrooms();

    add_trees();
    add_flora();

    delete[] sin_lookup_table;
    delete[] cos_lookup_table;
    delete[] leaves;
    delete[] trunks;
    delete[] shroom_caps;
    delete[] shroom_stems;
}

}   // t_gen
