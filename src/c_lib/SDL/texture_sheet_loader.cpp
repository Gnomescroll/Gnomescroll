#include "texture_sheet_loader.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/macros.hpp>
#include <SDL/texture_loader.hpp>

namespace TextureSheetLoader
{

const unsigned int N_SURFACES = 32;

struct TileMeta
{
    int texture_sheet;
    int xpos;
    int ypos;
};

TextureSheetLoader::TextureSheetLoader(unsigned int tile_size) :
    tile_size(tile_size), texture_num(0), tile_num(0), texture_format(GL_BGRA)
{
    this->meta = new struct TileMeta[256];
    this->filenames = (char**)calloc(N_SURFACES, sizeof(char*));
    this->surfaces = (SDL_Surface**)calloc(N_SURFACES, sizeof(SDL_Surface*));
    
    this->texture_sheet = create_surface_from_nothing(16*tile_size, 16*tile_size);
    this->grey_scale_texture_sheet = create_surface_from_nothing(16*tile_size, 16*tile_size);
    this->texture_stack = (Uint32*) calloc(4*256*tile_size*tile_size, sizeof(char));
}

TextureSheetLoader::~TextureSheetLoader()
{
    if (this->surfaces != NULL)
    {
        for (unsigned int i=0; i<N_SURFACES; i++)
            if (this->surfaces[i] != NULL)
                SDL_FreeSurface(surfaces[i]);
        free(this->surfaces);
    }
    if (this->filenames != NULL)
    {
        for (unsigned int i=0; i<N_SURFACES; i++)
            if (this->filenames[i] != NULL)
                free(this->filenames[i]);
        free(this->filenames);
    }
    if (this->meta != NULL) delete[] this->meta;
    if (this->texture_sheet != NULL) SDL_FreeSurface(this->texture_sheet);
    if (this->grey_scale_texture_sheet != NULL) SDL_FreeSurface(this->grey_scale_texture_sheet);
    if (this->texture_stack != NULL) free(this->texture_stack);
}

SpriteSheet TextureSheetLoader::load_texture(const char* filename)
{
    for (unsigned int i=0; i<this->texture_num; i++)
        if (this->filenames[i] != NULL)
            GS_ASSERT(strcmp(this->filenames[i], filename) != 0);

    SDL_Surface* s = create_surface_from_file(filename);
    GS_ASSERT(s != NULL);
    if (s == NULL) return NULL_SPRITE_SHEET;

    SpriteSheet index = this->load_texture_from_surface(s);

    if (index < 0) SDL_FreeSurface(s);

    GS_ASSERT(this->filenames[index] == NULL);
    this->filenames[index] = (char*)malloc((strlen(filename) + 1) * sizeof(char));
    strcpy(this->filenames[index], filename);

    return index;
}

SpriteSheet TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
    GS_ASSERT(surface != NULL);
    if (surface == NULL) return NULL_SPRITE_SHEET;

    GS_ASSERT(this->texture_num < N_SURFACES);
    if (this->texture_num >= N_SURFACES) return NULL_SPRITE_SHEET;
    
    surfaces[this->texture_num] = surface;

    SpriteSheet tx = (SpriteSheet)this->texture_num;
    this->texture_num += 1;
    return tx;
}

