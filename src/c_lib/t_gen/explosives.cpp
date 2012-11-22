#include "explosives.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <physics/quadrant.hpp>
#include <state/server_state.hpp>

namespace t_gen
{

static const size_t MAX_IMMUNE_CUBES = 10;
static size_t immune_cubes_ct = 0; 
static CubeID immune_cubes[MAX_IMMUNE_CUBES];

static CubeID cannonball = NULL_CUBE;
static CubeID plasmagen = NULL_CUBE;

static CubeID add_immune_cube(const char* name)
{
    GS_ASSERT(immune_cubes_ct < MAX_IMMUNE_CUBES);
    if (immune_cubes_ct >= MAX_IMMUNE_CUBES) return NULL_CUBE;
    CubeID cube_id = t_map::get_cube_id(name);
    GS_ASSERT(t_map::isValidCube(cube_id));
    if (!t_map::isValidCube(cube_id)) return NULL_CUBE;
    immune_cubes[immune_cubes_ct++] = cube_id;
    return cube_id;
}

void init_explosives()
{
    add_immune_cube("empty_block");
    add_immune_cube("bedrock");
    add_immune_cube("steel_block_1");
    cannonball = add_immune_cube("steel_block_2");
    add_immune_cube("steel_block_3");

    plasmagen = t_map::get_cube_id("plasmagen");
    GS_ASSERT(t_map::isValidCube(plasmagen));
}

static void shoot_cannonball()
{
    // TODO
}

void create_explosion(const int x, const int y, const int z)
{
    // check upper bounds
    // we should not be calling this function out of bounds, so assert
    GS_ASSERT(x >= 0 && x < t_map::map_dim.x);
    GS_ASSERT(y >= 0 && y < t_map::map_dim.y);
    GS_ASSERT(z > 0 && z < t_map::map_dim.z);
    if (x < 0 || x >= t_map::map_dim.x) return; //stop if the location of the explosive is not valid
    if (y < 0 || y >= t_map::map_dim.y) return;
    if (z <= 0 || z >= t_map::map_dim.z) return;    // also check the floor

    // constant for helping walk the axes
    static const int dir[2] = {-1, 1};
    static const int sides[3][3] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }};

    // boundaries for the explosion, which can be contained by iron and bedrock
    int pos[3] = { x,y,z };
    int bounds[3][2];
    CubeID cubes[3][2];
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
    {
        bounds[i][j] = pos[i] + dir[j]*PLASMAGEN_BLAST_RADIUS;
        cubes[i][j] = EMPTY_CUBE;
    }

    // determine blocked paths for the explosion
    // starting from the center, walk outwards in both directions at the same time
    // for each axis. if the cube is solid, mark it and stop.
    // otherwish, damage objects within a sphere that fits inside the empty cube
    for (int i=0; i<3; i++)
    for (int j=0; j<PLASMAGEN_BLAST_RADIUS; j++)
    {
        int p[3];
        for (int m=0; m<3; m++)
            p[m] = pos[m] + j*sides[i][i];
        p[0] = translate_point(p[0]);
        p[1] = translate_point(p[1]);

        if (bounds[i][0] <= j)
        {
            CubeID cube_id = t_map::get(p[0], p[1], p[2]);
            if (cube_id == EMPTY_CUBE)
            {
                ServerState::damage_objects_within_sphere(
                    vec3_init(p[0]+0.5f, p[1]+0.5f, p[2]+0.5f),
                    0.5f, PLASMAGEN_ENTITY_DAMAGE, NULL_AGENT,
                    OBJECT_PLASMAGEN, NULL_ENTITY_ID, false); 
            }
            else
            {
                bounds[i][0] = -j;
                cubes[i][0] = cube_id;
            }
        }

        for (int m=0; m<3; m++)
            p[m] = pos[m] - j*sides[i][i];
        p[0] = translate_point(p[0]);
        p[1] = translate_point(p[1]);

        if (bounds[i][1] >= j)
        {
            CubeID cube_id = t_map::get(p[0], p[1], p[2]);
            if (cube_id == EMPTY_CUBE)
            {
                ServerState::damage_objects_within_sphere(
                    vec3_init(p[0]+0.5f, p[1]+0.5f, p[2]+0.5f),
                    0.5f, PLASMAGEN_ENTITY_DAMAGE, NULL_AGENT,
                    OBJECT_PLASMAGEN, NULL_ENTITY_ID, false); 
            }
            else
            {
                bounds[i][0] = j;
                cubes[i][1] = cube_id;
            }
        }
    }
        
    for (int i=0; i<3; i++)
    for (int j=0; i<2; j++)
    {   // damage each of the stop blocks heavily, if they are not immune
        if (!in_array(cubes[i][j], immune_cubes, immune_cubes_ct))
            t_map::set(
                x+sides[i][i]*bounds[i][j],
                y+sides[i][i]*bounds[i][j],
                z+sides[i][i]*bounds[i][j], EMPTY_CUBE);   // TODO -- apply high damage with a TMA

        // shoot cannonball
        //if (cubes[i][j] == cannonball)
        //{
            //shoot_cannonball();
        //}
    }
}

}   // t_gen
