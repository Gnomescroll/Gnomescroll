
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

            e.light  = 15; //upper bits for skylight
            mc->set_element(i,j,k,e);
        }
        if (k < 0) return;

        // render gradient down from top block
        
/*
        for (int _k=0; k>=0 && _k<16; k--, _k++)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light  |= 16*(15-_k)+ 0x0f; //clear upper bits
            mc->set_element(i,j,k,e);
        }
*/
        // black out everything below
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light = 0; //clear upper bits
            mc->set_element(i,j,k,e);
        }
    }

}

//for comparision purpose
int get_light(int x, int y, int z)
{

    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 16;  //so it does not try to update

    return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
}

void set_light(int x, int y, int z, int value)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;
    GS_ASSERT(mc != NULL);
    GS_ASSERT( (y >> 4) < 32);
    GS_ASSERT( (x >> 4) < 32);
 
    //printf("%i\n", (z<<8)+((y&15)<<4)+(x&15) );

    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = value;
}


void skylight_update(int x, int y, int z)
{
    int li = get_light(x,y,z);

    GS_ASSERT(! isSolid(x,y,z));
    if(li-1 <= 0) return;

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )   //do a single get block for this!!
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


    _x = x;
    _y = y;
    _z = (z+1) % 128;
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


    _x = x;
    _y = y;
    _z = (z+127)%128; //z -1
    //x
    if(!isSolid(_x,_y,_z) && get_light(_x,_y,_z) < li-1 )
    {
        set_light(_x,_y,_z, li-1);
        skylight_update(_x,_y,_z);
    }


}


void update_skylight2(int ci, int cj)
{

    class MAP_CHUNK* mc = main_map->chunk[32*cj + ci];

    mc->refresh_height_cache();

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    for(int k=0; k<128; k++)
    {
        if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_light(16*ci+i,16*cj+j,k) != 15) // || get_light(i,j,k) < 16)
            continue;

        skylight_update(16*ci+i,16*cj+j,k);

    }

}

}   // t_map
