#pragma once


#ifndef GLuint
    typedef unsigned int GLuint;
#endif

struct SDL_Surface;
struct SDL_PixelFormat;

GLuint block_texture = 0;
//GLuint block_texture_no_gamma_correction = 0; //deprecate if nothing is using this

namespace t_map
{
    const int MAX_TEXTURES = MAX_CUBES*6;

    unsigned char cube_side_texture_array[MAX_CUBES*6]; // for now?

    GLuint terrain_map_glsl = 0 ; //for shader
    GLuint block_textures_normal = 0; //linear scale

    int must_lock_block_surface;
    SDL_Surface *block_surface;
    SDL_PixelFormat *block_surface_pixel_format;
    int block_surface_width, block_surface_height;

    void init_textures();

    void set_cube_side_texture(int id, int side, int tex_id);
    int get_cube_side_texture(int id, int side) 
    {
        return cube_side_texture_array[6*id + side]; 
    }
    int get_cube_primary_texture_index(char* name)
    {
        int id = get_cube_id(name);
        const int primary_side = 0; // TODO?
        int cube = get_cube_side_texture(id, primary_side);
        if (cube == 0) cube = 255;  // use error cube
        return cube;
    }

    
    void get_random_pixel(int cube_id, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
    void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);
    
}

extern "C"
{
void set_cube_side_texture(int id, int side, int tex_id) GNOMESCROLL_API;
}
