#pragma once

#ifndef GLuint
    typedef unsigned int GLuint;
#endif

#include <t_map/glsl/structs.hpp>

namespace t_map
{
    
const int MAX_TEXTURES = MAX_CUBES*6;

extern unsigned char cube_side_texture_array[MAX_CUBES*6]; // for now?

extern GLuint terrain_map_glsl; //for shader
extern GLuint block_textures_normal; //linear scale

extern int must_lock_block_surface;
extern struct SDL_Surface* block_surface;
extern struct SDL_PixelFormat* block_surface_pixel_format;
extern int block_surface_width, block_surface_height;

extern struct SDL_Surface* block_item_64_surface;
extern struct SDL_Surface* block_item_16_surface;

void init_textures();

void set_cube_side_texture(int id, int side, int tex_id);
int get_cube_side_texture(int id, int side) 
{
    return cube_side_texture_array[6*id + side]; 
}

int get_cube_primary_texture_index(const char* name)
{
    int id = get_cube_id(name);
    const int primary_side = 0;
    int cube = get_cube_side_texture(id, primary_side);
    return cube;
}

int get_cube_primary_texture_index(int id)
{
    const int primary_side = 0;
    int cube = get_cube_side_texture(id, primary_side);
    return cube;
}

void get_random_pixel(int cube_id, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);

void set_cube_side_texture(int id, int side, int tex_id);

//static int cube_palette_index = 1;

}    // t_map
