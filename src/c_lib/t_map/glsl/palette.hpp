#pragma once

namespace t_map
{

static int cube_texture_palette_index = 1;
static int cube_texture_palette_lookup[256];
static int cube_texture_palette_lookup_max[256]; // number of pallete elements per cube
static unsigned char cube_texture_palette[512*6];

static int cube_texture_palette_debug[256];

//static int cube_color_palette_index = 1;
//static int cube_color_palette_lookup[256];
//static unsigned short cube_color_palette_lookup_max[256];
//static struct ColorElement cube_color_palette[512];

void start_cube_palette(CubeID cube_id);
void end_cube_palette();
void set_cube_palette_texture(int id, int side, int tex_id);


void start_cube_palette(CubeID cube_id)
{
    static int last_cube_id = -1;
    cube_texture_palette_debug[cube_texture_palette_index] = 1;

    if(cube_texture_palette_lookup[cube_id] == 0)
    {
        cube_texture_palette_lookup[cube_id] = cube_texture_palette_index;
        //cube_color_palette_lookup[cube_id] = cube_color_palette_index;
    }
    else
    {
        //if this cube id has been used before
        if(cube_id != last_cube_id) GS_ABORT();
    }
    cube_texture_palette_lookup_max[cube_id]++;
    last_cube_id = cube_id;
}

void push_cube_palette()
{
    cube_texture_palette_index++;
}

void set_cube_palette_texture(int id, int side, int tex_id)
{
    if(cube_texture_palette_debug[cube_texture_palette_index] != 1)
    {
        GS_ABORT();
    }

    cube_texture_palette[6*cube_texture_palette_index + side] = tex_id;

    GS_ASSERT(cube_texture_palette_index <= cube_texture_palette_lookup[id] + cube_texture_palette_lookup_max[id] );
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

    for(int i=0; i<256; i++)
    {
        //printf("block_id= %i index= %i max= %i \n", i, cube_texture_palette_lookup[i], cube_texture_palette_lookup_max[i] );
        if(cube_texture_palette_lookup[i] != 0) 
        {
            for(int j=0;j<cube_texture_palette_lookup_max[i]; j++) 
            {
                printf("\t %i: ", j);
                for(int k=0; k<6; k++) printf("%i ",  cube_texture_palette[6*(cube_texture_palette_lookup[i]+j)+k] );
                printf("\n");
            }
        }
    }

    //abort();
}

}
