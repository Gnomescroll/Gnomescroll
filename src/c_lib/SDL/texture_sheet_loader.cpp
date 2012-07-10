#include "texture_sheet_loader.hpp"

#include <common/macros.hpp>
#include <SDL/texture_loader.hpp>

namespace TextureSheetLoader
{

const int N_SURFACES = 32;

struct TileMeta
{
    int texture_sheet;
    int xpos;
    int ypos;
};

TextureSheetLoader::TextureSheetLoader(unsigned int tile_size)
: tile_size(tile_size), texture_num(0), tile_num(0), texture_format(GL_BGRA)
{
    this->meta = new struct TileMeta[256];
    this->surfaces = (SDL_Surface**)calloc(N_SURFACES, sizeof(SDL_Surface*));
    
    this->texture_sheet = create_surface_from_nothing(16*tile_size, 16*tile_size);
    this->grey_scale_texture_sheet = create_surface_from_nothing(16*tile_size, 16*tile_size);
    this->texture_stack = (Uint32*) calloc(4*256*tile_size*tile_size, sizeof(char));
}

TextureSheetLoader::~TextureSheetLoader()
{
	if (this->surfaces != NULL)
	{
		for (int i=0; i<N_SURFACES; i++)
			if (this->surfaces[i] != NULL)
				SDL_FreeSurface(surfaces[i]);
		free(this->surfaces);
	}
    if (this->meta != NULL) delete[] this->meta;
    if (this->texture_sheet != NULL) SDL_FreeSurface(this->texture_sheet);
    if (this->grey_scale_texture_sheet != NULL) SDL_FreeSurface(this->grey_scale_texture_sheet);
    if (this->texture_stack != NULL) free(this->texture_stack);
}

int TextureSheetLoader::load_texture(char* filename)
{
	GS_ASSERT(this->texture_num < N_SURFACES);
	if (this->texture_num >= N_SURFACES) return -1;

	GS_ASSERT(this->surfaces[this->texture_num] == NULL);
    this->surfaces[this->texture_num] = create_surface_from_file(filename);

	GS_ASSERT(this->surfaces[this->texture_num] != NULL);
    if (surfaces[this->texture_num] == NULL) return -1;

    return this->texture_num++;
}

int TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
	GS_ASSERT(surface != NULL);
	if (surface == NULL) return -1;
	
    GS_ASSERT(this->texture_num < N_SURFACES);
    if (this->texture_num >= N_SURFACES) return -1;
    surfaces[this->texture_num] = surface;
    return texture_num++;
}

//blit to sheet or return texture id
int TextureSheetLoader::blit(unsigned int sheet_id, int source_x, int source_y)
{
	GS_ASSERT(sheet_id < texture_num);
    if (sheet_id >= texture_num) return 255;

	GS_ASSERT(source_x > 0);
    if (source_x < 1) return 255;
	GS_ASSERT(source_y > 0);
    if (source_y < 1) return 255;

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
    if (s == NULL) return 255;

    // record metadata
    meta[tile_num].texture_sheet = sheet_id;
    meta[tile_num].xpos = source_x;
    meta[tile_num].ypos = source_y;
    
	// check that tiles are in bounds
    GS_ASSERT(source_x*(int)tile_size < s->w && source_y*(int)tile_size < s->h);
    if (source_x*(int)tile_size >= s->w || source_y*(int)tile_size >= s->h)
        return 255;

    int index = (tile_num % 16) + 16*(tile_num/16);

    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK(this->texture_sheet);

    if (s_lock) SDL_LockSurface(s);
    if (c_lock) SDL_LockSurface(this->texture_sheet);

    Uint32* stack_pixels = (Uint32*) this->texture_stack;
    Uint32* sheet_pixels = (Uint32*) this->texture_sheet->pixels;

    int dest_x = (index % 16) * this->tile_size;
    int dest_y = (index / 16) * this->tile_size;

    for (unsigned int i=0; i < tile_size; i++)
    for (unsigned int j=0; j < tile_size; j++) 
    {
		unsigned int pix_index = s->w*(tile_size*source_y+j) + (tile_size*source_x+i);
        Uint32 pix = ((Uint32*)s->pixels)[pix_index];
        unsigned char r,g,b,a;
		SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);
        pix = SDL_MapRGBA(this->texture_sheet->format, r,g,b,a);
        
        stack_pixels[tile_size*tile_size*index + (j*tile_size+i)] = pix;        
        sheet_pixels[(16*tile_size)*((dest_y+j)) + (dest_x+i)] = pix;
    }

    if (s_lock) SDL_UnlockSurface(s);
    if (c_lock) SDL_UnlockSurface(texture_sheet);

	// increment
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

    CubeTextureSheetLoader = new TextureSheetLoader(32);
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
    if (ItemSurface != NULL)
		save_surface_to_png(ItemSurface, (char*)"./screenshot/items.png");    
    if (GreyScaleItemSurface != NULL)
		save_surface_to_png(GreyScaleItemSurface, (char*)"./screenshot/grey_scale_items.png");
    if (CubeTextureSheetLoader != NULL) delete CubeTextureSheetLoader;
    if (ItemTextureSheetLoader != NULL) delete ItemTextureSheetLoader;
    teardown_item_texture();
}

}	// TextureSheetLoader


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


