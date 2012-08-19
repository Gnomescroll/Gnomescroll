
#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>

namespace t_map
{

/*
    Update skylights seperately
*/

//e[ (z<<8)+((y&15)<<4)+(x&15) ] = e;
void update_skylight(int chunk_i, int chunk_j)
{
    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    //struct MAP_e e;

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int k=127;

        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)    //iterate until we hit top block
                break;

            e.light  |= 0xff; //upper bits for skylight
            mc->set_element(i,j,k,e);
        }
        if (k < 0) return;

        // render gradient down from top block
        for (int _k=0; k>=0 && _k<16; k--, _k++)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light  |= 16*(15-_k)+ 0x0f; //clear upper bits
            mc->set_element(i,j,k,e);
        }

        // black out everything below
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light  |= 0x0f; //clear upper bits
            mc->set_element(i,j,k,e);
        }
    }

}

int get_light(int x, int y, int z)
{
    class MAP_CHUNK* mc = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 0;

    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;


    return c->e[ (z<<8)+((y&15)<<4)+(x&15) ].light / 16;
}

int set_light(int x, int y, int z, int value)
{
    class MAP_CHUNK* mc = main_map->chunk[ MAP_CHUNK_XDIM*(y >> 4) + (x >> 4) ];

    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;


    c->e[ (z<<8)+((y&15)<<4)+(x&15) ].light |= value*16;
}


void skylight_update(int x, int y, int z)
{
    int li = get_light_value(x,y,z);

    GS_ASSERT(! isSolid(x,y,z));
    if(li-1 == 0) return;

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


    _x = x;
    _y = y;
    _z = (z+1) % 128;
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


    _x = x
    _y = y;
    _z = (z+127)%128; //z -1
    //x
    if(!isSolid(_x,_y,_z) && get_light_value(_x,_y,_z) < li+1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


}


void update_skylight2(int chunk_i, int chunk_j)
{

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];

    mc->refresh_height_cache();

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {


    }

}

}   // t_map
