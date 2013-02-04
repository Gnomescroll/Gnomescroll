#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>

namespace t_map
{

struct LightUpdateElement
{
    int x,y,z;
};

static const int va[3*6] =
{
    1,0,0,
    -1,0,0,
    0,1,0,
    0,-1,0,
    0,0,1,
    0,0,-1
};

/*
    Skylight 
*/


int get_skylight(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 15;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 0;  //so it does not try to update

    return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light & 0x0f;  //bottom half
}

void set_skylight(int x, int y, int z, int value)
{
    GS_ASSERT(z >= 0 && z <= 128);
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);
    GS_ASSERT(value < 16 && value >= 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    GS_ASSERT(x >= 0 && x < 512)
    GS_ASSERT(y >= 0 && y < 512)

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
    {
        GS_ASSERT(false);
        return;
    }

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
    light = (light & 0xf0) | (value & 0x0f);
    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;


    #if DC_CLIENT
    main_map->set_update(x,y);
    #endif
}

/*
    Skylight Array
*/

struct LightUpdateElement* sky_light_array = NULL;
int sky_light_array_max      = 8*1024;
int sky_light_array_index    = 0;
int sky_light_array_n        = 0;

void _push_skylight_update(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    //skip update of blocks in null chunks
    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;


    //skip solid blocks
    struct MAP_ELEMENT e = get_element(x, y, z);
    if(fast_cube_properties[e.block].solid == true)
    {
        GS_ASSERT(false);
        return;
    }

    if(sky_light_array_index == sky_light_array_max)
    {
        sky_light_array_max *= 2;
        sky_light_array = (struct LightUpdateElement*) realloc(sky_light_array, sky_light_array_max* sizeof(struct LightUpdateElement));
        printf("_push_sky_light_update: reallocing light array to: %d \n", sky_light_array_max);
    }

    sky_light_array[sky_light_array_index].x = x;
    sky_light_array[sky_light_array_index].y = y;
    sky_light_array[sky_light_array_index].z = z;

    sky_light_array_index++;
}

void _skylight_update_core(int max_iterations);

void _skylight_update_core()
{
    _skylight_update_core(1000); //do 1000 iteratations maxs
    _skylight_update_core(1000); //do 1000 iteratations maxs
}

void _skylight_update_core(int max_iterations)
{
    if(sky_light_array_index == 0)
        return;

    int index = sky_light_array_n;

    int stop_index = index + max_iterations;
    if(stop_index > sky_light_array_index)
        stop_index = sky_light_array_index;
    if(max_iterations == 0)
        stop_index = sky_light_array_index;

    while(index != stop_index)
    {
        int x = sky_light_array[index].x;
        int y = sky_light_array[index].y;
        int z = sky_light_array[index].z;

        class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
        if(mc == NULL)
        {
            index++;
            continue;
        }

        struct MAP_ELEMENT e = get_element(x,y,z);
        int li = (e.light & 0x0f);
        GS_ASSERT(li == get_skylight(x,y,z));

        if(fast_cube_properties[e.block].solid == true)
        {
            GS_ASSERT(false);
            index++;
            continue;
        }



        //if block is sunlight and block above is solid, then set to not-sunlight
        struct MAP_ELEMENT te = get_element(x,y,z+1);
        //struct MAP_ELEMENT be = get_element(x,y,z-1);

        if(z == 127)  GS_ASSERT((te.light & 0x0f) == 15);
        //top level must be sunlight
        //if block is not sunlight and sunlight is above block, set sunlight


        struct MAP_ELEMENT ea[6];

        for(int i=0; i<6; i++)
            ea[i] = get_element(x+va[3*i+0], y+va[3*i+1], z+va[3*i+2]);


        if( (te.light & 0x0f) == 15 && li != 15)
        {
            li = 15;
            set_skylight(x,y,z, li);
            
            //if(isSolid(x,y,z-1) == false)
            //    _push_skylight_update(x,y,z-1);

            for(int j=0; j<6; j++)
            {
                if( (ea[j].light & 0x0f) < 14 && fast_cube_properties[ea[j].block].solid == false)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }

            _push_skylight_update(x,y,z);

            index++;
            continue;
        }


        if(li == 15 && fast_cube_properties[te.block].solid == true)
        {
            li = 14; //maybe zero?
            set_skylight(x,y,z, li);

            if(isSolid(x,y,z-1) == false)
                _push_skylight_update(x,y,z-1);
            _push_skylight_update(x,y,z);

            index++;
            continue;
        }



        /*
            Normal Light
        */

        int _min = 16;
        int _max = 0;

        for(int i=0; i<6; i++)
        {
            if(fast_cube_properties[ea[i].block].solid == false)
            {
                int _li = (ea[i].light & 0x0f);
                if( _li < _min) _min = _li;
                if( _li > _max) _max = _li;
            }
        }

        //update around


/*
        if(li == 15 )
        {
            for(int j=0; j<6; j++)
            {
                if( (ea[j].light & 0x0f) < 14 && fast_cube_properties[ea[j].block].solid == false)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }

            index++;
            continue;
        }
*/
        //proprogate light below level 15
    /*
        if(li != 15 && li != _max -1 && !(_max == 0 && li == 0))
        {
            //printf("min/max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);
            li = _max - 1;
            //if(li < 0) li = 0;
            GS_ASSERT(_max - 1 >= 0);

            set_skylight(x,y,z, li);

            for(int j=0; j<6; j++)
            {
                int _li =  ea[j].light & 0x0f;
                if(_li != 15 && fast_cube_properties[ea[j].block].solid == false)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }
            _push_skylight_update(x,y,z);

            index++;
            continue;
        }
    */

        if(li != 15 && li > _max -1 && li > 0)
        {
            //printf("sky_min: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);
            
            li = li - 1;

            //GS_ASSERT(_max == 0 || li == _max-1); //not always true

            set_skylight(x,y,z, li);
            for(int j=0; j<6; j++)
            {
                if( fast_cube_properties[ea[j].block].solid == false)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }
            _push_skylight_update(x,y,z);
            index++;
            continue;
        }  


        if(li != 15 && _max > li +1)
        {
            //printf("sky_max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

            li = _max - 1;
            //GS_ASSERT(_max - 1 >= 0);

            set_skylight(x,y,z, li);

            for(int j=0; j<6; j++)
            {
                //if( (ea[j].light >> 4) +1 < li && fast_cube_properties[ea[j].block].solid == false)
                if(fast_cube_properties[ea[j].block].solid == false)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }
            _push_skylight_update(x,y,z);

            index++;
            continue;
        }


        index++;
        continue;
    }

    sky_light_array_n = index;

    if(max_iterations == 0)
    printf("sunlight_update: array_clean: itr= %d max_iterators= %d index= %d \n", sky_light_array_index, max_iterations, index);

    if(index == sky_light_array_index)
    {
        sky_light_array_n = 0;
        sky_light_array_index = 0;
    }
}


#if 0
void update_skylight(int chunk_i, int chunk_j)
{
    //return;

    GS_ASSERT(chunk_i >= 0 && chunk_i < 32);
    GS_ASSERT(chunk_j >= 0 && chunk_j < 32);

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    GS_ASSERT(mc != NULL);

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {

        struct MAP_ELEMENT e;

        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;

        GS_ASSERT(x >= 0 && x < 512)
        GS_ASSERT(y >= 0 && y < 512)

        int k = map_dim.z-1;

        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if(e.block != 0)    //iterate until we hit top block
                break;
            set_skylight(x,y,k, 15);
        }
        if (k < 0) return;
        // black out everything below
    
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if(e.block != 0)
                continue;
            set_skylight(x,y,k, 0);
        }
    
    }

}


