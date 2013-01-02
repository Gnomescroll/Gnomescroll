#include "texture_sheet_loader.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/macros.hpp>
#include <SDL/texture_loader.hpp>
#include <t_map/config/textures.hpp>
#include <t_map/glsl/shader.hpp>
#include <animations/sprite_voxelizer.hpp>

namespace TextureSheetLoader
{

const size_t N_SURFACES = 32;

TextureSheetLoader::TextureSheetLoader(size_t tile_size) :
    tile_size(tile_size), tiles_wide(16), tiles_high(16),
    surface_num(0), tile_num(0), format(GL_BGRA),
    texture(0), greyscale_texture(0), mag_filter(GL_NEAREST)
{
    this->meta = new struct TileMeta[this->tiles_wide*this->tiles_high];
    this->filenames = (char**)calloc(N_SURFACES, sizeof(char*));
    this->surfaces = (SDL_Surface**)calloc(N_SURFACES, sizeof(SDL_Surface*));

    this->width = this->tiles_wide*this->tile_size;
    this->height = this->tiles_high*this->tile_size;
    
    this->surface = create_surface_from_nothing(this->width, this->height);
    this->greyscale_surface = create_surface_from_nothing(this->width, this->height);
    this->texture_stack = (Uint32*) calloc(4*this->width*this->height, sizeof(char));

    this->pixels = (Color*)calloc(this->width*this->height, sizeof(Color));

    GLenum format = GL_BGRA;
    if (this->surface->format->Rmask == 0x000000ff)
        format = GL_RGBA;

    this->format = format;
}

TextureSheetLoader::~TextureSheetLoader()
{
    if (this->surfaces != NULL)
    {
        for (size_t i=0; i<N_SURFACES; i++)
            if (this->surfaces[i] != NULL)
                SDL_FreeSurface(surfaces[i]);
        free(this->surfaces);
    }
    if (this->filenames != NULL)
    {
        for (size_t i=0; i<N_SURFACES; i++)
            if (this->filenames[i] != NULL)
                free(this->filenames[i]);
        free(this->filenames);
    }
    if (this->meta != NULL) delete[] this->meta;
    
    if (this->surface != NULL) SDL_FreeSurface(this->surface);
    if (this->greyscale_surface != NULL) SDL_FreeSurface(this->greyscale_surface);
    if (this->texture_stack != NULL) free(this->texture_stack);
    if (this->pixels != NULL) free(this->pixels);

    if (this->texture != 0) glDeleteTextures(1, &this->texture);
    if (this->greyscale_texture != 0) glDeleteTextures(1, &this->greyscale_texture);
}

SpriteSheet TextureSheetLoader::load_texture(const char* filename)
{
    for (size_t i=0; i<this->surface_num; i++)
        if (this->filenames[i] != NULL)
            GS_ASSERT(strcmp(this->filenames[i], filename) != 0);

    SDL_Surface* s = create_surface_from_file(filename);
    IF_ASSERT(s == NULL) return NULL_SPRITE_SHEET;

    SpriteSheet index = this->load_texture_from_surface(s);

    if (index < 0) SDL_FreeSurface(s);

    GS_ASSERT(this->filenames[index] == NULL);
    this->filenames[index] = (char*)malloc((strlen(filename) + 1) * sizeof(char));
    strcpy(this->filenames[index], filename);

    return index;
}

SpriteSheet TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface, SpriteSheet sheet_id)
{
    IF_ASSERT(surface == NULL) return NULL_SPRITE_SHEET;
    IF_ASSERT(sheet_id == NULL_SPRITE_SHEET) return NULL_SPRITE_SHEET;
    IF_ASSERT(sheet_id >= (SpriteSheet)N_SURFACES) return NULL_SPRITE_SHEET;
    
    surfaces[sheet_id] = surface;
    return sheet_id;
}

SpriteSheet TextureSheetLoader::load_texture_from_surface(struct SDL_Surface* surface)
{
    SpriteSheet index = this->load_texture_from_surface(surface, (SpriteSheet)this->surface_num);
    this->surface_num++;
    return index;
}

