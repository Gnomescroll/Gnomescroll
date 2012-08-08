#pragma once

namespace TextureSheetLoader
{

class TextureSheetLoader
{
    public:

        unsigned int tile_size; // pixel dimension of each sprite/tile in sheet
        unsigned int texture_num;
        unsigned int tile_num;
        
        GLenum texture_format;

        char** filenames;
        SDL_Surface** surfaces;
        
        struct TileMeta* meta;

        struct SDL_Surface* texture_sheet;  //for 2d array
        struct SDL_Surface* grey_scale_texture_sheet;  //for 2d array
        Uint32* texture_stack; //for 3d arrays

        int load_texture(const char* filename);
        int load_texture_from_surface(struct SDL_Surface* surface);

        // blit to sheet and return texture id
        int blit(unsigned int sheet_id, int source_x, int source_y);

        void generate_grey_scale();

    explicit TextureSheetLoader(unsigned int tile_size);
    ~TextureSheetLoader();
};

extern class TextureSheetLoader* CubeTextureSheetLoader;
extern Uint32* CubeTextureStack;
extern struct SDL_Surface* CubeSurface;

extern class TextureSheetLoader* ItemTextureSheetLoader;
extern Uint32* ItemTextureStack;
extern struct SDL_Surface* ItemSurface;
extern struct SDL_Surface* GreyScaleItemSurface;
extern GLuint ItemSheetTexture;
extern GLuint GreyScaleItemTexture;

void init();
void init_greyscale();
void init_item_texture();
void teardown_item_texture();

void teardown();

}   // TextureSheetLoader

//cube texture sheet api
int LUA_load_cube_texture_sheet(const char* filename);
int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y);
void LUA_save_cube_texture();

//item texture sheet api
int LUA_load_item_texture_sheet(const char* filename);
int LUA_load_item_texture_sheet(struct SDL_Surface* surface);
int LUA_blit_item_texture(int sheet_id, int source_x, int source_y);
void LUA_save_item_texture();
