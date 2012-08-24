#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{

typedef enum
{
    CRYSTAL_CLUSTER_LINEAR,
    CRYSTAL_CLUSTER_GAUSSIAN,
} CrystalClusterMode;

const float CRYSTAL_CLUSTER_PROBABILITY = 0.0015f; // this might need to be dynamic to meet a target cluster count
const int CRYSTAL_CLUSTER_RADIUS = 3;
const float CRYSTAL_CLUSTER_FALLOFF = 0.85f;
const CrystalClusterMode CRYSTAL_CLUSTER_MODE = CRYSTAL_CLUSTER_LINEAR;
const int CRYSTAL_CLUSTER_Z_DIFF_MAX = 3;

const int n_crystals = 3;
int crystals[n_crystals] = {t_map::ERROR_CUBE };
int crystal_strata[n_crystals*2] = {0};

int rock = t_map::ERROR_CUBE;
int bedrock = t_map::ERROR_CUBE;

// TODO: move
int get_highest_block_of_type(int block_id)
{
    for (int k=127; k>=0; k--)
    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
        if (t_map::get(i,j,k) == block_id)
            return k;
    return -1;
}

void init_crystals()
{
    static int inited = 0;
    if (inited++) return;

    // load crystal types
    int i=0;
    crystals[i++] = t_mech::get_mech_type("blue_crystal");
    crystals[i++] = t_mech::get_mech_type("red_crystal");
    crystals[i++] = t_mech::get_mech_type("green_crystal");
    // TODO -- check that all crystals are not ERROR_MECH

    bedrock = t_map::get_cube_id("bedrock");
    GS_ASSERT(bedrock != t_map::ERROR_CUBE);

    rock = t_map::get_cube_id("rock");
    GS_ASSERT(rock != t_map::ERROR_CUBE);

    int top = get_highest_block_of_type(rock) + 1;

    int step = (int)ceil(((1.0f/3.0f) * ((float)top)));
    
    // calculate stratification for crystal distribution
    for (int i=0; i<n_crystals; i++)
    {
        crystal_strata[2*i+0] = step * (3-i-0);
        crystal_strata[2*i+1] = step * (3-i-1);
    }
}
    
void place_crystal_cluster(int x, int y, int z, int crystal_id)
{
    for (int i=x-CRYSTAL_CLUSTER_RADIUS; i<x+CRYSTAL_CLUSTER_RADIUS; i++)
    for (int j=y-CRYSTAL_CLUSTER_RADIUS; j<y+CRYSTAL_CLUSTER_RADIUS; j++)
    {
        float p = ((float)(abs(i-x) + abs(j-y))) * CRYSTAL_CLUSTER_FALLOFF;
        if (randf() > p) continue;
        int ii = translate_point(i);
        int jj = translate_point(j);
        int k = get_nearest_open_block(ii,jj,z);
        if (abs(z-k) > CRYSTAL_CLUSTER_Z_DIFF_MAX) continue;
        int id = t_map::get(ii,jj,k-1);
        if (id != rock && id != bedrock) continue;
        t_mech::create_crystal(ii,jj,k,crystal_id);
    }
}

int get_crystal_type(int z)
{
    int i=0;
    for (; i<n_crystals; i++)
        if (z < crystal_strata[i] && z >= crystal_strata[i-1])
            return crystals[i];
    GS_ASSERT(false);
    return crystals[i];
}

void populate_crystals()
{
    printf("Begin populate crystals\n");
    init_crystals();
    printf("Crystal inited\n");
    
    /* Heuristic:
     *      If block == "rock" and block.z+1 == 0 and randf() < p
     *          place crystal cluster
     */

    GS_ASSERT(rock != t_map::ERROR_CUBE);
    GS_ASSERT(bedrock != t_map::ERROR_CUBE);

    int ct = 0;

    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
    for (int k=0; k<128; k++)
    {
        if (randf() > CRYSTAL_CLUSTER_PROBABILITY) continue;   // probability test
        int id = t_map::get(i,j,k); // get cube
        if (id != rock && id != bedrock) continue;   // check rock
        if (t_map::get(i,j,k+1) != 0) continue; // check block above is open

        // choose crystal type based on z level
        int crystal_id = get_crystal_type(k+1);
        place_crystal_cluster(i,j,k+1, crystal_id);
        ct++;
        printf("placed crystal cluster %d\n", ct);
    }

    printf("placed %d crystal clusters\n", ct);
}

}   // t_gen
