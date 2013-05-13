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

//int* skylight_array_debug;
//int* envlight_array_debug;

int get_skylight(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 15;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
        return 0;  //so it does not try to update

    return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light & 0x0f;  //bottom half
}

int get_envlight(int x, int y, int z);
int _skylight_update_condition(int x, int y, int z);

void set_skylight(int x, int y, int z, int value)
{
    //GS_ASSERT(z >= 0 && z <= 128);
    //GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    //int env_light =  get_envlight(x,y,z);

    //GS_ASSERT(x >= 0 && x < 512)
    //GS_ASSERT(y >= 0 && y < 512)

    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];

    if(mc == NULL)
    {
    #if !PRODUCTION
        GS_ASSERT(false);
    #endif
        return;
    }

    MapElement e = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ];

    if((e.light & 0x0f) == value)
        return;

#if !PRODUCTION
    GS_ASSERT(value < 16 && value >= 0);
    GS_ASSERT(!fast_cube_properties[e.block].solid);
    //GS_ASSERT((e.light & 0x0f) != value);
#endif

#if 0
    int condition = _skylight_update_condition(x,y,z);
    int pvalue = (e.light & 0x0f);
    int cvalue = value;
    printf("set_skylight: %d %d %d, pvalue= %d cvalue= %d, condition= %d \n", x,y,z, pvalue,cvalue, condition);
#endif

#if 0
    if(skylight_array_debug[128*512*z + 512*y + x]  != -1)
    {
        int pvalue = skylight_array_debug[128*512*z + 512*y + x];
        int cvalue = (e.light & 0x0f);
        GS_ASSERT(pvalue == cvalue)

        //printf("skylight error: %d %d %d, should be %d, is %d \n", x,y,z, pvalue, cvalue);
    }
#endif

    e.light = (e.light & 0xf0) | (value & 0x0f);
    mc->e[(z<<8)+((y&15)<<4)+(x&15)] = e;

#if !PRODUCTION
    GS_ASSERT((e.light & 0x0f) == value);
#endif

    //skylight_array_debug[128*512*z + 512*y + x] = value;


    #if DC_CLIENT
    main_map->set_update(x,y);
    #endif

    //GS_ASSERT(env_light == get_envlight(x,y,z));
}

/*
    Skylight Array
*/

const int LightElementArrayMax = 64*1024;
struct LightElementArray
{
    int num;
    struct LightUpdateElement arr[LightElementArrayMax];
    struct LightElementArray* next;
};

struct LightElementArray* LEA;

struct LightUpdateElement* sky_light_array = NULL;
const int sky_light_array_max      = 16*1024*1024;
//int sky_light_array_index    = 0;
//int sky_light_array_n        = 0;

int sky_light_array_start    = 0;
int sky_light_array_num      = 0;

/*
    Note: lighting is slow, and should use circular array
*/

int _skylight_update_condition(int x, int y, int z);

#define LIGHT_CONDITION_COUNTER 0

#if LIGHT_CONDITION_COUNTER
    static int condition_arr[8];
#endif

OPTIMIZED
void _push_skylight_update(int x, int y, int z)
{

    if(sky_light_array_num == sky_light_array_max)
        return;

    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    //skip update of blocks in null chunks
    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
        return;

#if LIGHT_CONDITION_COUNTER
    int condition = _skylight_update_condition(x,y,z);

    condition_arr[condition]++;

    static int count = 0;
    count++;

    if(count % (1024*32) == 0)
    {
        for(int i=0; i<8; i++)
            printf("condition %d: count= %d \n", i, condition_arr[i]);
    }
#endif
    //if(condition == 0)
    //    return;

    //skip solid blocks
    struct MapElement e = get_element(x, y, z);
    IF_ASSERT(fast_cube_properties[e.block].solid)
        return;

    int index = (sky_light_array_start+sky_light_array_num) % sky_light_array_max;
    sky_light_array[index].x = x;
    sky_light_array[index].y = y;
    sky_light_array[index].z = z;

    sky_light_array_num++;
}

void _skylight_update_core(int max_iterations);