//blit to sheet or return texture id
int TextureSheetLoader::blit(SpriteSheet sheet_id, int source_x, int source_y)
{
    // sanity checks
    GS_ASSERT(sheet_id < texture_num);
    if (sheet_id >= texture_num) return NULL_SPRITE;

    GS_ASSERT(source_x > 0);
    if (source_x < 1) return NULL_SPRITE;
    GS_ASSERT(source_y > 0);
    if (source_y < 1) return NULL_SPRITE;

    GS_ASSERT(this->tile_num <= 0xff);
    if (this->tile_num > 0xff) return NULL_SPRITE;

    // decrement x,y because arguments should be 1-indexed
    source_x--;
    source_y--;

    //check to see if already loaded
    for (unsigned int i=0; i<this->tile_num; i++)
        if (meta[i].texture_sheet == (int)sheet_id
         && meta[i].xpos == source_x
         && meta[i].ypos == source_y)
            return i;

    // get surface
    SDL_Surface* s = this->surfaces[sheet_id];
    GS_ASSERT(s != NULL);
    if (s == NULL) return NULL_SPRITE;

    // record metadata
    meta[tile_num].texture_sheet = sheet_id;
    meta[tile_num].xpos = source_x;
    meta[tile_num].ypos = source_y;
    
    // check that tiles are in bounds
    GS_ASSERT(source_x*(int)tile_size < s->w && source_y*(int)tile_size < s->h);
    if (source_x*(int)tile_size >= s->w || source_y*(int)tile_size >= s->h)
        return NULL_SPRITE;

    // lock surfaces
    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK(this->texture_sheet);

    if (s_lock) SDL_LockSurface(s);
    if (c_lock) SDL_LockSurface(this->texture_sheet);

    // alias pixel buffers
    Uint32* stack_pixels = (Uint32*) this->texture_stack;
    Uint32* sheet_pixels = (Uint32*) this->texture_sheet->pixels;

    // sprite coordinate index into destination pixel buffers
    int index = (tile_num % 16) + 16*(tile_num/16);
    int dest_x = (index % 16) * this->tile_size;
    int dest_y = (index / 16) * this->tile_size;

    // copy sprite icon pixels from source pixel buffer to sheet and stack buffers
    for (unsigned int i=0; i < tile_size; i++)
    for (unsigned int j=0; j < tile_size; j++) 
    {
        unsigned int pix_index = s->w*(tile_size*source_y+j) + (tile_size*source_x+i);
        GS_ASSERT(pix_index < (unsigned int)s->w*s->h);
        
        // convert source pixel to final format
        Uint32 pix = ((Uint32*)s->pixels)[pix_index];
        unsigned char r,g,b,a;
        SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);
        pix = SDL_MapRGBA(this->texture_sheet->format, r,g,b,a);
        
        unsigned int stack_index = tile_size*tile_size*index + (j*tile_size+i);
        GS_ASSERT(stack_index < 256*tile_size*tile_size);
        stack_pixels[stack_index] = pix;
        unsigned int sheet_index = (16*tile_size)*((dest_y+j)) + (dest_x+i);
        GS_ASSERT(sheet_index < (unsigned int)this->texture_sheet->w*this->texture_sheet->h);
        sheet_pixels[sheet_index] = pix;
    }

    // unlock
    if (s_lock) SDL_UnlockSurface(s);
    if (c_lock) SDL_UnlockSurface(texture_sheet);

    // increment sprite/icon/tile count
    return tile_num++;
}

void TextureSheetLoader::generate_grey_scale()
{
    float gamma_correction[256];

    for (int i=0; i<256; i++)
    {
        float intensity = ((float) i) / 255.0f;
        gamma_correction[i] = powf(intensity, 1.0f/2.2f);
    }

    int c_lock = SDL_MUSTLOCK(texture_sheet);
    int s_lock = SDL_MUSTLOCK(grey_scale_texture_sheet);

    if (c_lock) SDL_LockSurface(texture_sheet);
    if (s_lock) SDL_LockSurface(grey_scale_texture_sheet);

    Uint8 r,g,b,a;

    for (unsigned int x=0; x<16*tile_size; x++)
    for (unsigned int y=0; y<16*tile_size; y++)
    {
        unsigned int index = y*16*tile_size + x;
        GS_ASSERT(index < (unsigned int)grey_scale_texture_sheet->w*grey_scale_texture_sheet->h);
        Uint32 pix = ((Uint32*)texture_sheet->pixels)[index];
        SDL_GetRGBA(pix, texture_sheet->format, &r, &g, &b, &a);

        float avg = (gamma_correction[r] + gamma_correction[g] + gamma_correction[b]) / 3.0f;
        avg = powf(avg, 2.2f);
        GS_ASSERT(avg >= 0.0f && avg <= 1.0f);

        unsigned char g = (unsigned char)(255.0f*avg);

        ((Uint32*)grey_scale_texture_sheet->pixels)[index] = SDL_MapRGBA(grey_scale_texture_sheet->format, g,g,g,a);
    }

    if (c_lock) SDL_UnlockSurface(texture_sheet);
    if (s_lock) SDL_UnlockSurface(grey_scale_texture_sheet);
}


