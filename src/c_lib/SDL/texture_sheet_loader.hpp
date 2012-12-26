#pragma once

#if DC_CLIENT

#include <common/color.hpp>
#include <SDL/constants.hpp>

namespace TextureSheetLoader
{

class TextureSheetLoader
{
    private:
        bool blit_meta(size_t meta_index);
        
    public:

        struct TileMeta
        {
            SpriteSheet sheet_id;
            size_t xpos;
            size_t ypos;
        };

        size_t width;
        size_t height;

        size_t tile_size; // pixel dimension of each sprite/tile in sheet
        size_t tiles_wide;
        size_t tiles_high;
        
        size_t surface_num;
        size_t tile_num;
        
        GLenum format;

        char** filenames;
        SDL_Surface** surfaces;
        
        struct TileMeta* meta;

        struct SDL_Surface* surface;  //for 2d array
        struct SDL_Surface* greyscale_surface;  //for 2d array
        Uint32* texture_stack; //for 3d arrays

        GLuint texture;
        GLuint greyscale_texture;

        GLenum mag_filter;

        Color* pixels;

        SpriteSheet load_texture(const char* filename);
        SpriteSheet load_texture_from_surface(struct SDL_Surface* surface);
        SpriteSheet load_texture_from_surface(struct SDL_Surface* surface, SpriteSheet sheet_id);

        // blit to sheet and return texture id
        int blit(SpriteSheet sheet_id, size_t source_x, size_t source_y);

        void generate_greyscale();

        void reload();
        void generate_texture();

        void save_texture(const char* filename);

        const Color* get_sprite_pixels(int sprite_id) const;

    explicit TextureSheetLoader(size_t tile_size);
    ~TextureSheetLoader();
};

extern class TextureSheetLoader* cube_texture_sheet_loader;
extern class TextureSheetLoader* item_texture_sheet_loader;
extern class TextureSheetLoader* badge_texture_sheet_loader;

void init();
void init_greyscale();
void init_item_texture();

void teardown();

//cube texture sheet api
SpriteSheet load_cube_texture_sheet(const char* filename);
int blit_cube_texture(SpriteSheet sheet_id, int source_x, int source_y);
void save_cube_texture();
SpriteSheet cube_texture_alias(const char* filename);

//item texture sheet api
SpriteSheet item_texture_alias(const char* filename);
int blit_item_texture(SpriteSheet sheet_id, int source_x, int source_y);
void save_item_texture();

// badge api
void save_badge_texture();
SpriteSheet badge_texture_alias(const char* filename);

void reload_texture_sheets();

}   // TextureSheetLoader

#endif

#if DC_SERVER
namespace TextureSheetLoader
{

// This is allowed for the server, so the dat configs can be shared between
// client and server

SpriteSheet _texture_alias(const char* spritesheet_filename)
{
    return (SpriteSheet)0;
}

#define cube_texture_alias _texture_alias
#define item_texture_alias _texture_alias
#define badge_texture_alias _texture_alias

}   // TextureSheetLoader
#endif