void _skylight_update_core()
{
    _skylight_update_core(6000); //do 1000 iteratations maxs
}

//sky light
#define _SL(x) (x & 0x0f)

OPTIMIZED
int _skylight_update_condition(int x, int y, int z)
{

    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
        return 0;

    struct MapElement e = get_element(x,y,z);
    int li = (e.light & 0x0f);

    if(fast_cube_properties[e.block].solid)
        return 0;

    struct MapElement ea[6];

    for (int i=0; i<6; i++)
        ea[i] = get_element(x+va[3*i+0], y+va[3*i+1], z+va[3*i+2]);

    struct MapElement te = get_element(x,y,z+1);

    if ((te.light & 0x0f) == 15 && li != 15)
    {
        return 1; //light above block is sunlight and block is not sunlight
    }

    if (li == 15 && fast_cube_properties[te.block].solid)
    {
        return 2; //block above is solid and this block is sunlight, therefore this block cannot be sunlight anymore
    }

    if(li == 15 && (te.light & 0x0f) != 15)
    {
        return 3; //this block is sunlight, but the block above it is not
    }

    /*
        Normal Light
    */

    int _min = 16;
    int _max = 0;

    for (int i=0; i<6; i++)
    {
        if (!fast_cube_properties[ea[i].block].solid)
        {
            int _li = (ea[i].light & 0x0f);
            if (_li < _min) _min = _li;
            if (_li > _max) _max = _li;
        }
    }

    if (li != 0 && li != 15 && li+1 > _max)
    {
        return 4; //is brigther than brighest adjacant block
        //li = li - 1;
        //set_skylight(x,y,z, li);
    }


    if (li != 15 && _max > li +1)  //its always condition 5
    {
        //printf("li= %d max= %d \n", li, _max);
        return 5;
        //GS_ASSERT(li != _max -1);
        //li = _max - 1;
        //set_skylight(x,y,z, li);
    }

    return 0;
}

OPTIMIZED
void _skylight_update_core(int max_iterations)
{
    if (sky_light_array_num == 0)
        return;

    if (max_iterations == 0)
        max_iterations = 1024;

    int itr_count = 0;

    while (itr_count < max_iterations && sky_light_array_num > 0)
    {
        itr_count++; //loop counter

        //int index = sky_light_array_start;

        int x = sky_light_array[sky_light_array_start].x;
        int y = sky_light_array[sky_light_array_start].y;
        int z = sky_light_array[sky_light_array_start].z;

        sky_light_array_start = (sky_light_array_start+1) % sky_light_array_max;
        sky_light_array_num--;

        //int condition = _skylight_update_condition(x,y,z);

        class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
        if (mc == NULL)
        {
            continue;
        }

        struct MapElement e = get_element(x,y,z);
        int li = (e.light & 0x0f);
        //GS_ASSERT(li == get_skylight(x,y,z));

        if(fast_cube_properties[e.block].solid)
        {
            //GS_ASSERT(false);
            continue;
        }

        //if block is sunlight and block above is solid, then set to not-sunlight

        //struct MapElement be = get_element(x,y,z-1);

        //if (z == 127)  GS_ASSERT((te.light & 0x0f) == 15);
        //top level must be sunlight
        //if block is not sunlight and sunlight is above block, set sunlight

        struct MapElement te = get_element(x,y,z+1);
        struct MapElement ea[6];

        for (int i=0; i<6; i++)
            ea[i] = get_element(x+va[3*i+0], y+va[3*i+1], z+va[3*i+2]);

        if ((te.light & 0x0f) == 15 && li != 15)
        {
            li = 15;
            set_skylight(x,y,z, li);

            //if (!isSolid(x,y,z-1))
            //    _push_skylight_update(x,y,z-1);

            GS_ASSERT(!fast_cube_properties[te.block].solid); //top block is light value 15

            for (int j=0; j<6; j++)
            {
                if ((ea[j].light & 0x0f) < 14 && !fast_cube_properties[ea[j].block].solid)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }

            _push_skylight_update(x,y,z);

            continue;
        }

        //if li=15 and block on top is solid, set to 14
        //if li=15 and block on top is not 15, set to 14
        if (li == 15 && fast_cube_properties[te.block].solid)
        {
            li = 14; //maybe zero?
            set_skylight(x,y,z, li);

            if (!isSolid(x,y,z-1))
                _push_skylight_update(x,y,z-1);
            _push_skylight_update(x,y,z);

            continue;
        }

        /*
            Normal Light
        */

        int _min = 16;
        int _max = 0;

        for (int i=0; i<6; i++)
        {
            if (!fast_cube_properties[ea[i].block].solid)
            {
                int _li = (ea[i].light & 0x0f);
                if (_li < _min) _min = _li;
                if (_li > _max) _max = _li;
            }
        }

        if (li != 15 && li > _max -1 && li > 0)
        {
            //printf("sky_min: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

            li = li - 1;

            //GS_ASSERT(_max == 0 || li == _max-1); //not always true

            set_skylight(x,y,z, li);
            for (int j=0; j<6; j++)
            {
                if (!fast_cube_properties[ea[j].block].solid)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }
            _push_skylight_update(x,y,z);
            continue;
        }


        if (li != 15 && _max > li +1)
        {
            //printf("sky_max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

            GS_ASSERT(li != _max -1);
            li = _max - 1;
            //GS_ASSERT(_max - 1 >= 0);

            set_skylight(x,y,z, li);

            for (int j=0; j<6; j++)
            {
                //if ((ea[j].light >> 4) +1 < li && !fast_cube_properties[ea[j].block].solid)
                if (!fast_cube_properties[ea[j].block].solid)
                    _push_skylight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
            }
            _push_skylight_update(x,y,z);
            continue;
        }
        continue;
    }

    //reset
    if (sky_light_array_num == 0)
    {
        //printf("sunlight array cleaned: \n");
        sky_light_array_start = 0;
    }
}

