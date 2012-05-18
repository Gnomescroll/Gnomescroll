#pragma once

namespace TextureSheetLoader
{

class TextureSheetLoader
{
    public:

    int TILE_SIZE;
    int tile_dim; //x/y dimension of each tile

    int texture_num;
    int tile_num;
    
    SDL_Surface* textures[32]; //replace with surfaces

    struct TileMeta* meta;

    struct SDL_Surface* texture_sheet;  //for 2d array
    struct SDL_Surface* grey_scale_texture_sheet;  //for 2d array
    Uint32* texture_stack; //for 3d arrays


    TextureSheetLoader(int tile_size);
    ~TextureSheetLoader();

    int load_texture(char* filename);
    void reload_texture(int id, char* filename);

    //blit to sheet or return texture id
    int blit(int sheet_id, int source_x, int source_y);

    void generate_grey_scale();
};

extern class TextureSheetLoader* CubeTextureSheetLoader;
extern struct SDL_Surface* CubeTexture;
extern Uint32* CubeTextureStack;

extern class TextureSheetLoader* ItemTextureSheetLoader;
extern struct SDL_Surface* ItemSurface;
extern Uint32* ItemTextureStack;

extern struct SDL_Surface* GreyScaleItemSurface;
extern Uint32 GreyScaleItemTexture;

void init();
void init_greyscale();

void teardown();

}


extern "C"
{
    //cube texture sheet api
    int LUA_load_cube_texture_sheet(char* filename) GNOMESCROLL_API; //LUA_API;
    int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y) GNOMESCROLL_API;
    void LUA_save_cube_texture() GNOMESCROLL_API;

    //item texture sheet api
    int LUA_load_item_texture_sheet(char* filename) GNOMESCROLL_API; //LUA_API;
    int LUA_blit_item_texture(int sheet_id, int source_x, int source_y) GNOMESCROLL_API;
    void LUA_save_item_texture() GNOMESCROLL_API;
}
