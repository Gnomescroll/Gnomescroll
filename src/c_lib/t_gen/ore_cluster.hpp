#pragma once

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

int generate_ore_vein(int x, int y, int z, int size, int tile_id);

void generate_ore_pocket(int _x, int _y, int _z, float size, int tile_id);

void populate_ore_veins(int number, const char* block_name)
{
    int tile_id = t_map::dat_get_cube_id(block_name);
    GS_ASSERT(tile_id >= 0);
    if (tile_id < 0) return;

    for(int i=0; i<number; i++)
    {
        int x = (int)genrand_int32() % map_dim.x;
        int y = (int)genrand_int32() % map_dim.y;
        int z = (int)genrand_int32() % map_dim.z;

        int ctile = t_map::get(x,y,z);
        if(ctile == 0) continue;

        int s = 4 + 4*(genrand_int32() % 4);

        generate_ore_vein(x,y,z, s, tile_id);
    }
}

void populate_ore()
{
    populate_ore_veins(4*8192, "methane_ice");
    populate_ore_veins(4*8192, "iron_ore");

    populate_ore_veins(4*2048, "coal");

    populate_ore_veins(4*4096, "copper_ore");
    populate_ore_veins(4*2048, "gallium_ore");
    populate_ore_veins(4*2048, "iridium_ore");
}

/*
    Add branching ore veins
*/

int generate_ore_vein(int x, int y, int z, int size, int tile_id)
{
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
        if (ctile == 0)
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

        t_map::set(x,y,z,tile_id);

        tries = 0; //reset
        ct++;
    }
    
    if(tries >= 20)
        printf("Warning: generate_ore_vein 20 attemps made to populate ore vein\n");

    return ct;
}


void generate_ore_pocket(int _x, int _y, int _z, float size, int tile_id)
{
    float asize = randf();
    float bsize = randf();
    float csize = randf();

    float norm = size*1.0f/cbrt(asize*bsize*csize);

    asize *= norm;
    bsize *= norm;    
    csize *= norm;

    printf("size= %f target= %f \n", asize*bsize*csize, size);

    Vec3 f = vec3_init(1.0, 0.0, 0.0);
    Vec3 r = vec3_init(0.0, 1.0, 0.0);
    Vec3 u = vec3_init(0.0, 0.0, 1.0);

    Mat3 rot = mat3_euler_rotation(randf(), randf(), randf() );

    f = vec3_apply_rotation(f, rot);
    r = vec3_apply_rotation(r, rot);
    u = vec3_apply_rotation(u, rot);

    int range = 1 + (int)size;
    for(int i= -range; i<=range; i++)
    for(int j= -range; j<=range; j++)
    for(int k= -range; k<=range; k++)
    {
        float x = 0.5f (float)(_x + i);
        float y = 0.5f (float)(_y + j);
        float x = 0.5f (float)(_z + k);

        if(abs(x*f.x+y*f.y+z*f.z) < asize
        && abs(x*r.x+y*r.y+z*r.z) < bsize
        && abs(x*u.x+y*u.y+z*u.z) < csize
            )
        {   
            int tx = (_x+i+512) % 512;
            int ty = (_y+j+512) % 512;
            int tz = (_z+k+127) % 127;

            t_map::set(tx,ty,tz,tile_id);
        }

    }
}   


}