/*
    Env Light
*/


//for comparision purpose
int get_envlight(int x, int y, int z)
{
    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
        return 0;  //so it does not try to update

    return (mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light >> 4);  //upper half
}

void set_envlight(int x, int y, int z, int value)
{


    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0) return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;


    //int sky_light =  get_skylight(x,y,z);

//#if DC_CLIENT
    //printf("set_envlight: %d %d %d, value= %d \n", x,y,z,value);
//#endif

    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
        return;


    MapElement e = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ];

#if !PRODUCTION
    GS_ASSERT(value < 16 && value >= 0);
    GS_ASSERT(!fast_cube_properties[e.block].solid);
#endif

/*
    if(e.block != 0)
    {
        printf("setting block type !0 env lighting to: %d \n", value);
    }
*/

/*
    if(envlight_array_debug[128*512*z + 512*y + x]  != -1)
    {
        int pvalue = envlight_array_debug[128*512*z + 512*y + x];
        int cvalue = (e.light >> 4);
        GS_ASSERT(pvalue == cvalue)

        //printf("skylight error: %d %d %d, should be %d, is %d \n", x,y,z, pvalue, cvalue);
    }
*/

    //int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
    e.light = (e.light & 0x0f) | (value << 4); // clear upper nib, set upper nib
    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ] = e;

#if DC_CLIENT
    main_map->set_update(x,y);
#endif

    //GS_ASSERT(sky_light == get_skylight(x,y,z));
    //envlight_array_debug[128*512*z + 512*y + x] = value;

}

/*
    Lighting variables
*/
struct LightUpdateElement* env_light_array = NULL;
int env_light_array_max      = 1024*1024;
int env_light_array_index    = 0;
int env_light_array_n        = 0;

