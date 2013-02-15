#pragma once

#include <t_map/common/constants.hpp>

namespace t_map
{

static int cube_texture_palette_index = 1;
static int cube_texture_palette_lookup[MAX_CUBES];
static int cube_texture_palette_lookup_max[MAX_CUBES]; // number of pallete elements per cube
static unsigned char cube_texture_palette[MAX_CUBES*2*6];

static int cube_texture_palette_debug[MAX_CUBES];

//static int cube_color_palette_index = 1;
//static int cube_color_palette_lookup[MAX_CUBES];
//static unsigned short cube_color_palette_lookup_max[MAX_CUBES];
//static struct ColorElement cube_color_palette[MAX_CUBES];

void start_cube_palette(CubeType cube_type);
void end_cube_palette();
void set_cube_palette_texture(CubeType id, int side, int tex_id);


void start_cube_palette(CubeType cube_type)
{
    static int last_cube_type = -1;
    cube_texture_palette_debug[cube_texture_palette_index] = 1;

    if (cube_texture_palette_lookup[cube_type] == 0)
    {
        cube_texture_palette_lookup[cube_type] = cube_texture_palette_index;
        //cube_color_palette_lookup[cube_type] = cube_color_palette_index;
    }
    else
    {
        //if this cube id has been used before
        GS_ASSERT_ABORT(cube_type == last_cube_type);
    }
    cube_texture_palette_lookup_max[cube_type]++;
    last_cube_type = cube_type;
}

void push_cube_palette()
{
    cube_texture_palette_index++;
}

void set_cube_palette_texture(CubeType id, int side, int tex_id)
{
    GS_ASSERT_ABORT(cube_texture_palette_debug[cube_texture_palette_index] == 1);

    cube_texture_palette[6*cube_texture_palette_index + side] = tex_id;

    GS_ASSERT(cube_texture_palette_index <= cube_texture_palette_lookup[id] + cube_texture_palette_lookup_max[id]);
    GS_ASSERT(cube_texture_palette_index >= cube_texture_palette_lookup[id]);
}

/*
    Color Palette
*/

/*
void set_cube_palette_color(int r, int g, int b, int a);

void set_cube_palette_color(int r, int g, int b, int a)
{
    struct ColorElement c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    cube_color_palette[cube_color_palette_index] = c;
    cube_color_palette_index++;
}
*/

void print_palette()
{

    for (int i=0; i<MAX_CUBES; i++)
    {
        //printf("block_id= %i index= %i max= %i \n", i, cube_texture_palette_lookup[i], cube_texture_palette_lookup_max[i] );
        if (cube_texture_palette_lookup[i] != 0)
        {
            for (int j=0;j<cube_texture_palette_lookup_max[i]; j++)
            {
                printf("\t %i: ", j);
                for (int k=0; k<6; k++) printf("%i ",  cube_texture_palette[6*(cube_texture_palette_lookup[i]+j)+k] );
                printf("\n");
            }
        }
    }

    //abort();
}

}
