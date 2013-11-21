/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "cache.hpp"

#include <t_map/t_properties.hpp>

#include <t_map/glsl/texture.hpp>
#include <t_map/glsl/structs.hpp>

/*
    Deprecate the Quad Cache
*/


namespace t_map
{

void init_quad_cache_vertex_cordinates();
void init_quad_cache_texture_cordinates();
void init_quad_cache_normals();

void init_quad_cache_texture_cordinates_compatibility();
void init_quad_cache_vertex_cordinates_compatibility();

void init_cache()
{
    static int init = 0;
    IF_ASSERT(init++) return;

    quad_cache = (struct Vertex*) malloc(MAX_CUBES*6*4 * sizeof(struct Vertex));
    memset(quad_cache, 0, MAX_CUBES*6*4 * sizeof(struct Vertex));

    quad_cache_compatibility = (struct Vertex*) malloc(MAX_CUBES*6*4 * sizeof(struct Vertex));
    memset(quad_cache_compatibility, 0, MAX_CUBES*6*4 * sizeof(struct Vertex));

    //init_quad_cache_normals();

    init_quad_cache_texture_cordinates();
    init_quad_cache_vertex_cordinates();

    init_quad_cache_texture_cordinates_compatibility();
    init_quad_cache_vertex_cordinates_compatibility();
}

void teardown_cache()
{
    free(quad_cache);
    free(quad_cache_compatibility);
}

static const int v_index[72] =
{
    1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
    0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
    1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
    0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
    1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
    0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
};

void init_quad_cache_vertex_cordinates()
{

    for (int cube_type=0; cube_type<MAX_CUBES; cube_type++)
    {
        for (int side=0; side<6; side++)
        {
            for (int i=0; i<4; i++)
            {
                int index = 12*side+3*i;
                quad_cache[cube_type*6*4 +4*side + i].x = v_index[index + 0];
                quad_cache[cube_type*6*4 +4*side + i].y = v_index[index + 1];
                quad_cache[cube_type*6*4 +4*side + i].z = v_index[index + 2];
            }
        }
    }

}
void init_quad_cache_texture_cordinates()
{

    static const unsigned char _0 = 0;
    static const unsigned char _1 = 1;

    for (int cube_type=0; cube_type<MAX_CUBES; cube_type++)
    {
        for (int side=0; side<6; side++)
        {
            quad_cache[cube_type*6*4 +4*side + 0].tex = 0;
            quad_cache[cube_type*6*4 +4*side + 1].tex = 0;
            quad_cache[cube_type*6*4 +4*side + 2].tex = 0;
            quad_cache[cube_type*6*4 +4*side + 3].tex = 0;

            unsigned char tmp = (unsigned char) cube_side_texture_array[6*cube_type+side];

            //if (cube_type == 1) printf("cube tex= %i \n", tmp);

            quad_cache[cube_type*6*4 +4*side + 0].tx = _0;
            quad_cache[cube_type*6*4 +4*side + 0].ty = _0;
            quad_cache[cube_type*6*4 +4*side + 0].tz = tmp;

            quad_cache[cube_type*6*4 +4*side + 1].tx = _0;
            quad_cache[cube_type*6*4 +4*side + 1].ty = _1;
            quad_cache[cube_type*6*4 +4*side + 1].tz = tmp;

            quad_cache[cube_type*6*4 +4*side + 2].tx = _1;
            quad_cache[cube_type*6*4 +4*side + 2].ty = _1;
            quad_cache[cube_type*6*4 +4*side + 2].tz = tmp;

            quad_cache[cube_type*6*4 +4*side + 3].tx = _1;
            quad_cache[cube_type*6*4 +4*side + 3].ty = _0;
            quad_cache[cube_type*6*4 +4*side + 3].tz = tmp;
        }
    }
}


void init_quad_cache_vertex_cordinates_compatibility()
{

    for (int cube_type=0;cube_type<MAX_CUBES;cube_type++)
    {
        for (int side=0;side<6;side++)
        {
            for (int i=0; i<4; i++)
            {
                int index = 12*side+3*i;
                quad_cache_compatibility[cube_type*6*4 +4*side + i].x = v_index[index + 0];
                quad_cache_compatibility[cube_type*6*4 +4*side + i].y = v_index[index + 1];
                quad_cache_compatibility[cube_type*6*4 +4*side + i].z = v_index[index + 2];
            }
        }
    }

}
void init_quad_cache_texture_cordinates_compatibility()
{

    //static const unsigned char _0 = 0;
    //static const unsigned char _1 = 255;

    //static const unsigned char _0 = 0;
    //static const unsigned char _1 = 255;

    //static const unsigned char _0 = 0;
    //static const unsigned char _1 = 255;

    for (int cube_type=0;cube_type<MAX_CUBES;cube_type++)
    {
        for (int side=0;side<6;side++)
        {
        /*
            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tex = 0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tex = 0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tex = 0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tex = 0;

            int tile_tex = (unsigned char) cube_side_texture_array[6*cube_type+side];

            int ix = (tile_tex % 16)*16;
            int iy = (tile_tex / 16)*16;

            ix = 0;
            iy = 0;

            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tx = _0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 0].ty = _0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tz = ix;
            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tw = iy;

            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tx = _0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].ty = _1;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tz = ix;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].ty = iy;

            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tx = _1;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].ty = _1;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tz = ix;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tw = iy;

            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tx = _1;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].ty = _0;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tz = ix;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tw = iy;
        */

            static const unsigned char _02 = 0;
            static const unsigned char _12 = 255;


            static const struct TextureElement texElementArray2[4] =
            {
                {{{_02,_02,0,0}}},
                {{{_02,_12,0,0}}},
                {{{_12,_12,0,0}}},
                {{{_12,_02,0,0}}}
            };


            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tex = texElementArray2[0].tex;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tex = texElementArray2[1].tex;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tex = texElementArray2[2].tex;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tex = texElementArray2[3].tex;

            //int iz = 0;
            //int iw = 0;

            int tile_tex = (unsigned char) cube_side_texture_array[6*cube_type+side];

            int iz = (tile_tex % 16)*16;
            int iw = (tile_tex / 16)*16;

            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tz = iz;
            quad_cache_compatibility[cube_type*6*4 +4*side + 0].tw = iw;

            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tz = iz;
            quad_cache_compatibility[cube_type*6*4 +4*side + 1].tw = iw;

            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tz = iz;
            quad_cache_compatibility[cube_type*6*4 +4*side + 2].tw = iw;

            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tz = iz;
            quad_cache_compatibility[cube_type*6*4 +4*side + 3].tw = iw;


            //v_list[offset+0].pos = _v_index[4*side+0].pos;
            //v_list[offset+1].pos = _v_index[4*side+1].pos;
            //v_list[offset+2].pos = _v_index[4*side+2].pos;
            //v_list[offset+3].pos = _v_index[4*side+3].pos;


        }
    }
}


void init_quad_cache_normals()
{
    for (int cube_type=0;cube_type<MAX_CUBES;cube_type++)
    {
        for (int side=0;side<6;side++)
        {
            for (int i=0;i<4;i++)
            {
            /*
                int index = cube_type*6*4 +4*side +i;
                quad_cache[index].normal = side;
            */
            }
        }
    }
}

/*
void init_quad_cache_normals()
{

    static const struct NormalElement normal_array[6] = {
        {{{0,0,1,0}}},
        {{{0,0,-1,0}}},
        {{{1,0,0,0}}},
        {{{-1,0,0,0}}},
        {{{0,1,0,0}}},
        {{{0,-1,0,0}}}
        };

    for (int cube_type=0;cube_type<MAX_CUBES;cube_type++)
    {
        for (int side=0;side<6;side++)
        {
            for (int i=0;i<4;i++)
            {
                int index = cube_type*6*4 +4*side +i;
                //quad_cache[index].n = normal_array[side].n;
                //quad_cache[index].normal[0] = normal_array[side].normal[0];
                //quad_cache[index].normal[1] = normal_array[side].normal[1];
                //quad_cache[index].normal[2] = normal_array[side].normal[2];
            }
        }
    }
}
*/
}