bool TextureSheetLoader::blit_meta(size_t meta_index)
{
    IF_ASSERT(meta_index >= this->tile_num) return false;

    struct TileMeta meta = this->meta[meta_index];

    // sanity checks
    GS_ASSERT(meta.sheet_id != NULL_SPRITE_SHEET);
    if (meta.sheet_id == NULL_SPRITE_SHEET) return NULL_SPRITE;
    GS_ASSERT(meta.sheet_id < (SpriteSheet)surface_num);
    if (meta.sheet_id >= (SpriteSheet)surface_num) return NULL_SPRITE;
    
    // get surface
    SDL_Surface* s = this->surfaces[meta.sheet_id];
    IF_ASSERT(s == NULL) return false;
    
    // check that tiles are in bounds
    GS_ASSERT(meta.xpos*tile_size < (size_t)s->w && meta.ypos*tile_size < (size_t)s->h);
    if (meta.xpos*tile_size >= (size_t)s->w || meta.ypos*tile_size >= (size_t)s->h)
        return false;

    // lock surfaces
    int s_lock = SDL_MUSTLOCK(s);
    int c_lock = SDL_MUSTLOCK(this->surface);

    if (s_lock) SDL_LockSurface(s);
    if (c_lock) SDL_LockSurface(this->surface);

    // alias pixel buffers
    Uint32* stack_pixels = (Uint32*)this->texture_stack;
    Uint32* sheet_pixels = (Uint32*)this->surface->pixels;

    // sprite coordinate index into destination pixel buffers
    size_t index = (meta_index % this->tiles_wide) + this->tiles_wide * (meta_index / this->tiles_wide);
    size_t dest_x = (index % this->tiles_wide) * this->tile_size;
    size_t dest_y = (index / this->tiles_wide) * this->tile_size;

    // copy sprite icon pixels from source pixel buffer to
    // sheet and stack buffers and unpacked pixel array
    for (size_t i=0; i<this->tile_size; i++)
    for (size_t j=0; j<this->tile_size; j++) 
    {
        size_t pix_index = s->w * (this->tile_size * meta.ypos + j) + (this->tile_size * meta.xpos + i);
        
        // convert source pixel to final format
        Uint32 pix = ((Uint32*)s->pixels)[pix_index];
        unsigned char r,g,b,a;
        SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);
        pix = SDL_MapRGBA(this->surface->format, r,g,b,a);
        
        size_t stack_index = this->tile_size * this->tile_size * index + (j * this->tile_size + i);
        stack_pixels[stack_index] = pix;
        size_t sheet_index = this->width * (dest_y + j) + (dest_x + i);
        sheet_pixels[sheet_index] = pix;

        size_t pixel_index = meta_index * this->tile_size * this->tile_size;
        pixel_index += (this->tile_size - j - 1) * this->tile_size + (this->tile_size - i - 1);
        this->pixels[pixel_index] = Color(r,g,b,a);
    }

    // unlock
    if (s_lock) SDL_UnlockSurface(s);
    if (c_lock) SDL_UnlockSurface(this->surface);

    return true;
}

//blit to sheet or return texture id
int TextureSheetLoader::blit(SpriteSheet sheet_id, size_t source_x, size_t source_y)
{
    IF_ASSERT(this->tile_num > 0xff) return NULL_SPRITE;
    IF_ASSERT(source_x < 1) return NULL_SPRITE;
    IF_ASSERT(source_y < 1) return NULL_SPRITE;

    // decrement x,y because arguments should be 1-indexed
    source_x--;
    source_y--;

    //check to see if already loaded
    for (size_t i=0; i<this->tile_num; i++)
        if (meta[i].sheet_id == sheet_id
         && meta[i].xpos == source_x
         && meta[i].ypos == source_y)
            return i;

    // record metadata
    size_t index = this->tile_num;
    meta[index].sheet_id = sheet_id;
    meta[index].xpos = source_x;
    meta[index].ypos = source_y;
    this->tile_num++;
    
    // blit
    if (!this->blit_meta(index)) return NULL_SPRITE;
    
    return index;
}

