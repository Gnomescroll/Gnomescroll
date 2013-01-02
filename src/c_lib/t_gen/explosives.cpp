#include "explosives.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
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
static CubeID immune_cubes[MAX_IMMUNE_CUBES];

static CubeID plasmagen = NULL_CUBE;
static CubeID rock_landmine = NULL_CUBE;
static CubeID regolith_landmine = NULL_CUBE;

static CubeID add_immune_cube(const char* name)
{
    IF_ASSERT(immune_cubes_ct >= MAX_IMMUNE_CUBES) return NULL_CUBE;
    CubeID cube_id = t_map::get_cube_id(name);
    GS_ASSERT(cube_id != ERROR_CUBE);
    immune_cubes[immune_cubes_ct++] = cube_id;
    return cube_id;
}

void init_explosives()
{
    add_immune_cube("empty_block");
    add_immune_cube("bedrock");
    add_immune_cube("steel_block_1");
    add_immune_cube("steel_block_2");
    add_immune_cube("steel_block_3");

    plasmagen = t_map::get_cube_id("plasmagen");
    GS_ASSERT(t_map::isValidCube(plasmagen));

    rock_landmine = t_map::get_cube_id("rock_landmine");
    GS_ASSERT(t_map::isValidCube(rock_landmine));

    regolith_landmine = t_map::get_cube_id("regolith_landmine");
    GS_ASSERT(t_map::isValidCube(regolith_landmine));
}

bool isLandmine(int x, int y, int z)
{   // TODO -- make property in CubeAttributes
    CubeID place = t_map::get(x, y, z);
    return (place == rock_landmine || place == regolith_landmine);
}

void create_explosion(const int x, const int y, const int z)
{   // WARNING: make sure this function is called after destroying the explosive block
    // check upper bounds
    // we should not be calling this function out of bounds, so assert
    IF_ASSERT(x < 0 || x >= t_map::map_dim.x) return; //stop IF_ASSERTthe location of the explosive is not valid
    IF_ASSERT(y < 0 || y >= t_map::map_dim.y) return;
    IF_ASSERT(z <= 0 || z >= t_map::map_dim.z) return;    // also check the floor

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

    const struct Vec3 position = vec3_init(x+0.5f, y+0.5f, z+0.5f);

    // notify clients
    Particle::plasmagen_explode_StoC msg;
    msg.position = position;
    msg.broadcast();

    // boundaries for the explosion, which can be contained by iron and bedrock
    const int pos[3] = { x,y,z };
    int bounds[3][2];
    CubeID cubes[3][2];
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
        int p[3];
        for (int m=0; m<3; m++)
            p[m] = pos[m] + j*sides[i][m]*dir[k];
        for (int m=0; m<2; m++)
            p[m] = translate_point(p[m]);

        if (bounds[i][k] >= j)
        {
            CubeID cube_id = t_map::get(p[0], p[1], p[2]);
            if (cube_id != EMPTY_CUBE)
            {
                bounds[i][k] = j;
                cubes[i][k] = cube_id;
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
            int p[3];
            for (int k=0; k<3; k++)
                p[k] = pos[k] + sides[i][k]*bounds[i][j];
            for (int k=0; k<2; k++)
                p[k] = translate_point(p[k]);

            t_map::apply_damage_broadcast(
                p[0], p[1], p[2],
                PLASMAGEN_BLOCK_DAMAGE,
                TMA_PLASMAGEN);

            // block behind this one
            for (int k=0; k<3; k++)
                p[k] += sides[i][k]*dir[j];
            for (int k=0; k<2; k++)
                p[k] = translate_point(p[k]);
            if (!in_array(t_map::get(p[0], p[1], p[2]), immune_cubes, immune_cubes_ct))
                t_map::apply_damage_broadcast(
                    p[0], p[1], p[2],
                    PLASMAGEN_BLOCK_DAMAGE,
                    TMA_PLASMAGEN);
        }

    // hitscan all objects in paths
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
    {
        size_t n_hit = 0;
        struct Vec3 end = vec3_add(position, vec3_scalar_mult(vsides[i], bounds[i][j]+0.5f));
        class Voxels::VoxelHitscanTarget* targets = ServerState::voxel_hitscan_list->hitscan_all(position, end, &n_hit);
        if (targets == NULL) continue;
        for (size_t k=0; k<n_hit; k++)
            Hitscan::damage_target(&targets[k], OBJECT_PLASMAGEN, PLASMAGEN_ENTITY_DAMAGE);
    }
}

}   // t_gen
