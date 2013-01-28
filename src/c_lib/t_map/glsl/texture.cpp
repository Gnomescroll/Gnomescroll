#include "texture.hpp"

#if DC_CLIENT
# include <SDL/texture_sheet_loader.hpp>
#endif

namespace t_map
{

unsigned char* cube_side_texture_array;

int get_cube_side_texture(CubeType type, int side)
{
    ASSERT_VALID_CUBE_TYPE(type);
    IF_INVALID_CUBE_TYPE(type)
    {
        printf("bad type: %d\n", type);
        return 0;
    }
    IF_ASSERT(side < 0 || side >= N_SIDES) return 0;
    return cube_side_texture_array[N_SIDES*type + side];
}

#if DC_CLIENT
GLuint terrain_map_glsl = 0; //for shader
GLuint block_textures_normal = 0; //linear scale

int must_lock_block_surface = 0;
SDL_Surface *block_surface = NULL;
SDL_PixelFormat *block_surface_pixel_format = NULL;
int block_surface_width, block_surface_height;

struct SDL_Surface* block_item_64_surface = NULL;
struct SDL_Surface* block_item_16_surface = NULL;

const int N_PIXEL_SAMPLES = 10;
const static int TEXTURE_WIDTH = 32;
static unsigned char** pixel_data = NULL;

void set_cube_side_texture(CubeType type, int side, int tex_id)
{
    IF_INVALID_CUBE_TYPE(type) return;
    IF_ASSERT(side < 0 || side >= N_SIDES) return;
    IF_ASSERT(tex_id < 0 || tex_id >= MAX_TEXTURES) return;
    cube_side_texture_array[N_SIDES*type + side] = tex_id;

    // Init Pixel Sampler
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

void get_random_pixel(CubeType type, int side, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a)
{
    int tex_id = get_cube_side_texture(type, side);
    int ra = (TEXTURE_WIDTH*TEXTURE_WIDTH)*tex_id + (rand() % (TEXTURE_WIDTH*TEXTURE_WIDTH));
    unsigned int t = TextureSheetLoader::cube_texture_sheet_loader->texture_stack[ra];
    *r = (t >> (8*0)) & 0xFF;
    *g = (t >> (8*1)) & 0xFF;
    *b = (t >> (8*2)) & 0xFF;
    *a = (t >> (8*3)) & 0xFF;
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
#endif

void init_textures()
{
    GS_ASSERT(cube_side_texture_array == NULL);
    cube_side_texture_array = (unsigned char*)calloc(MAX_CUBES*N_SIDES, sizeof(unsigned char));
    #if DC_CLIENT
    GS_ASSERT(pixel_data == NULL);
    pixel_data = (unsigned char**)calloc(MAX_TEXTURES, sizeof(unsigned char));
    #endif
}

void teardown_textures()
{
    if (cube_side_texture_array != NULL) free(cube_side_texture_array);
    #if DC_CLIENT
    if (pixel_data != NULL) free(pixel_data);
    #endif
}

}    // t_map