void TextureSheetLoader::generate_greyscale()
{
    static float gamma_correction[256];

    for (int i=0; i<256; i++)
    {
        float intensity = ((float) i) / 255.0f;
        gamma_correction[i] = powf(intensity, 1.0f/2.2f);
    }

    int c_lock = SDL_MUSTLOCK(surface);
    int s_lock = SDL_MUSTLOCK(greyscale_surface);

    if (c_lock) SDL_LockSurface(surface);
    if (s_lock) SDL_LockSurface(greyscale_surface);

    Uint8 r,g,b,a;

    for (size_t x=0; x<this->width; x++)
    for (size_t y=0; y<this->height; y++)
    {
        size_t index = y*this->width + x;
        GS_ASSERT(index < (size_t)greyscale_surface->w*greyscale_surface->h);
        Uint32 pix = ((Uint32*)surface->pixels)[index];
        SDL_GetRGBA(pix, surface->format, &r, &g, &b, &a);

        float avg = (gamma_correction[r] + gamma_correction[g] + gamma_correction[b]) / 3.0f;
        avg = powf(avg, 2.2f);
        GS_ASSERT(avg >= 0.0f && avg <= 1.0f);

        unsigned char g = (unsigned char)(255.0f*avg);

        ((Uint32*)greyscale_surface->pixels)[index] = SDL_MapRGBA(greyscale_surface->format, g,g,g,a);
    }

    if (c_lock) SDL_UnlockSurface(surface);
    if (s_lock) SDL_UnlockSurface(greyscale_surface);

    create_texture_from_surface(item_texture_sheet_loader->greyscale_surface,
        &this->greyscale_texture, this->mag_filter);
}

void TextureSheetLoader::generate_texture()
{
    create_texture_from_surface(this->surface, &this->texture, this->mag_filter);    
}

void TextureSheetLoader::reload()
{
    // remove existing loaded surfaces
    for (size_t i=0; i<this->surface_num; i++)
    {
        GS_ASSERT(this->surfaces[i] != NULL);
        if (this->surfaces[i] == NULL) continue;
        SDL_FreeSurface(this->surfaces[i]);
        this->surfaces[i] = NULL;
    }

    // regenerate surfaces from filenames
    for (size_t i=0; i<this->surface_num; i++)
    {
        GS_ASSERT(this->filenames[i] != NULL);
        if (this->filenames[i] == NULL) continue;
        printf("Reloading texture file: %s\n", this->filenames[i]);
        SDL_Surface* s = create_surface_from_file(this->filenames[i]);
        IF_ASSERT(s == NULL)
        {
            this->surface_num = i;
            break;
        }

        SpriteSheet index = this->load_texture_from_surface(s, (SpriteSheet)i);
        IF_ASSERT(index != (SpriteSheet)i)
        {
            this->surface_num = i;
            SDL_FreeSurface(s);
            break;
        }
    }

    // wipe out main surfaces
    MALLOX(SDL_Surface*, surfaces, 2);
    surfaces[0] = this->surface;
    surfaces[1] = this->greyscale_surface;
    for (int k=0; k<2; k++)
    {
        SDL_Surface* surface = surfaces[k];
        int lock = SDL_MUSTLOCK(surface);
        if (lock) SDL_LockSurface(surface);
        for (int i=0; i<this->surface->w*this->surface->h; i++)
            ((Uint32*)surface->pixels)[i] = SDL_MapRGBA(surface->format, 0,0,0,0);
        if (lock) SDL_UnlockSurface(surface);
    }

    // re-blit from tile meta
    for (size_t i=0; i<this->tile_num; i++) this->blit_meta(i);

    // destroy existing textures
    if (this->texture != 0) glDeleteTextures(1, &this->texture);
    if (this->greyscale_texture != 0) glDeleteTextures(1, &this->greyscale_texture);

    // regenerate textures
    this->generate_texture();
    this->generate_greyscale();
}