class TextureSheetLoader* CubeTextureSheetLoader = NULL;
Uint32* CubeTextureStack = NULL;
struct SDL_Surface* CubeSurface = NULL;

class TextureSheetLoader* ItemTextureSheetLoader = NULL;
Uint32* ItemTextureStack = NULL;
struct SDL_Surface* ItemSurface = NULL;
struct SDL_Surface* GreyScaleItemSurface = NULL;
GLuint ItemSheetTexture = 0;
GLuint GreyScaleItemTexture = 0;

void init()
{
    //CubeSurfaceSheet
    GS_ASSERT(CubeTextureSheetLoader == NULL);
    GS_ASSERT(CubeTextureStack == NULL);
    GS_ASSERT(CubeSurface == NULL);

    CubeTextureSheetLoader = new TextureSheetLoader(16);        //pixel size for cube textures
    CubeTextureStack = CubeTextureSheetLoader->texture_stack;
    CubeSurface = CubeTextureSheetLoader->texture_sheet;
    
    //ItemTextureSheet
    GS_ASSERT(ItemTextureSheetLoader == NULL);
    GS_ASSERT(ItemTextureStack == NULL);
    GS_ASSERT(ItemSurface == NULL);
    GS_ASSERT(ItemSheetTexture == 0);

    ItemTextureSheetLoader = new TextureSheetLoader(16);
    ItemTextureStack = ItemTextureSheetLoader->texture_stack;
    ItemSurface = ItemTextureSheetLoader->texture_sheet;
}

void init_item_texture()
{
    GS_ASSERT(ItemSheetTexture == 0);
    if (ItemSheetTexture != 0) return;
    
    GS_ASSERT(ItemSurface != NULL);
    if (ItemSurface == NULL) return;

    GS_ASSERT(ItemTextureSheetLoader != NULL);
    if (ItemTextureSheetLoader == NULL) return;

    GLenum format = GL_BGRA;
    if (ItemSurface->format->Rmask == 0x000000ff)
        format = GL_RGBA;

    ItemTextureSheetLoader->texture_format = format;

    GLenum MAG_FILTER = GL_NEAREST;
    create_texture_from_surface(ItemSurface, &ItemSheetTexture, MAG_FILTER);
}

void init_greyscale()
{    
    GS_ASSERT(GreyScaleItemSurface == NULL);
    GS_ASSERT(GreyScaleItemTexture == 0);
    ItemTextureSheetLoader->generate_grey_scale();
    GreyScaleItemSurface = ItemTextureSheetLoader->grey_scale_texture_sheet;
    create_texture_from_surface(GreyScaleItemSurface, &GreyScaleItemTexture, GL_NEAREST);
}

void teardown_item_texture()
{
    if (ItemSheetTexture != 0) glDeleteTextures(1, &ItemSheetTexture);
}

void teardown()
{
    if (CubeTextureSheetLoader != NULL) delete CubeTextureSheetLoader;
    if (ItemTextureSheetLoader != NULL) delete ItemTextureSheetLoader;
    teardown_item_texture();
}

SpriteSheet load_cube_texture_sheet(const char* filename)
{
    return CubeTextureSheetLoader->load_texture(filename);
}

int blit_cube_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return CubeTextureSheetLoader->blit(sheet_id, source_x, source_y);
}

void save_cube_texture()
{
    save_surface_to_png(CubeSurface, SCREENSHOT_PATH "cubes.png");
}

//Item API
SpriteSheet load_item_texture_sheet(const char* filename)
{
    return ItemTextureSheetLoader->load_texture(filename);
}

SpriteSheet load_item_texture(struct SDL_Surface* surface)
{
    return ItemTextureSheetLoader->load_texture_from_surface(surface);
}

int blit_item_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return ItemTextureSheetLoader->blit(sheet_id, source_x, source_y);
}

void save_item_texture()
{
    GS_ASSERT(ItemSurface != NULL);
    GS_ASSERT(GreyScaleItemSurface != NULL);
    if (ItemSurface != NULL)
        save_surface_to_png(ItemSurface, SCREENSHOT_PATH "items.png");
    if (GreyScaleItemSurface != NULL)
        save_surface_to_png(GreyScaleItemSurface, SCREENSHOT_PATH "greyscale_items.png");
}

}   // TextureSheetLoader
