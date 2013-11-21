/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#ifndef GLuint
typedef unsigned int GLuint;
#endif

#include <t_map/glsl/structs.hpp>

namespace t_map
{

const int N_SIDES = 6;
const int MAX_TEXTURES = MAX_CUBES*N_SIDES;
extern unsigned char* cube_side_texture_array;

void init_textures();
void teardown_textures();

int get_cube_side_texture(CubeType type, int side);

int get_cube_primary_texture_index(CubeType type)
{
    const int primary_side = 0;
    int tex_index = get_cube_side_texture(type, primary_side);
    return tex_index;
}

int get_cube_primary_texture_index(const char* name)
{
    CubeType type = get_cube_type(name);
    return get_cube_primary_texture_index(type);
}

#if DC_CLIENT
extern GLuint terrain_map_glsl; //for shader
extern GLuint block_textures_normal; //linear scale

extern int must_lock_block_surface;
extern struct SDL_Surface* block_surface;
extern struct SDL_PixelFormat* block_surface_pixel_format;
extern int block_surface_width, block_surface_height;

extern struct SDL_Surface* block_item_64_surface;
extern struct SDL_Surface* block_item_16_surface;

void get_random_pixel(CubeType type, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);

void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);

void set_cube_side_texture(CubeType type, int side, int tex_id);
#endif

}    // t_map