const Color* TextureSheetLoader::get_sprite_pixels(int sprite_id) const 
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)this->tile_num) return NULL;
    size_t index = sprite_id * this->tile_size * this->tile_size;
    return &this->pixels[index];
}

class TextureSheetLoader* cube_texture_sheet_loader = NULL;
class TextureSheetLoader* item_texture_sheet_loader = NULL;
class TextureSheetLoader* badge_texture_sheet_loader = NULL;

void init()
{
    GS_ASSERT(cube_texture_sheet_loader == NULL);
    GS_ASSERT(item_texture_sheet_loader == NULL);
    GS_ASSERT(badge_texture_sheet_loader == NULL);
    cube_texture_sheet_loader = new TextureSheetLoader(32);        //pixel size for cube textures
    item_texture_sheet_loader = new TextureSheetLoader(16);
    item_texture_sheet_loader->mag_filter = GL_NEAREST;
    badge_texture_sheet_loader = new TextureSheetLoader(16);
}

void init_item_texture()
{
    IF_ASSERT(item_texture_sheet_loader == NULL) return;
    item_texture_sheet_loader->generate_texture();
}

void init_greyscale()
{    
    item_texture_sheet_loader->generate_greyscale();
}

void teardown()
{
    if (cube_texture_sheet_loader  != NULL) delete cube_texture_sheet_loader;
    if (item_texture_sheet_loader  != NULL) delete item_texture_sheet_loader;
    if (badge_texture_sheet_loader != NULL) delete badge_texture_sheet_loader;
}

SpriteSheet load_cube_texture_sheet(const char* filename)
{
    return cube_texture_sheet_loader->load_texture(filename);
}

int blit_cube_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return cube_texture_sheet_loader->blit(sheet_id, source_x, source_y);
}

void save_cube_texture()
{
    save_surface_to_png(cube_texture_sheet_loader->surface, SCREENSHOT_PATH "cubes.png");
}

SpriteSheet cube_texture_alias(const char* filename)
{
    return cube_texture_sheet_loader->load_texture(filename);
}

//Item API
SpriteSheet item_texture_alias(const char* filename)
{
    return item_texture_sheet_loader->load_texture(filename);
}

SpriteSheet load_item_texture(struct SDL_Surface* surface)
{
    return item_texture_sheet_loader->load_texture_from_surface(surface);
}

int blit_item_texture(SpriteSheet sheet_id, int source_x, int source_y)
{
    return item_texture_sheet_loader->blit(sheet_id, source_x, source_y);
}

void save_item_texture()
{
    IF_ASSERT(item_texture_sheet_loader->surface == NULL) return;
    IF_ASSERT(item_texture_sheet_loader->greyscale_surface == NULL) return;

    save_surface_to_png(item_texture_sheet_loader->surface,
        SCREENSHOT_PATH "items.png");

    save_surface_to_png(item_texture_sheet_loader->greyscale_surface,
        SCREENSHOT_PATH "greyscale_items.png");
}

// badge api
void save_badge_texture()
{
    IF_ASSERT(badge_texture_sheet_loader->surface == NULL) return;

    save_surface_to_png(badge_texture_sheet_loader->surface,
        SCREENSHOT_PATH "badges.png");
}

SpriteSheet badge_texture_alias(const char* filename)
{
    return badge_texture_sheet_loader->load_texture(filename);
}

void init_badge_texture()
{
    badge_texture_sheet_loader->generate_texture();
}

void reload_texture_sheets()
{
    printf("Reloading texture sheets\n");
    cube_texture_sheet_loader->reload();
    t_map::blit_block_item_sheet();
    item_texture_sheet_loader->reload();
    Animations::load_sprite_voxelizer();
    t_map::reload_textures();
    badge_texture_sheet_loader->reload();
}

}   // TextureSheetLoader