//proprogate out
void update_skylight_out(int x, int y, int z)
{
    int li = get_skylight(x,y,z);

    //GS_ASSERT(! isSolid(x,y,z));
    if(li-1 <= 0) return;

/*
    !isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 ) {
        set_skylight(_x,_y,_z, li-1);
    These can be combined into a single function
*/

/*
    Recursion can be replaced by poping onto a circular buffer
*/

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )   //do a single get block for this!!
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //GS_ASSERT( ((x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2) == (x+512-1) % 512);

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = y;
    _z = (z-1+map_dim.z)%map_dim.z; //z -1
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

}

//call this when removing a block
void update_skylight_in(int x, int y, int z)
{
    //int li = get_skylight(x,y,z);

    if(isSolid(x,y,z))
    {
        GS_ASSERT(false);
        return;
    }
    //technically, if top block of height map
    if( get_skylight(x,y,z+1) == 15 )
    {
        GS_ASSERT( z+1 == main_map->get_height(x,y) );

        /*
            BUG:
            Fails on removing block capping a column?
        */

        int _z = z;
        //assume first element is not solid
        while(1)
        {
            set_skylight(x,y,_z,15);
            if(isSolid(x,y,_z-1) || _z==0)
                break;
            _z--;
        }

        for(int tz=_z; tz <= z; tz++)
            update_skylight_out(x,y,tz);

        return;
    }
    int li = 0;

    int _x,_y,_z, tli;

    //x
    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //z
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = y;
    _z = (z+(map_dim.z-1))%map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;


    if(li != 0)
    {
        set_skylight(x,y,z, li);
        update_skylight_out(x,y,z);
    }
}

