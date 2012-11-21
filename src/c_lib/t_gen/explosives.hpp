#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <common/random.hpp>
#include <physics/quadrant.hpp>

namespace t_gen
{

const int MAX_STRENGTH = 20;

void create_explosion(const int x, const int y, const int z, const int strength)
{
    // check for sane strength value
    GS_ASSERT(strength > 0 && strength <= MAX_STRENGTH);
    if (strength <= 0 || strength > MAX_STRENGTH) return;
    
    // get and check cubes to be used
    static CubeID bedrock = t_map::get_cube_id("bedrock");
    static CubeID iron = t_map::get_cube_id("steel_block_1");
    static CubeID plasmagen = t_map::get_cube_id("plasmagen");
    GS_ASSERT(t_map::isValidCube(bedrock));
    GS_ASSERT(t_map::isValidCube(iron));
    GS_ASSERT(t_map::isValidCube(plasmagen));
    if (!t_map::isValidCube(bedrock)) return;
    if (!t_map::isValidCube(iron)) return;
    if (!t_map::isValidCube(plasmagen)) return;

    // check upper bounds
    // we should not be calling this function out of bounds, so assert
    GS_ASSERT(x >= 0 && x < t_map::map_dim.x);
    GS_ASSERT(y >= 0 && y < t_map::map_dim.y);
    GS_ASSERT(z > 0 && z < t_map::map_dim.z);
    if (x < 0 || x >= t_map::map_dim.x) return; //stop if the location of the explosive is not valid
    if (y < 0 || y >= t_map::map_dim.y) return;
    if (z <= 0 || z >= t_map::map_dim.z) return;    // also check the floor

    // boundaries for the explosion, which can be contained by iron and bedrock
    int xstart = x - strength;
    int ystart = y - strength;
    int zstart = z - strength;
    int xend = x + strength;
    int yend = x + strength;
    int zend = x + strength;

    // determine blocked paths for the explosion
    for (int i=xstart; i<xend; i++)
    for (int j=ystart; j<yend; j++)
    for (int k=zstart; k<zend; k++)
    {
        if (k < 0 || k >= t_map::map_dim.z) continue;
        int ix = translate_point(i);
        int iy = translate_point(j);
        int iz = k;

        CubeID cube_id = t_map::get(ix, iy, iz);
        if (cube_id == bedrock || cube_id == iron)
        {
            if (ix <= x) xstart = ix;
            if (ix >= x) xend = ix;

            if (iy <= y) ystart = iy;
            if (iy >= y) yend = iy;

            if (iz <= z) zstart = iz;
            if (iz >= z) zend = iz;

            continue;
        }
    }

    // do explosion within bounds defined by blocked paths
    for (int i=xstart; i<xend; i++)
    for (int j=ystart; j<yend; j++)
    for (int k=zstart; k<zend; k++)
    {
        if (k < 0 || k >= t_map::map_dim.z) continue;
        int ix = translate_point(i);
        int iy = translate_point(j);
        int iz = k;

        CubeID cube_id = t_map::get(ix, iy, iz);
        if (cube_id == plasmagen && ix != x && iy != y && iz != z)
        {
            // make sure to unset the plasmegen cube,
            // or we will end up in infinite recursion and crash
            t_map::set(ix, iy, iz, EMPTY_CUBE);
            create_explosion(ix, iy, iz, strength);
            continue;
        }

        // check the cube value again; we should not have gone
        // this far since we gathered boundary data
        GS_ASSERT(cube_id != bedrock && cube_id != iron);
        if (cube_id == bedrock || cube_id == iron) continue;

        // TODO -- switch to t_map::apply_damage_broadcast();
        t_map::set(iz, iy, iz, EMPTY_CUBE);
    }
}

}   // t_gen
