/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_CLIENT
# error Do not include this file in the client
#endif

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <physics/vec3.hpp>
#include <physics/mat4.hpp>
/*
    Add Geods
    -- add a very hard outer shell
    -- add internal shiny mineral layers
*/

namespace t_gen
{
void populate_ore_veins(int number, const char* block_name);
void populate_ore_pocket_cuboid(int number, const char* block_name);
void populate_ore_pocket_elliptoid(int number, const char* block_name);

void populate_ore()
{





    //populate_ore_veins(4*8192, "methane_ice");
    populate_ore_veins(4*8192, "iron_ore");

    populate_ore_veins(4*2048, "coal");

    populate_ore_veins(4*4096, "copper_ore");
    //populate_ore_veins(4*2048, "gallium_ore");
    populate_ore_veins(4*2048, "iridium_ore");

    populate_ore_pocket_cuboid(2*4096, "gallium_ore");
    populate_ore_pocket_elliptoid(8192, "methane_ice");

}

/*
    Add branching ore veins
*/
int generate_ore_vein(int x, int y, int z, int size, CubeType tile_id);


void populate_ore_veins(int number, const char* block_name)
{
    CubeType tile_id = t_map::get_cube_type(block_name);
    const static int regolith_id = t_map::get_cube_type("regolith");

    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;
    const float dephchange=2.0f; //how much more probable it is for an ore to generate lower than higher
    //if (dephchange>1)
        //printf("Ores have a higher probability of generating lower\n");

    for (int i=0; i<number; i++)
    {
        int x = (int)genrand_int32() % map_dim.x;
        int y = (int)genrand_int32() % map_dim.y;
        int z = ((int)genrand_int32() % map_dim.z) / dephchange;
        if (z <= map_dim.z / dephchange / dephchange)
            {
                z = ((int)genrand_int32() % map_dim.z) / dephchange; //make ores lower even more probable
            }

        int ctile = t_map::get(x,y,z);
        if (ctile == 0 || ctile == regolith_id) continue;

        int s = 4 + 4*(genrand_int32() % 4);

        generate_ore_vein(x,y,z, s, tile_id);
    }
}


int generate_ore_vein(int x, int y, int z, int size, CubeType tile_id)
{
    const static int regolith_id = t_map::get_cube_type("regolith");

    const static int_fast8_t s_array[18] = {
            0,0,1,  //top
            0,0,-1, //bottom
            1,0,0,  //north
            -1,0,0, //south
            0,1,0,  //west
            0,-1,0, //east
            };

    int tries = 0;
    int ct = 0;

    while (ct < size && tries < 20)
    {
        int direction = (int) (genrand_int32() % 6);

        int cx = x+s_array[3*direction+0];
        int cy = y+s_array[3*direction+1];
        int cz = z+s_array[3*direction+2];

        int ctile = t_map::get(cx,cy,cz);

        //dont change position
        if (ctile == 0 || ctile == regolith_id)
        {
            tries++;
            continue;
        }

        x = cx;
        y = cy;
        z = cz;

        //ctile = t_map::get(x,y,z);

        z %= 127;
        x %= 512;
        y %= 512;
        if (x < 0) x += 512;
        if (y < 0) y += 512;
        if (z < 0) z++;

        if (ctile == tile_id)
        {
            tries++;
            continue;
        }

        // we need this, because x,y are exceeding twice the bounds
        // of the map

        t_map::set_fast(x,y,z,tile_id);

        tries = 0; //reset
        ct++;
    }

    //if (tries >= 20)
        //printf("Warning: generate_ore_vein 20 attemps made to populate ore vein\n");

    return ct;
}


int generate_ore_pocket_elliptoid(int _x, int _y, int _z, float size, CubeType tile_id);
int generate_ore_pocket_cuboid(int _x, int _y, int _z, float size, CubeType tile_id);

void populate_ore_pocket_cuboid(int number, const char* block_name)
{
    const static CubeType regolith_id = t_map::get_cube_type("regolith");
    CubeType tile_id = t_map::get_cube_type(block_name);

    GS_ASSERT(t_map::isValidCube(tile_id));
    if (!t_map::isValidCube(tile_id)) return;

    int ore_count = 0;
    int clusters = 0;

    for (int i=0; i<number; i++)
    {
        int x = (int)genrand_int32() % map_dim.x;
        int y = (int)genrand_int32() % map_dim.y;
        int z = (int)genrand_int32() % map_dim.z;

        CubeType ctile = t_map::get(x,y,z);
        if (ctile == EMPTY_CUBE || ctile == regolith_id)
            continue;


        //int ctile = t_map::get(x,y,z);
        //if (ctile == 0) continue;

        //int s = 4 + 4*(genrand_int32() % 4);

        float size = 0.0f;
        float value = randf();

        if (value <= 0.50)
        {
            size = 0.50f;
        }
        else if (value <= 0.85f)
        {
            size = 0.75;
        }
        else if (value <= 1.0)
        {
            size = 1.6f;
        }
        //generate_ore_vein(x,y,z, s, tile_id);
        ore_count += generate_ore_pocket_cuboid(x,y,z, size, tile_id);

        clusters++;
    }

    printf("populate_ore_pocket_cuboid: ore= %s ore_units= %d clusters= %d \n", block_name, ore_count, clusters);
}

int generate_ore_pocket_cuboid(int _x, int _y, int _z, float size, CubeType tile_id)
{
    float asize = randf();
    float bsize = randf();
    float csize = randf();

    float norm = cbrt(size/2.0f)/cbrt(asize*bsize*csize);

    asize *= norm;
    bsize *= norm;
    csize *= norm;

    Vec3 f = vec3_init(1.0, 0.0, 0.0);
    Vec3 r = vec3_init(0.0, 1.0, 0.0);
    Vec3 u = vec3_init(0.0, 0.0, 1.0);

    Mat3 rot = mat3_euler_rotation(randf(), randf(), randf());

    f = vec3_apply_rotation(f, rot);
    r = vec3_apply_rotation(r, rot);
    u = vec3_apply_rotation(u, rot);


    int _set =0;
    int _skipped = 0;

    int range = 1 + (int)size;
    for (int i= -range; i<=range; i++)
    for (int j= -range; j<=range; j++)
    for (int k= -range; k<=range; k++)
    {
        if (_z+k <= 0 || _z+k >= 127)
            continue;

        float x = 0.5f + (float) (i);
        float y = 0.5f + (float) (j);
        float z = 0.5f + (float) (k);

        if (abs(x*f.x+y*f.y+z*f.z) < asize &&
            abs(x*r.x+y*r.y+z*r.z) < bsize &&
            abs(x*u.x+y*u.y+z*u.z) < csize)
        {

            int tx = (_x+i+512) % 512;
            int ty = (_y+j+512) % 512;
            int tz = (_z+k+128) % 128;

            t_map::set_fast(tx,ty,tz,tile_id);
            _set ++;

        }
        else
        {
            _skipped++;
        }

    }

    return _set;
    //printf("set= %d skipped= %d size= %f target= %f asize= %.02f bsize= %.02f csize= %.02f \n", _set,_skipped, asize*bsize*csize, size, asize,bsize,csize);
}


int generate_ore_pocket_elliptoid(int _x, int _y, int _z, float size, CubeType tile_id);

void populate_ore_pocket_elliptoid(int number, const char* block_name)
{

    CubeType tile_id = t_map::get_cube_type(block_name);
    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;

    int ore_count = 0;
    int clusters = 0;
    for (int i=0; i<number; i++)
    {
        int x = (int)genrand_int32() % map_dim.x;
        int y = (int)genrand_int32() % map_dim.y;
        int z = (int)genrand_int32() % map_dim.z;

        int ctile = t_map::get(x,y,z);
        if (ctile == 0)
            continue;

        //int ctile = t_map::get(x,y,z);
        //if (ctile == 0) continue;

        //int s = 4 + 4*(genrand_int32() % 4);

        float size = 0.0f;
        float value = randf();

        if (value <= 0.50f)
        {
            size = 1.00;
        }
        else if (value <= 1.0)
        {
            size = 1.5f;
        }
        //generate_ore_vein(x,y,z, s, tile_id);
        ore_count += generate_ore_pocket_elliptoid(x,y,z, size, tile_id);
        clusters++;
    }

    printf("populate_ore_elliptoid_pockets: ore= %s ore_units= %d clusters= %d \n", block_name, ore_count, clusters);
}

int generate_ore_pocket_elliptoid(int _x, int _y, int _z, float size, CubeType tile_id)
{
    float asize = randf();
    float bsize = randf();
    float csize = randf();

    float norm = 1.0f/sqrt(asize*asize+bsize*bsize+csize*csize);

    asize *= norm;
    bsize *= norm;
    csize *= norm;

    Vec3 f = vec3_init(1.0, 0.0, 0.0);
    Vec3 r = vec3_init(0.0, 1.0, 0.0);
    Vec3 u = vec3_init(0.0, 0.0, 1.0);

    Mat3 rot = mat3_euler_rotation(randf(), randf(), randf());

    f = vec3_apply_rotation(f, rot);
    r = vec3_apply_rotation(r, rot);
    u = vec3_apply_rotation(u, rot);


    int _set =0;
    int _skipped = 0;

    int range = 1 + (int)size;
    for (int i= -range; i<=range; i++)
    for (int j= -range; j<=range; j++)
    for (int k= -range; k<=range; k++)
    {
        if (_z+k <= 0 || _z+k >= 127)
            continue;

        float x = 0.5f + (float) (i);
        float y = 0.5f + (float) (j);
        float z = 0.5f + (float) (k);

        float dx = (x*f.x+y*f.y+z*f.z)*asize;
        float dy = (x*r.x+y*r.y+z*r.z)*bsize;
        float dz = (x*u.x+y*u.y+z*u.z)*csize;

        if (dx*dx + dy*dy + dz*dz < size*size)
        {
            int tx = (_x+i+512) % 512;
            int ty = (_y+j+512) % 512;
            int tz = (_z+k+128) % 128;

            t_map::set_fast(tx,ty,tz, tile_id);
            _set ++;

        }
        else
        {
            _skipped++;
        }

    }

    return _set;
}

}   // t_gen