void _push_envlight_update(int x, int y, int z)
{

    //DEBUGGING
    //return;

    if ((z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return;
    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    //skip update of blocks in null chunks
    class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if (mc == NULL)
    {
        //GS_ASSERT(false);
        return;
    }

    struct MapElement e = get_element(x, y, z);
    IF_ASSERT(fast_cube_properties[e.block].solid)
        return;

    if (env_light_array_index == env_light_array_max)
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

OPTIMIZED
void _envlight_update_core(int max_iterations)
{
    if (env_light_array_index == 0)
        return;

    int index = env_light_array_n;

    int stop_index = index + max_iterations;
    if (stop_index > env_light_array_index)
        stop_index = env_light_array_index;
    if (max_iterations == 0)
        stop_index = env_light_array_index;

    while (index != stop_index)
    //while (index != env_light_array_index)
    {
        int x = env_light_array[index].x;
        int y = env_light_array[index].y;
        int z = env_light_array[index].z;

        class MapChunk* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
        if (mc == NULL)
        {
            index++;
            continue;
        }

        struct MapElement e = get_element(x,y,z);

        int li = (e.light >> 4);
        //GS_ASSERT(li == get_envlight(x,y,z));

        if (fast_cube_properties[e.block].light_source)
        {
            //this code path is deprecated
            GS_ASSERT(li == fast_cube_attributes[e.block].light_value);

            for (int i=0; i<6; i++)
            {
                struct MapElement _e = get_element(x+va[3*i+0], y+va[3*i+1], z+va[3*i+2]);
                if ((_e.light >> 4) < li -1 && !fast_cube_properties[_e.block].solid)
                    _push_envlight_update(x+va[3*i+0], y+va[3*i+1], z+va[3*i+2]);
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

            if (fast_cube_properties[e.block].solid)
            {
                //GS_ASSERT(false);
                //if (index != 0)
                    //printf("ERROR: env_light called on solid block on index %d of %d \n", index, env_light_array_index);
                index++;
                continue;
            }

            //non-solid non-light source
            // 1> check to make sure light value is correct; not too high, or too low
            // 2> If light value in adjacent block is

            struct MapElement ea[6];

            int _min = 16;
            int _max = 0;

            for (int i=0; i<6; i++)
            {
                ea[i] = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                if (!fast_cube_properties[ea[i].block].solid || fast_cube_properties[ea[i].block].light_source)
                {
                    int _li = (ea[i].light >> 4);
                    if (_li < _min) _min = _li;
                    if (_li > _max) _max = _li;
                }
            }

            //if (_max != 0 || _min != 0 || li != 0)
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
            //if (li >= _max && li > 0)

            //min: x,y,z= 413 152 61 max= 10 min= 6 li= 9  //this is error!

            //if (li >= _max -1 && li > 0)

        #if 0
            if (li != _max -1 && !(_max == 0 && li ==0))
            {
                //printf("min/max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);
                li = _max - 1;
                //if (li < 0) li = 0;
                GS_ASSERT(_max - 1 >= 0);

                set_envlight(x,y,z, li);

                for (int j=0; j<6; j++)
                {
                    if (!fast_cube_properties[ea[j].block].solid)
                        _push_envlight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                }
                _push_envlight_update(x,y,z);

                index++;
                continue;
            }
        #else
            if (li > _max -1 && li > 0)
            {
                //printf("env_min: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

                li = li - 1;

                //GS_ASSERT(_max == 0 || li == _max-1); //not always true

                set_envlight(x,y,z, li);
                for (int j=0; j<6; j++)
                {
                    if (!fast_cube_properties[ea[j].block].solid)
                        _push_envlight_update(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                }
                _push_envlight_update(x,y,z);
                index++;
                continue;
            }


            if (_max > li +1)
            {
                //printf("env_max: x,y,z= %d %d %d max= %d min= %d li= %d \n", x,y,z, _max, _min, li);

                li = _max - 1;
                //GS_ASSERT(_max - 1 >= 0);

                set_envlight(x,y,z, li);

                for (int j=0; j<6; j++)
                {
                    //if ((ea[j].light >> 4) +1 < li && !fast_cube_properties[ea[j].block].solid)
                    if (!fast_cube_properties[ea[j].block].solid)
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

    if (index == env_light_array_index)
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
    struct MapElement e = get_element(x,y,z);

    if ((e.light >> 4) != fast_cube_attributes[e.block].light_value)
    {
        //GS_ASSERT(false) //should not happen!
        set_envlight(x,y,z, fast_cube_attributes[e.block].light_value);
    }
    GS_ASSERT(get_skylight(x,y,z) == 0);
    GS_ASSERT((e.light & 0x0f) == 0);
    GS_ASSERT((e.light & 0x0f) == get_skylight(x,y,z));

    //placed solid block
    if (fast_cube_properties[e.block].solid)
    {
        for (int j=0; j<6; j++)
        {
            struct MapElement e = get_element(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
            if (!fast_cube_properties[e.block].solid)
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

    class MapChunk* mc = main_map->chunk[32*chunk_j + chunk_i];

    /*
        Envlight
    */
    for (int k=0; k<128; k++)
    {
        for (int i=0; i<16; i++)
        for (int j=0; j<16; j++)
        {
            int x = 16*chunk_i + i;
            int y = 16*chunk_j + j;

            struct MapElement e = mc->get_element(i,j,k);

            if (fast_cube_properties[e.block].light_source)
            {
                set_envlight(x,y,k, fast_cube_attributes[e.block].light_value);
            }
            else
            {
                if (fast_cube_properties[e.block].solid)
                {
                    GS_ASSERT(get_envlight(x,y,k) == 0);
                }
                else
                {
                    //set_envlight(x,y,k, 0);
                }
            }
        }

        if (k % 8 == 7)
            _envlight_update_core();
    }


}

void assert_skylight(int chunk_i, int chunk_j)
{
    //debugging
    return;

    int k = map_dim.z-1;

    for (; k>=0; k--)
    for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;

       if (!isSolid(x,y,k))
            _push_skylight_update(x,y,k);
    }
/*
    while (sky_light_array_index > 0)
    {
        _skylight_update_core(0);
    }
*/

    //_skylight_update_core(0);
    //_skylight_update_core(0);
    _skylight_update_core(1600);

    return;


    for (int k=0; k<map_dim.z; k++)
    for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;
        int z = k;

        int li = get_skylight(x,y,z);

        if (isSolid(x,y,z))
            continue;

        GS_ASSERT(li != 15 || !isSolid(x,y,z+1));

        if (li==15)
        {

            struct MapElement e = get_element(x,y,z+1);
            if (get_skylight(x,y,z+1) != 15)
            {
                printf("ERROR: block= %d light= %d \n", e.block, get_skylight(x,y,z+1));
            }


            GS_ASSERT(get_skylight(x,y,z+1) == 15);       //this is getting triggered
            GS_ASSERT(!isSolid(x,y,z+1));
        }

    }

}

/*
    Rolling lighting update
*/
const int _rolling_lighting_prime = 1553; //prime number
int* _rolling_index_array = NULL; // 32x32

//will update ~10 blocks per call with 3217 as prime
//will take 3217 calls to converge?

OPTIMIZED
void _lighting_rolling_update(int chunk_i, int chunk_j, int itr_count_max)
{

    class MapChunk* mc = main_map->chunk[ 32*chunk_j + chunk_i ];
    if(mc == NULL)
    {
        GS_ASSERT(false);
        return;
    }

    int cindex = 32*chunk_j+chunk_i;
    int index = _rolling_index_array[cindex];

    int itr_count = 0;

    while(itr_count < itr_count_max)
    {
        itr_count++;
        index += _rolling_lighting_prime;

        if(index >= 16*16*128)
        {
            index = (index+1) % _rolling_lighting_prime;
        }

        int x = 32*chunk_i + index % 16;
        int y = 32*chunk_j + (index >> 4) % 16; //divide by 16
        int z = index >> 8;                     //(index/256);

        struct MapElement e = get_element(x,y,z);

        if (!fast_cube_properties[e.block].solid)
        {
            _push_skylight_update(x,y,z);       //use condition check first
            _push_envlight_update(x,y,z);
        }
    }

    _rolling_index_array[cindex] = index;
}

void init_update_sunlight0(int chunk_i, int chunk_j)
{
    class MapChunk* mc = main_map->chunk[ 32*chunk_j + chunk_i ];
    IF_ASSERT(mc == NULL)
        return;

    for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;
        int k = map_dim.z-1;

        struct MapElement e;
        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if (e.block != 0)    //iterate until we hit top block
                break;
            set_skylight(x,y,k, 15);
            //_push_skylight_update(x,y,k);
        }
        if (k < 0) continue;
        // black out everything below

        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if (e.block != 0)
                continue;
            //set_skylight(x,y,k, 0);
            //_push_skylight_update(x,y,k);
        }
    }
}

void init_update_sunlight1(int chunk_i, int chunk_j)
{
    class MapChunk* mc = main_map->chunk[ 32*chunk_j + chunk_i ];
    IF_ASSERT(mc == NULL)
        return;

    for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    {
        int x = i + 16*chunk_i;
        int y = j + 16*chunk_j;
        int k = map_dim.z-1;

        struct MapElement e;
        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if (e.block != 0)    //iterate until we hit top block
                break;
            //set_skylight(x,y,k, 15);
            _push_skylight_update(x,y,k);
        }
        if (k < 0) return;
        // black out everything below

        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            //e = get_element(x,y,k);
            if (e.block != 0)
                continue;
            //set_skylight(x,y,k, 0);
            _push_skylight_update(x,y,k);
        }
    }
}

/*

*/

void lighting_rolling_update(int max_updates);

void post_gen_map_lighting()
{
    int t0 = _GET_MS_TIME();

    for(int i=0; i<32; i++)
    for(int j=0; j<32; j++)
    {
        init_update_sunlight0(i,j);
    }

    for(int i=0; i<32; i++)
    for(int j=0; j<32; j++)
    {
        init_update_sunlight1(i,j);

        while(sky_light_array_num != 0)
            _skylight_update_core(64*1024);
    }

/*
    for(int i=0; i<32; i++)
    for(int j=0; j<32; j++)
    {
        _lighting_rolling_update(i,j, 128*16*16);
        while(sky_light_array_num != 0)
            _skylight_update_core(1000);
    }
*/

    int t1 = _GET_MS_TIME();
    printf("post_gen_map_lighting: %d ms\n", t1-t0);

}

/*
    Interface.hpp
*/

//replaces  main_map->lighting_rolling_update(max_updates);

void lighting_rolling_update(int max_updates)
{

    //too many pending updates
    ////if(sky_light_array_num > 0)
    //    return;

    int count = 1;

    for (int i=0; i<32; i++)
    for (int j=0; j<32; j++)
    {
        if (main_map->chunk[i + MAP_CHUNK_XDIM*j] != NULL)
            count++;
    }

    int _max_updates = (max_updates / count) + 1; //updates per chunk

    //printf("max_updates= %d \n", _max_updates);

    for (int i=0; i<32; i++)
    for (int j=0; j<32; j++)
    {
        if (main_map->chunk[i + MAP_CHUNK_XDIM*j] == NULL)
            continue;
        _lighting_rolling_update(i,j, _max_updates);
    }
}


/*
    Init and Teardown
*/

void init_lighting()
{
    GS_ASSERT(env_light_array == NULL);
    GS_ASSERT(sky_light_array == NULL);
    env_light_array = (struct LightUpdateElement*) malloc(env_light_array_max* sizeof(struct LightUpdateElement));
    sky_light_array = (struct LightUpdateElement*) malloc(sky_light_array_max* sizeof(struct LightUpdateElement));

    _rolling_index_array = (int*) malloc(sizeof(int)*32*32);
    for(int i=0; i<32*32; i++)
        _rolling_index_array[i] = rand() % _rolling_lighting_prime;


/*
    skylight_array_debug = new int[512*512*128];
    envlight_array_debug = new int[512*512*128];

    for(int i=0; i<512*512*128; i++)
    {
        skylight_array_debug[i] = -1;
        envlight_array_debug[i] = -1;
    }
*/
}

void teardown_lighting()
{
    free(env_light_array);
    free(sky_light_array);
    free(_rolling_index_array);
}

}   // t_map
