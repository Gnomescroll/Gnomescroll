#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

namespace t_gen
{

typedef enum
{
    CRYSTAL_CLUSTER_LINEAR,
    CRYSTAL_CLUSTER_GAUSSIAN,
}   CrystalClusterMode;

const float CRYSTAL_CLUSTER_PROBABILITY = 0.012f; // this might need to be dynamic to meet a target cluster count
const int CRYSTAL_CLUSTER_RADIUS = 3;
const float CRYSTAL_CLUSTER_FALLOFF = 0.6f;
const CrystalClusterMode CRYSTAL_CLUSTER_MODE = CRYSTAL_CLUSTER_LINEAR;
const int CRYSTAL_CLUSTER_Z_DIFF_MAX = 3;

const int n_crystals = 3;
MechType crystals[n_crystals] = { NULL_MECH_TYPE };
float crystal_strata[n_crystals*2] = {0};

CubeType rock = ERROR_CUBE;
CubeType bedrock = ERROR_CUBE;

float falloffs[CRYSTAL_CLUSTER_RADIUS*2+1] = {1.0f};

int* cluster_size = NULL;
int cluster_id = 0;
int crystal_type_count[n_crystals] = {0};

// TODO: move
int get_highest_block_of_type(CubeType cube_type)
{
    for (int k=127; k>=0; k--)
    for (int i=0; i<512; i++)
    for (int j=0; j<512; j++)
        if (t_map::get(i,j,k) == cube_type)
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

    bedrock = t_map::get_cube_type("bedrock");
    IF_ASSERT(!isValid(bedrock)) return;

    rock = t_map::get_cube_type("rock");
    IF_ASSERT(!isValid(rock)) return;

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

void place_crystal_cluster(const Vec3i& position, MechType crystal_id)
{
    const float r = CRYSTAL_CLUSTER_RADIUS;
    for (int i=position.x - r; i<position.x + r; i++)
    for (int j=position.y - r; j<position.y + r; j++)
    {
        int dist = abs(i - position.x) + abs(j - position.y); // manhattan
        float p = falloffs[dist];

        if (mrandf() > p) continue;
        Vec3i pos = vec3i_init(i, j, position.z);
        pos = translate_position(pos);
        pos.z = t_map::get_nearest_surface_block(pos);
        if (abs(position.z - pos.z) > CRYSTAL_CLUSTER_Z_DIFF_MAX) continue;
        int id = t_map::get(pos.x, pos.y, pos.z - 1);
        if (id != rock) continue;
        if (!t_mech::can_place_mech(pos, crystal_id))
            continue;
        t_mech::create_crystal(pos, crystal_id);
        cluster_size[cluster_id]++;
        crystal_type_count[get_crystal_index(crystal_id)]++;
    }
    cluster_id++;
}

MechType get_crystal_type(float percent_complete)
{
    for (int i=0; i<n_crystals; i++)
        if (percent_complete >= crystal_strata[2*i+0] && percent_complete < crystal_strata[2*i+1])
            return crystals[i];
    GS_ASSERT(false);
    printf("pct error: %f\n", percent_complete);
    return crystals[n_crystals-1];
}

void populate_crystals()
{
    init_crystals();

    /* Heuristic:
     *      If block == "rock" and block.z+1 == 0 and mrandf() < p
     *          place crystal cluster
     */

    GS_ASSERT(rock != ERROR_CUBE);
    GS_ASSERT(bedrock != ERROR_CUBE);

    int ct = 0;
    int ct_max = 1000;
    int* loc = (int*)malloc(3 * ct_max * sizeof(int));

    for (int k=map_dim.z-1; k>=0; k--)
    for (int i=0; i<map_dim.x; i++)
    for (int j=0; j<map_dim.y; j++)
    {
        if (mrandf() > CRYSTAL_CLUSTER_PROBABILITY) continue;   // probability test
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

    IF_ASSERT(ct <= 0)
    {
        free(loc);
        return;
    }

    cluster_size = (int*)calloc(ct, sizeof(int));

    float pct = 0.0f;
    float inc = 1.0f/ct;
    for (int i=0; i<ct; i++)
    {
        MechType crystal_id = get_crystal_type(pct);
        int x = loc[3*i+0];
        int y = loc[3*i+1];
        int z = loc[3*i+2];
        place_crystal_cluster(vec3i_init(x, y, z), crystal_id);
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
