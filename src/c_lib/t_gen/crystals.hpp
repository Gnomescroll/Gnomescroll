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

const float CRYSTAL_CLUSTER_PROBABILITY = 0.012f; // this might need to be dynamic to meet a target cluster count
const int CRYSTAL_CLUSTER_RADIUS = 3;
const float CRYSTAL_CLUSTER_FALLOFF = 0.6f;
const CrystalClusterMode CRYSTAL_CLUSTER_MODE = CRYSTAL_CLUSTER_LINEAR;
const int CRYSTAL_CLUSTER_Z_DIFF_MAX = 3;

const int n_crystals = 3;
int crystals[n_crystals] = {t_map::ERROR_CUBE };
float crystal_strata[n_crystals*2] = {0};

int rock = t_map::ERROR_CUBE;
int bedrock = t_map::ERROR_CUBE;

float falloffs[CRYSTAL_CLUSTER_RADIUS*2+1] = {1.0f};

int* cluster_size = NULL;
int cluster_id = 0;
int crystal_type_count[n_crystals] = {0};

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
    crystals[i++] = t_mech::get_mech_type_id("blue_crystal");
    crystals[i++] = t_mech::get_mech_type_id("red_crystal");
    crystals[i++] = t_mech::get_mech_type_id("green_crystal");
    // TODO -- check that all crystals are not ERROR_MECH

    bedrock = t_map::get_cube_id("bedrock");
    GS_ASSERT(bedrock != t_map::ERROR_CUBE);

    rock = t_map::get_cube_id("rock");
    GS_ASSERT(rock != t_map::ERROR_CUBE);

    //int top = get_highest_block_of_type(rock) + 1;

    //int step = (int)ceil(((1.0f/3.0f) * ((float)top)));
    
    //// calculate stratification for crystal distribution
    //for (int i=0; i<n_crystals; i++)
    //{
        //crystal_strata[2*i+0] = step * (3-i-0);
        //crystal_strata[2*i+1] = step * (3-i-1);
    //}

    i = 0;
    crystal_strata[i++] = 0.0f;
    crystal_strata[i++] = 0.425f;
    crystal_strata[i++] = 0.425f;
    crystal_strata[i++] = 0.75f;
    crystal_strata[i++] = 0.75f;
    crystal_strata[i++] = 1.0f;

    for (int i=0; i<CRYSTAL_CLUSTER_RADIUS*2+1; i++)
        falloffs[i] = powf(CRYSTAL_CLUSTER_FALLOFF, i);
}

int get_crystal_index(int crystal_id)
{
    for (int i=0; i<n_crystals; i++)
        if (crystals[i] == crystal_id)
            return i;
    GS_ASSERT(false);
    return 0;
}

void place_crystal_cluster(int x, int y, int z, int crystal_id)
{
    
    for (int i=x-CRYSTAL_CLUSTER_RADIUS; i<x+CRYSTAL_CLUSTER_RADIUS; i++)
    for (int j=y-CRYSTAL_CLUSTER_RADIUS; j<y+CRYSTAL_CLUSTER_RADIUS; j++)
    {
        int dist = abs(i-x) + abs(j-y); // manhattan
        float p = falloffs[dist];
        
        if ((float)genrand_real1() > p) continue;
        int ii = translate_point(i);
        int jj = translate_point(j);
        int k = t_map::get_nearest_open_block(ii,jj,z);
        if (abs(z-k) > CRYSTAL_CLUSTER_Z_DIFF_MAX) continue;
        int id = t_map::get(ii,jj,k-1);
        if (id != rock) continue;

        if( t_mech::can_place_crystal(ii,jj,k, 0) == false) //check to see if placement is valid
            continue;
        t_mech::create_crystal(ii,jj,k,crystal_id);
        cluster_size[cluster_id]++;
        crystal_type_count[get_crystal_index(crystal_id)]++;
    }
    cluster_id++;
}

int get_crystal_type(float percent_complete)
{
    int i=0;
    for (; i<n_crystals; i++)
    //{
        //printf("pct: %f; i %d; range %0.2f-%0.2f\n", percent_complete, i, crystal_strata[2*i+0], crystal_strata[2*i+1]);
        if (percent_complete >= crystal_strata[2*i+0] && percent_complete < crystal_strata[2*i+1])
            return crystals[i];
    //}
    GS_ASSERT(false);
    printf("pct error: %f\n", percent_complete);
    return crystals[n_crystals-1];
}

void populate_crystals()
{
    printf("Begin populate crystals\n");
    init_crystals();
    
    /* Heuristic:
     *      If block == "rock" and block.z+1 == 0 and genrand_real1() < p
     *          place crystal cluster
     */

    GS_ASSERT(rock != t_map::ERROR_CUBE);
    GS_ASSERT(bedrock != t_map::ERROR_CUBE);

    int ct = 0;
    int ct_max = 1000;
    int* loc = (int*)malloc(3 * ct_max * sizeof(int));

    for (int k=127; k>=0; k--)
    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
    {
        if ((float)genrand_real1() > CRYSTAL_CLUSTER_PROBABILITY) continue;   // probability test
        int id = t_map::get(i,j,k); // get cube
        if (id != rock) continue;
        if (t_map::get(i,j,k+1) != 0) continue; // check block above is open

        if (ct >= ct_max)
        {
            ct_max *= 2;
            int* tmp = (int*)realloc(loc, 3*ct_max*sizeof(int));
            GS_ASSERT(tmp != NULL);
            if (tmp == NULL)
            {
                ct_max /= 2;
                break;
            }
            loc = tmp;
        }

        loc[3*ct+0] = i;
        loc[3*ct+1] = j;
        loc[3*ct+2] = k+1;
        ct++;
    }

    GS_ASSERT(ct > 0);
    if (ct <= 0)
    {
        free(loc);
        return;
    }

    cluster_size = (int*)calloc(ct, sizeof(int));

    float pct = 0.0f;
    float inc = 1.0f/(float)ct;
    for (int i=0; i<ct; i++)
    {
        int crystal_id = get_crystal_type(pct);
        int x = loc[3*i+0];
        int y = loc[3*i+1];
        int z = loc[3*i+2];
        place_crystal_cluster(x,y,z, crystal_id);
        pct += inc;
    }

    free(loc);

    printf("Placed %d crystal clusters\n", ct);

    int sum = 0;
    for (int i=0; i<ct; i++)
        sum += cluster_size[i];

    float avg_cluster_size = ((float)sum)/((float)ct);
    printf("Average cluster size: %f\n", avg_cluster_size);

    printf("Crystal counts:\n");
    for (int i=0; i<n_crystals; i++)
        printf("\t%d: %d\n", i, crystal_type_count[i]);

    free(cluster_size);
}

}   // t_gen
