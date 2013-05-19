#include "explosives.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <physics/quadrant.hpp>
#include <state/server_state.hpp>
#include <particle/plasmagen_spur.hpp>

namespace t_gen
{

static const size_t MAX_IMMUNE_CUBES = 10;
static size_t immune_cubes_ct = 0;
static CubeType immune_cubes[MAX_IMMUNE_CUBES];

static CubeType plasmagen = NULL_CUBE;
static CubeType rock_landmine = NULL_CUBE;
static CubeType regolith_landmine = NULL_CUBE;

static CubeType add_immune_cube(const char* name)
{
    IF_ASSERT(immune_cubes_ct >= MAX_IMMUNE_CUBES) return NULL_CUBE;
    CubeType cube_type = t_map::get_cube_type(name);
    GS_ASSERT(cube_type != ERROR_CUBE);
    immune_cubes[immune_cubes_ct++] = cube_type;
    return cube_type;
}

void init_explosives()
{
    add_immune_cube("empty_block");
    add_immune_cube("bedrock");
    add_immune_cube("steel_block_1");
    add_immune_cube("steel_block_2");
    add_immune_cube("steel_block_3");

    plasmagen = t_map::get_cube_type("plasmagen");
    GS_ASSERT(t_map::isValidCube(plasmagen));

    rock_landmine = t_map::get_cube_type("rock_landmine");
    GS_ASSERT(t_map::isValidCube(rock_landmine));

    regolith_landmine = t_map::get_cube_type("regolith_landmine");
    GS_ASSERT(t_map::isValidCube(regolith_landmine));
}

//bool is_landmine(const Vec3i& position)
//{   // TODO -- make property in CubeAttributes
    //CubeType place = t_map::get(position);
    //return (place == rock_landmine || place == regolith_landmine);
//}

void explode_landmine_damage_players(const Vec3i& position)
{
    printf("Landmine damaging players at "); vec3i_print(position);
    Vec3 p = vec3_add(vec3_init(position), vec3_init(0.5f));
    Hitscan::against_agents(p, vec3_init(0, 0, 1), 2);
}

void create_explosion(const Vec3i& position)
{   // WARNING: make sure this function is called after destroying the explosive block
    // check upper bounds
    // we should not be calling this function out of bounds, so assert
    printf("Creating explosion at "); vec3i_print(position);
    IF_ASSERT(!is_boxed_position(position)) return;

    // constant for helping walk the axes
    static const int dir[2] = { -1, 1 };
    static const int sides[3][3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }};
    static const struct Vec3 vsides[3] = {
        vec3_init(1, 0, 0),
        vec3_init(0, 1, 0),
        vec3_init(0, 0, 1)};

    const struct Vec3 fp = vec3_add(vec3_init(position), vec3_init(0.5f));

    // notify clients
    Particle::plasmagen_explode_StoC msg;
    msg.position = fp;
    msg.broadcast();

    // boundaries for the explosion, which can be contained by iron and bedrock
    int bounds[3][2];
    CubeType cubes[3][2];
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
    {
        bounds[i][j] = PLASMAGEN_BLAST_RADIUS;
        cubes[i][j] = EMPTY_CUBE;
    }

    // determine blocked paths for the explosion
    // starting from the center, walk outwards in both directions at the same time
    // for each axis. if the cube is solid, mark it and stop.
    // otherwish, damage objects within a sphere that fits inside the empty cube
    for (int i=0; i<3; i++)
    for (int j=1; j<PLASMAGEN_BLAST_RADIUS; j++)
    for (int k=0; k<2; k++)
    {
        Vec3i p;
        for (int m=0; m<3; m++)
            p.i[m] = position.i[m] + j*sides[i][m]*dir[k];
        p = translate_position(p);

        if (bounds[i][k] >= j)
        {
            CubeType cube_type = t_map::get(p);
            if (cube_type != EMPTY_CUBE)
            {
                bounds[i][k] = j;
                cubes[i][k] = cube_type;
            }
        }
    }

    // flip lower bounds
    for (int i=0; i<3; i++)
        bounds[i][0] *= -1;

    // damage each of the stop blocks and the block behind them heavily,
    // if they are not immune
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
        if (!in_array(cubes[i][j], immune_cubes, immune_cubes_ct))
        {
            Vec3i p;
            for (int k=0; k<3; k++)
                p.i[k] = position.i[k] + sides[i][k]*bounds[i][j];
            p = translate_position(p);

            t_map::apply_damage_broadcast(p, PLASMAGEN_BLOCK_DAMAGE, TMA_PLASMAGEN);

            // block behind this one
            for (int k=0; k<3; k++)
                p.i[k] += sides[i][k]*dir[j];
            p = translate_position(p);
            if (!in_array(t_map::get(p), immune_cubes, immune_cubes_ct))
                t_map::apply_damage_broadcast(p, PLASMAGEN_BLOCK_DAMAGE, TMA_PLASMAGEN);
        }

    // hitscan all objects in paths
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
    {
        size_t n_hit = 0;
        struct Vec3 end = vec3_add(fp, vec3_scalar_mult(vsides[i], bounds[i][j]+0.5f));
        class Voxels::VoxelHitscanTarget* targets = ServerState::voxel_hitscan_list->hitscan_all(fp, end, &n_hit);
        if (targets == NULL) continue;
        for (size_t k=0; k<n_hit; k++)
            Hitscan::damage_target(&targets[k], OBJECT_PLASMAGEN, PLASMAGEN_ENTITY_DAMAGE);
    }
}

}   // t_gen
