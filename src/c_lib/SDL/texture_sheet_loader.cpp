#include "texture_sheet_loader.hpp"

#include <SDL/texture_loader.hpp>

namespace TextureSheetLoader
{

struct TileMeta
{
    int texture_sheet;
    int xpos;
    int ypos;
};

TextureSheetLoader::TextureSheetLoader(int tile_size)
{
    meta = new struct TileMeta[256];

    TILE_SIZE = tile_size;
    for (int i=0; i<N_SURFACES; i++) surfaces[i] = NULL;
    texture_num=0;
    tile_num=0;

    texture_sheet = create_surface_from_nothing(16*TILE_SIZE, 16*TILE_SIZE);
    grey_scale_texture_sheet = create_surface_from_nothing(16*TILE_SIZE, 16*TILE_SIZE);
    texture_stack = (Uint32*) calloc(4*256*TILE_SIZE*TILE_SIZE, sizeof(char));
}

TextureSheetLoader::~TextureSheetLoader()
{
    for (int i=0; i<N_SURFACES; i++)
        if (this->surfaces[i] != NULL) SDL_FreeSurface(surfaces[i]);
    delete[] meta;
    if (this->texture_sheet != NULL) SDL_FreeSurface(texture_sheet);
    if (this->grey_scale_texture_sheet != NULL) SDL_FreeSurface(grey_scale_texture_sheet);
    if (this->texture_stack != NULL) free(texture_stack);
}

int TextureSheetLoader::load_texture(char* filename)
{
    int id = texture_num;

    surfaces[texture_num] = create_surface_from_file(filename);

	GS_ASSERT(surfaces[texture_num] != NULL);
    if (surfaces[texture_num] == NULL)
    {
        printf("TextureSheetLoader: loading spritesheet %s failed \n", filename);
        return -1;
    }

    texture_num++;
    return id;
}

int TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
    int id = texture_num;
    GS_ASSERT(id < N_SURFACES);
    if (id >= N_SURFACES) return -1;
    surfaces[texture_num]= surface;
    texture_num++;
    return id;
}


void TextureSheetLoader::reload_texture(int id, char* filename)
{

}

//blit to sheet or return texture id
int TextureSheetLoader::blit(int sheet_id, int source_x, int source_y)
{
    if (source_x < 1)
    {
        printf("TextureSheetLoader::blit source_x less than 1: sheet_id= %i source_y= %i source_x= %i \n", sheet_id, source_x, source_y);
        GS_ASSERT(false);
        return 255;
    }
    if (source_y < 1)
    {
        printf("TextureSheetLoader::blit source_y less than 1: sheet_id= %i source_y= %i source_x= %i \n", sheet_id, source_x, source_y);
        GS_ASSERT(false);
        return 255;
    }
    source_y --;
    source_x --;

    if (sheet_id >= texture_num)
    {
        printf("Error: TextureSheetLoader::blit error!!! FIX \n");
        return 255;
    }    

    //check to see if already loaded
    for (int i=0; i<tile_num; i++)
    {
        struct TileMeta m = meta[i];
        if (m.texture_sheet == sheet_id && m.xpos == source_x && m.ypos == source_y)
            return i;
    }

    //increment and store
    struct TileMeta m;
    m.texture_sheet = sheet_id;
    m.xpos = source_x;
    m.ypos = source_y;

    meta[tile_num] = m;
    int INDEX = tile_num;
    tile_num++;

    SDL_Surface* s = this->surfaces[sheet_id];
    GS_ASSERT(s != NULL);
    if (s == NULL) return 255;
    
    CubeTextureStack = (Uint32*) this->texture_stack;

    if (source_x * TILE_SIZE >= s->w || source_y * TILE_SIZE >= s->h)
    {
        printf("Error: LUA_blit_cube_texture, texture out of bounds\n");
        return 255;
    }

    int index = (INDEX % 16) + 16*(INDEX/16);
    Uint32 pix; 

    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK(texture_sheet);

    if (s_lock) SDL_LockSurface(s);
    if (c_lock) SDL_LockSurface(texture_sheet);

    Uint32* Pixels1 = (Uint32*) CubeTextureStack;
    Uint32* Pixels2 = (Uint32*) texture_sheet->pixels;

    int dest_x = index % 16;
    int dest_y = index / 16;

    for (int i=0; i < TILE_SIZE; i++)
    for (int j=0; j < TILE_SIZE; j++) 
    {
        pix = ((Uint32*) s->pixels)[ s->w*(j+TILE_SIZE*source_y) + (i+TILE_SIZE*source_x) ];
        
        Pixels1[ TILE_SIZE*TILE_SIZE*index + (j*TILE_SIZE+i) ] = pix;
        Pixels2[ (16*TILE_SIZE)*((dest_y*TILE_SIZE + j)) + (TILE_SIZE*dest_x + i) ] = pix;
    }

    if (s_lock) SDL_UnlockSurface(s);
    if (c_lock) SDL_UnlockSurface(texture_sheet);

    return INDEX;
}

