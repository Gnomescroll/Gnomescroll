#include "texture.hpp"

namespace t_map
{

unsigned char cube_side_texture_array[MAX_CUBES*6]; // for now?

GLuint terrain_map_glsl = 0; //for shader
GLuint block_textures_normal = 0; //linear scale

int must_lock_block_surface = 0;
SDL_Surface *block_surface = NULL;
SDL_PixelFormat *block_surface_pixel_format = NULL;
int block_surface_width, block_surface_height;

struct SDL_Surface* block_item_64_surface;
struct SDL_Surface* block_item_16_surface;

}   // t_map

#if DC_CLIENT

#include <SDL/texture_sheet_loader.hpp>

namespace t_map
{

const int N_PIXEL_SAMPLES = 10;
const static int TEXTURE_WIDTH = 16;
static unsigned char* pixel_data[MAX_TEXTURES];
GLuint block_texture = 0;

void init_textures()
{
    static int init = 0;

    if (init == 0)
    {
        memset(pixel_data, 0, MAX_TEXTURES);
        init = 1;
    }
    else
    {
        init = 0;
        printf("Error: init_textures called twice! \n");
    }

}

void set_cube_side_texture(int id, int side, int tex_id) 
{
    //printf("set_cube_side_texture: %i %i %i \n", id, side, tex_id);

    if (tex_id < 0) return;
    if (tex_id > MAX_TEXTURES)
    {
        printf("_set_cube_side_texture: error, tex id would exceed MAX_TEXTURES: id= %i, side=%i, tex_id= %i\n", id, side, tex_id);
        return;
    }
    if (6*id + side > MAX_CUBES*6)
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

    /*

    //init pixel sampler if it has not been inited
    if (pixel_data[tex_id] == NULL) 
    {
        pixel_data[tex_id] = new unsigned char[4*32*32];

        int _tx = tex_id % 16;
        int _ty = tex_id / 16;
                
        int tx = _tx*32;
        int ty = _ty*32;
        
        int px, py;
        unsigned char r,b,g,a;

        for (int i=0; i < 32; i++) 
        for (int j=0; j < 32; j++) 
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

    */
}

void get_random_pixel(int cube_id, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    GS_ASSERT(TextureSheetLoader::CubeTextureStack != NULL);
    if (TextureSheetLoader::CubeTextureStack == NULL) return;
    
    int tex_id = get_cube_side_texture(cube_id, side);
    int ra = (TEXTURE_WIDTH*TEXTURE_WIDTH)*tex_id + (rand() % (TEXTURE_WIDTH*TEXTURE_WIDTH));
    unsigned int t = TextureSheetLoader::CubeTextureStack[ra];
    *r = (t >> (8*0)) & 0xff;
    *g = (t >> (8*1)) & 0xff;
    *b = (t >> (8*2)) & 0xff;
    *a = (t >> (8*3)) & 0xff;
}
/*
    Dont check locking and pull out of pixel buffer instead!!!
*/
void get_texture_pixel(int px, int py, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a)
{
    //if (must_lock_block_surface) SDL_LockSurface(block_surface);
    //int p = px + py*block_surface_width;
    //Uint32 pixel = ((Uint32*)block_surface->pixels)[p];
    //SDL_GetRGBA(pixel, block_surface_pixel_format, r,g,b,a);
    //if (must_lock_block_surface) SDL_UnlockSurface(block_surface);

    printf("texture.cpp, get_texture_pixel(), not implemented\n");
}

}    // t_map

#endif