void update_skylight_boundary(int _ci, int _cj)
{
    class MAP_CHUNK* mc;
    int ci, cj;

    //north?
    ci = (_ci + 1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 0;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //south?
    ci = (_ci + -1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 15;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

    //west?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + 1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 0;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //east?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + -1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 15;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

}

void update_skylight2(int ci, int cj)
{
    update_skylight_boundary(ci, cj);

    class MAP_CHUNK* mc = main_map->chunk[32*cj + ci];

    mc->refresh_height_cache();

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    for(int k=0; k<map_dim.z; k++)
    {
        if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
            continue;

        update_skylight_out(16*ci+i,16*cj+j,k);

    }

}
#endif

/*
    Env Light
*/


//for comparision purpose
int get_envlight(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 0;  //so it does not try to update

    return (mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light >> 4);  //upper half
}

void set_envlight(int x, int y, int z, int value)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

//#if DC_CLIENT
    //printf("set_envlight: %d %d %d, value= %d \n", x,y,z,value);
//#endif

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;

    GS_ASSERT(value < 16 && value >= 0);

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
    light = (light & 0x0f) | (value << 4); // clear upper nib, set upper nib
    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;

#if DC_CLIENT
    main_map->set_update(x,y);
#endif
}

/*
    Lighting variables
*/
struct LightUpdateElement* env_light_array = NULL;
int env_light_array_max      = 8*1024;
int env_light_array_index    = 0;
int env_light_array_n        = 0;

void _push_envlight_update(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    //skip update of blocks in null chunks
    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
    {
        GS_ASSERT(false);
        return;
    }

    struct MAP_ELEMENT e = get_element(x, y, z);
    if(fast_cube_properties[e.block].solid == true)
    {
        GS_ASSERT(false);
        return;
    }

    if(env_light_array_index == env_light_array_max)
    {
        env_light_array_max *= 2;
        env_light_array = (struct LightUpdateElement*) realloc(env_light_array, env_light_array_max* sizeof(struct LightUpdateElement));
        printf("_push_envlight_update: reallocing light array to: %d \n", env_light_array_max);
    }

    env_light_array[env_light_array_index].x = x;
    env_light_array[env_light_array_index].y = y;
    env_light_array[env_light_array_index].z = z;

    env_light_array_index++;
}

void _envlight_update_core(int max_iterations);

void _envlight_update_core()
{
    _envlight_update_core(1000); //do 1000 iteratations maxs
    _envlight_update_core(1000); //do 1000 iteratations maxs
}

void _envlight_update_core(int max_iterations)
{
    if(env_light_array_index == 0)
        return;

    int index = env_light_array_n;

    int stop_index = index + max_iterations;
    if(stop_index > env_light_array_index)
        stop_index = env_light_array_index;
    if(max_iterations == 0)
        stop_index = env_light_array_index;

    while(index != stop_index)
    //while(index != env_light_array_index)
    {
        int x = env_light_array[index].x;
        int y = env_light_array[index].y;
        int z = env_light_array[index].z;

        class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
        if(mc == NULL)
        {
            index++;
            continue;
        }

        struct MAP_ELEMENT e = get_element(x,y,z);

        int li = (e.light >> 4);
        //GS_ASSERT(li == get_envlight(x,y,z));

        if(fast_cube_properties[e.block].light_source == true)
        {
            //this code path is deprecated
            GS_ASSERT(li == fast_cube_attributes[e.block].light_value);

            for(int i=0; i<6; i++)
            {
                struct MAP_ELEMENT _e = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                if( (_e.light >> 4) < li -1 && fast_cube_properties[_e.block].solid == false)
                    _push_envlight_update(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
            }
            index++;
            continue;
        }
        else
        {
    /*
        ASSERT!!!
        - determine invariants and stationary conditions
    */
            //solid non-light source

            if(fast_cube_properties[e.block].solid == true)
            {
                GS_ASSERT(false);
                if(index != 0)
                    printf("ERROR: env_light called on solid block on index %d of %d \n", index, env_light_array_index);
                index++;
                continue;
            }

            //non-solid non-light source
            // 1> check to make sure light value is correct; not too high, or too low
            // 2> If light value in adjacent block is

            struct MAP_ELEMENT ea[6];

            int _min = 16;
            int _max = 0;

            for(int i=0; i<6; i++)
            {
                ea[i] = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                if(fast_cube_properties[ea[i].block].solid == false || fast_cube_properties[ea[i].block].light_source == true)
                {
                    int _li = (ea[i].light >> 4);
                    if( _li < _min) _min = _li;
                    if( _li > _max) _max = _li;
                }
            }

            //if(_max != 0 || _min != 0 || li != 0)
            //    printf("test: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

            /*
                If there is no light to proprogate inwards, check if light value is too high
                - the light value of current cube can be maximum of one less than the brightest adjacent cube

                min=0, max=2, li=2 //value is too high, set to max-1

                set block to 0 at 298, 399, 58
                test: x,y,z= 298 399 58 max= 14 min= 14 li= 0 
                max: x,y,z= 298 399 58 max= 14 min= 14 li= 0 
                set_envlight: 298 399 58, value= 13 
                test: x,y,z= 298 399 58 max= 14 min= 14 li= 13 

            */
            //if(li >= _max && li > 0)

            //min: x,y,z= 413 152 61 max= 10 min= 6 li= 9  //this is error!

            //if(li >= _max -1 && li > 0)

        /*
            if(li != _max -1 && !(_max == 0 && li ==0) )
            {
                //printf("min/max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);
                li = _max - 1;
                //if(li < 0) li = 0;
                GS_ASSERT(_max - 1 >= 0);

                set_envlight(x,y,z, li);

                for(int j=0; j<6; j++)
                {
                    if(fast_cube_properties[ea[j].block].solid == false)
                        _push_envlight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                }
                _push_envlight_update(x,y,z);

                index++;
                continue;
            }
        */        
        #if 1
            if(li > _max -1 && li > 0)
            {
                //printf("env_min: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);
                
                li = li - 1;

                //GS_ASSERT(_max == 0 || li == _max-1); //not always true

                set_envlight(x,y,z, li);
                for(int j=0; j<6; j++)
                {
                    if( fast_cube_properties[ea[j].block].solid == false)
                        _push_envlight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                }
                _push_envlight_update(x,y,z);
                index++;
                continue;
            }  


            if(_max > li +1)
            {
                //printf("env_max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

                li = _max - 1;
                //GS_ASSERT(_max - 1 >= 0);

                set_envlight(x,y,z, li);

                for(int j=0; j<6; j++)
                {
                    //if( (ea[j].light >> 4) +1 < li && fast_cube_properties[ea[j].block].solid == false)
                    if(fast_cube_properties[ea[j].block].solid == false)
                        _push_envlight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                }
                _push_envlight_update(x,y,z);

                index++;
                continue;
            }
        #endif
        }

            
        index++;
        continue;
    }

    env_light_array_n = index;

    if(index == env_light_array_index)
    {
        env_light_array_n = 0;
        env_light_array_index = 0;
    }
}

/*
    Each block should only set its own values
*/

void _push_envlight_update(int x, int y, int z);

//handle block addition
void light_add_block(int x, int y, int z)
{

    struct MAP_ELEMENT e = get_element(x,y,z);

    if( (e.light >> 4) != fast_cube_attributes[e.block].light_value )
    {
        //GS_ASSERT(false) //should not happen!
        set_envlight(x,y,z, fast_cube_attributes[e.block].light_value);
    }

    //placed solid block
    if(fast_cube_properties[e.block].solid == true)
    {
        for(int j=0; j<6; j++)
        {
            struct MAP_ELEMENT e = get_element(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
            if( fast_cube_properties[e.block].solid == false )
            {
                _push_envlight_update(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
                _push_skylight_update(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
            }
        }
    }
    else
    {
        _push_envlight_update(x,y,z);
        _push_skylight_update(x,y,z);
    }
}

//call on chunk init
void init_update_envlight(int chunk_i, int chunk_j)
{
    //return;
    //printf("init_update_envlight: %d %d \n", chunk_i, chunk_j);

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];

    /*
        Envlight
    */
    for(int k=0; k<128; k++)
    {
        for(int i=0; i<16; i++)
        for(int j=0; j<16; j++)
        {
            int x = 16*chunk_i + i;
            int y = 16*chunk_j + j;

            struct MAP_ELEMENT e = mc->get_element(i,j,k);

            if(fast_cube_properties[e.block].light_source == true)
            {
                set_envlight(x,y,k, fast_cube_attributes[e.block].light_value);
            }
            else
            {
                if(fast_cube_properties[e.block].solid == true)
                {
                    GS_ASSERT(get_envlight(x,y,k) == 0);
                }
                else
                {
                    //set_envlight(x,y,k, 0);
                }
            }
        }

        if(k % 8 == 7 )
            _envlight_update_core();
    }


}

void init_update_sunlight(int chunk_i, int chunk_j)
{
    //class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;
        int k = map_dim.z-1;

        for (; k>=0; k--)
            set_skylight(x,y,k, 0);

    }

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;
        int k = map_dim.z-1;

        _push_skylight_update(x,y,k);
    /*
        // get highest block
        for (; k>=0; k--)
        {
            struct MAP_ELEMENT e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if(e.block != 0)    //iterate until we hit top block
                break;
            set_skylight(x,y,k, 15);
            _push_skylight_update(x,y,k);
        }

        for (; k>=0; k--)
        {
            struct MAP_ELEMENT e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if(fast_cube_properties[e.block].solid == true)    //iterate until we hit top block
                continue;
            set_skylight(x,y,k, 0);
        }
    */
    }


    _skylight_update_core(0);

/*
    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int x = 16*chunk_i + i;
        int y = 16*chunk_j + j;
        int z = 127;

        struct MAP_ELEMENT e = mc->get_element(i,j,z);
        if(fast_cube_properties[e.block].solid != true)
        {
            _push_skylight_update(x,y,z);
        }
    }

    _skylight_update_core();
*/
}

void init_lighting()
{
    env_light_array = (struct LightUpdateElement*) malloc(env_light_array_max* sizeof(struct LightUpdateElement));
    sky_light_array = (struct LightUpdateElement*) malloc(sky_light_array_max* sizeof(struct LightUpdateElement));
}

void teardown_lighting()
{
    if(env_light_array != NULL)
        free(env_light_array);
    if(sky_light_array != NULL)
        free(sky_light_array);
}

}   // t_map