void TextureSheetLoader::generate_grey_scale()
{
    float gamma_correction[256];

    for (int i=0; i< 256; i++)
    {
        float intensity = ((float) i) / 255.0f;
        gamma_correction[i] = powf(intensity, 1.0f/2.2f);
    }

    int c_lock = SDL_MUSTLOCK(texture_sheet);
    int s_lock = SDL_MUSTLOCK(grey_scale_texture_sheet);

    if (c_lock) SDL_LockSurface(texture_sheet);
    if (s_lock) SDL_LockSurface(grey_scale_texture_sheet);

    Uint8 r,g,b,a;

    for (int x=0; x<16*TILE_SIZE; x++)
    for (int y=0; y<16*TILE_SIZE; y++)
    {
        int index = y*16*TILE_SIZE + x;
        Uint32 pix = ((Uint32*)texture_sheet->pixels)[index];
        SDL_GetRGBA(pix, texture_sheet->format, &r, &g, &b, &a);

        float avg = (gamma_correction[r] + gamma_correction[g] + gamma_correction[b]) / 3.0f;
        avg = powf(avg, 2.2f);

        if (avg > 1.0f || avg < 0.0f) printf("ERROR TextureSheetLoader::generate_grey_scale: %f \n", avg);

        unsigned char g = (int)(255.0f*avg);

        ((Uint32*)grey_scale_texture_sheet->pixels)[index] = SDL_MapRGBA(grey_scale_texture_sheet->format, g,g,g,a);

    }

    if (c_lock) SDL_UnlockSurface(texture_sheet);
    if (s_lock) SDL_UnlockSurface(grey_scale_texture_sheet);

}


class TextureSheetLoader* CubeTextureSheetLoader = NULL;
struct SDL_Surface* CubeSurface = NULL;
Uint32* CubeTextureStack = NULL;

class TextureSheetLoader* ItemTextureSheetLoader = NULL;
struct SDL_Surface* ItemSurface = NULL;
Uint32* ItemTextureStack = NULL;
GLuint ItemSheetTexture = 0;

struct SDL_Surface* GreyScaleItemSurface = NULL;
GLuint GreyScaleItemTexture = 0;

void init()
{
    //CubeSurfaceSheet
    CubeTextureSheetLoader = new TextureSheetLoader(32);
    CubeTextureStack = CubeTextureSheetLoader->texture_stack;
    CubeSurface = CubeTextureSheetLoader->texture_sheet;
    //ItemTextureSheet
    ItemTextureSheetLoader = new TextureSheetLoader(16);
    ItemTextureStack = ItemTextureSheetLoader->texture_stack;
    ItemSurface = ItemTextureSheetLoader->texture_sheet;
}

void init_item_texture()
{
    create_texture_from_surface(ItemSurface, &ItemSheetTexture, GL_NEAREST);
}

void init_greyscale()
{    
    ItemTextureSheetLoader->generate_grey_scale();
    GreyScaleItemSurface = ItemTextureSheetLoader->grey_scale_texture_sheet;
    save_surface_to_png(GreyScaleItemSurface, (char*)"./screenshot/grey_scale_items.png");
    save_surface_to_png(ItemSurface , (char*)"./screenshot/items.png");
    create_texture_from_surface(GreyScaleItemSurface, &GreyScaleItemTexture, GL_NEAREST);
}

void teardown()
{
    delete CubeTextureSheetLoader;
    delete ItemTextureSheetLoader;
}


}


int LUA_load_cube_texture_sheet(char* filename)
{
    return TextureSheetLoader::CubeTextureSheetLoader->load_texture(filename);
}

int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y)
{
    return TextureSheetLoader::CubeTextureSheetLoader->blit(sheet_id, source_x, source_y);
}

void LUA_save_cube_texture()
{
    save_surface_to_png(TextureSheetLoader::CubeSurface, (char*) "./screenshot/cube_texture.png");
}

//Item API
int LUA_load_item_texture_sheet(char* filename)
{
    return TextureSheetLoader::ItemTextureSheetLoader->load_texture((char*) filename);
}

int LUA_load_item_texture(struct SDL_Surface* surface)
{
    return TextureSheetLoader::ItemTextureSheetLoader->load_texture_from_surface(surface);
}

int LUA_blit_item_texture(int sheet_id, int source_x, int source_y)
{
    return TextureSheetLoader::ItemTextureSheetLoader->blit(sheet_id, source_x, source_y);
}

void LUA_save_item_texture()
{
    save_surface_to_png(TextureSheetLoader::ItemSurface, (char*) "./screenshot/item_texture.png");
}


