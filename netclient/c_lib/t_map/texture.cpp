#include "texture.hpp"


namespace t_map
{

const int N_PIXEL_SAMPLES = 10;
const static int TEXTURE_WIDTH = 32;
static unsigned char* pixel_data[MAX_TEXTURES];

void set_cube_side_texture(int id, int side, int tex_id) 
{
    if(tex_id < 0 ) return;
    if(tex_id > MAX_TEXTURES)
    {
        printf("_set_cube_side_texture: error, tex id would exceed MAX_TEXTURES: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    if(6*id + side > max_cubes*6)
    {
        printf("_set_cube_side_texture: error, would overflow array: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    cube_side_texture_array[6*id +side] = tex_id;


    //DEBUG!!!
    return; //DEBUG!!!
    //DEBUG!!!
    
    /*
        Init Pixel Sampler
    */

    //init pixel sampler if it has not been inited
    if(pixel_data[tex_id] == NULL) 
    {
        pixel_data[tex_id] = new unsigned char[4*32*32];

        int _tx = tex_id % 16;
        int _ty = tex_id / 16;
                
        int tx = _tx*32;
        int ty = _ty*32;
        
        int px, py;
        unsigned char r,b,g,a;

        for(int i=0; i < 32; i++) 
        for(int j=0; j < 32; j++) 
        {
            //pixel offset
            px = tx + i;
            py = ty + j;

            get_texture_pixel(px, py, &r, &g,&b, &a);
            
            pixel_data[tex_id][4*(32*i+j)+0] = r;
            pixel_data[tex_id][4*(32*i+j)+1] = g;
            pixel_data[tex_id][4*(32*i+j)+2] = b;
            pixel_data[tex_id][4*(32*i+j)+3] = a;
        }

    }
}

int get_cube_side_texture(int id, int side) 
{
    return cube_side_texture_array[6*id + side]; 
}

void get_random_pixel(int cube_id, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    int tex_id = get_cube_side_texture(cube_id, side);
    int ra = rand() % (32*32);
    *r = pixel_data[tex_id][4*(ra)+0];
    *g = pixel_data[tex_id][4*(ra)+1];
    *b = pixel_data[tex_id][4*(ra)+2];
    *a = pixel_data[tex_id][4*(ra)+3];
}

void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    if (must_lock_block_surface) SDL_LockSurface(block_surface);
    int p = px + py*block_surface_width;
    Uint32 pixel = ((Uint32*)block_surface->pixels)[p];
    SDL_GetRGBA(pixel, block_surface_pixel_format, r,g,b,a);
    if (must_lock_block_surface) SDL_UnlockSurface(block_surface);
}



void init_textures()
{
    if(block_texture == 0) 
    { 
        printf("init terrain_map: load block textures \n");
        block_surface=IMG_Load("media/texture/blocks_01.png");
        if(!block_surface) {printf("IMG_Load: %s \n", IMG_GetError());return;
    }
    
    block_surface_pixel_format = block_surface->format;
    must_lock_block_surface = SDL_MUSTLOCK(block_surface);
    block_surface_width = (int)block_surface->w;
    block_surface_height = (int)block_surface->h;

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &block_texture );
    glBindTexture( GL_TEXTURE_2D, block_texture );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int texture_format = (block_surface->format->Rmask == 0x000000ff) ? GL_RGBA : GL_BGRA;

    //formats: GL_BGRA_EXT, GL_SRGB_ALPHA_EXT, GL_SRGBA_EXT

    //internal format, input format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8_EXT, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );
    //texture without gamma correction
    glGenTextures( 1, &block_texture_no_gamma_correction );
    glBindTexture( GL_TEXTURE_2D, block_texture_no_gamma_correction );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w,block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    glDisable(GL_TEXTURE_2D);
    }
}

}