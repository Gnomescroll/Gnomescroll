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
    for(int i=0; i<32; i++) textures[i] = NULL;
    texture_num=0;
    tile_num=0;

    texture_sheet = create_surface_from_nothing(16*TILE_SIZE, 16*TILE_SIZE);
    grey_scale_texture_sheet = create_surface_from_nothing(16*TILE_SIZE, 16*TILE_SIZE);
    texture_stack = (unsigned int*) malloc(4*256*TILE_SIZE*TILE_SIZE);
}

TextureSheetLoader::~TextureSheetLoader()
{
    for (int i=0; i<32; i++)
    {
        if (this->textures[i] != NULL) SDL_FreeSurface(textures[i]);
    }
    delete[] meta;
}

int TextureSheetLoader::load_texture(char* filename)
{
    int id = texture_num;

    textures[texture_num] = _load_image(filename);

    if(textures[texture_num] == NULL)
    {
        printf("TextureSheetLoader: loading spritesheet %s failed \n", filename);
        return 0;
    }

    texture_num++;
    return id;
}

int TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
    int id = texture_num;
    textures[texture_num]= surface;
    texture_num++;
    return id;
}


void TextureSheetLoader::reload_texture(int id, char* filename)
{

}

//blit to sheet or return texture id
int TextureSheetLoader::blit(int sheet_id, int source_x, int source_y)
{
    if(source_x < 1)
    {
        printf("TextureSheetLoader::blit source_x less than 1: sheet_id= %i source_y= %i source_x= %i \n", sheet_id, source_x, source_y);
        GS_ASSERT(false);
        return 0;
    }
    if(source_y < 1)
    {
        printf("TextureSheetLoader::blit source_y less than 1: sheet_id= %i source_y= %i source_x= %i \n", sheet_id, source_x, source_y);
        GS_ASSERT(false);
        return 0;
    }
    source_y --;
    source_x --;

    if(sheet_id >= texture_num)
    {
        printf("Error: TextureSheetLoader::blit error!!! FIX \n");
        return 255;
    }    
    //check to see if already loaded
    for(int i=0; i<tile_num; i++)
    {
        struct TileMeta m = meta[i];
        if(m.texture_sheet == sheet_id && m.xpos == source_x && m.ypos == source_y)
        {
            //printf("TextureSheetLoader::blit \n");
            return i;
        }
    }

    //increment and store
    struct TileMeta m;
    m.texture_sheet = sheet_id;
    m.xpos = source_x;
    m.ypos = source_y;

    meta[tile_num] = m;
    int INDEX = tile_num;
    tile_num++;

    /*

    */

    SDL_Surface* s = this->textures[sheet_id];
    
    //SDL_Surface* CubeTexture = TextureSheetLoader::CubeTexture;
    //Uint32* CubeTextureStack = (Uint32*) this->texture_stack;
    CubeTextureStack = (Uint32*) this->texture_stack;

    if( source_x* TILE_SIZE >= s->w || source_y* TILE_SIZE >= s->h )
    {
        printf("Error: LUA_blit_cube_texture, texture out of bounds \n");
        return 255;
    }

    int index = (INDEX % 16) + 16*(INDEX/16);
    Uint32 pix; 

    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK( texture_sheet);

    if(s_lock) SDL_LockSurface(s);
    if(c_lock) SDL_LockSurface( texture_sheet);

    Uint32* Pixels1 = (Uint32*) CubeTextureStack;
    Uint32* Pixels2 = (Uint32*) texture_sheet->pixels;

    int dest_x = index % 16;
    int dest_y = index / 16;

    for(int i=0; i < TILE_SIZE; i++)
    for(int j=0; j < TILE_SIZE; j++) 
    {
        pix = ((Uint32*) s->pixels)[ s->w*(j+TILE_SIZE*source_y) + (i+TILE_SIZE*source_x) ];
        
        Pixels1[ TILE_SIZE*TILE_SIZE*index + (j*TILE_SIZE+i) ] = pix;
        Pixels2[ (16*TILE_SIZE)*( (dest_y*TILE_SIZE + j) ) + (TILE_SIZE*dest_x + i) ] = pix;
    }

    if(c_lock) SDL_UnlockSurface( texture_sheet);
    if(s_lock) SDL_UnlockSurface(s);

    /*

    */
    return INDEX;
}

void TextureSheetLoader::generate_grey_scale()
{
    float gamma_correction[256];

    for(int i=0; i< 256; i++)
    {
        float intensity = ((float) i) / 255;
        gamma_correction[i] = pow(intensity, 1.0/2.2);
    }

    int c_lock = SDL_MUSTLOCK( texture_sheet);
    int s_lock = SDL_MUSTLOCK( grey_scale_texture_sheet);

    if(c_lock) SDL_LockSurface( texture_sheet);
    if(s_lock) SDL_LockSurface( grey_scale_texture_sheet);

    unsigned char* s = (unsigned char*)texture_sheet->pixels;
    unsigned char* d = (unsigned char*)grey_scale_texture_sheet->pixels;

    unsigned char r,g,b,a;

    for(int x=0; x<16*TILE_SIZE; x++)
    for(int y=0; y<16*TILE_SIZE; y++)
    {
        r = s[4*(y*16*TILE_SIZE+x) + 0];
        g = s[4*(y*16*TILE_SIZE+x) + 1];
        b = s[4*(y*16*TILE_SIZE+x) + 2];
        a = s[4*(y*16*TILE_SIZE+x) + 3];

        float avg = (gamma_correction[r] + gamma_correction[g] + gamma_correction[b]) / 3.0;
        avg = pow(avg, 2.2);

        if(avg > 1.0 || avg < 0.0)
        {
            printf("ERROR TextureSheetLoader::generate_grey_scale: %f \n", avg);
        }

        unsigned char g = (int)(255.0*avg);

        d[4*(y*16*TILE_SIZE+x) + 0] = g;
        d[4*(y*16*TILE_SIZE+x) + 1] = g;
        d[4*(y*16*TILE_SIZE+x) + 2] = g;
        d[4*(y*16*TILE_SIZE+x) + 3] = a;

    }

    if(c_lock) SDL_UnlockSurface( texture_sheet);
    if(s_lock) SDL_UnlockSurface( grey_scale_texture_sheet);

}


class TextureSheetLoader* CubeTextureSheetLoader = NULL;
struct SDL_Surface* CubeTexture = NULL;
Uint32* CubeTextureStack = NULL;

class TextureSheetLoader* ItemTextureSheetLoader = NULL;
struct SDL_Surface* ItemSurface = NULL;
Uint32* ItemTextureStack = NULL;
GLuint ItemSheetTexture = 0;

struct SDL_Surface* GreyScaleItemSurface = NULL;
GLuint GreyScaleItemTexture = 0;

void init()
{
    //CubeTextureSheet
    CubeTextureSheetLoader = new TextureSheetLoader(32);
    CubeTextureStack = CubeTextureSheetLoader->texture_stack;
    CubeTexture = CubeTextureSheetLoader->texture_sheet;
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
    save_surface_to_png(TextureSheetLoader::CubeTexture, (char*) "./screenshot/cube_texture.png");
}

//Item API
int LUA_load_item_texture_sheet(char* filename)
{
    return TextureSheetLoader::ItemTextureSheetLoader->load_texture( (char*) filename);
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


