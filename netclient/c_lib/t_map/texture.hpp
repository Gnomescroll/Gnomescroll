#pragma once


GLuint block_texture = 0;
GLuint block_texture_no_gamma_correction = 0; //deprecate if nothing is using this

namespace t_map
{
     
    int must_lock_block_surface;
    SDL_Surface *block_surface;
    SDL_PixelFormat *block_surface_pixel_format;
    int block_surface_width, block_surface_height;

    void init_textures();

    const int N_PIXEL_SAMPLES = 10;

    
}