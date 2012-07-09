#pragma once

namespace TextureSheetLoader
{

const int N_SURFACES = 32;

class TextureSheetLoader
{
    public:

    int TILE_SIZE;
    int tile_dim; //x/y dimension of each tile

    int texture_num;
    int tile_num;
    
    SDL_Surface* surfaces[N_SURFACES]; //replace with surfaces

    struct TileMeta* meta;

    struct SDL_Surface* texture_sheet;  //for 2d array
    struct SDL_Surface* grey_scale_texture_sheet;  //for 2d array
    Uint32* texture_stack; //for 3d arrays


    TextureSheetLoader(int tile_size);
    ~TextureSheetLoader();

    int load_texture(char* filename);
    int load_texture_from_surface(struct SDL_Surface* surface);

    void reload_texture(int id, char* filename);

    //blit to sheet or return texture id
    int blit(int sheet_id, int source_x, int source_y);

    void generate_grey_scale();
};

extern class TextureSheetLoader* CubeTextureSheetLoader;
extern struct SDL_Surface* CubeSurface;
extern Uint32* CubeTextureStack;

extern class TextureSheetLoader* ItemTextureSheetLoader;
extern struct SDL_Surface* ItemSurface;
extern Uint32* ItemTextureStack;
extern GLuint ItemSheetTexture;

extern struct SDL_Surface* GreyScaleItemSurface;
extern GLuint GreyScaleItemTexture;

void init();
void init_greyscale();
void init_item_texture();

void teardown();

}


//cube texture sheet api
int LUA_load_cube_texture_sheet(char* filename);
int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y);
void LUA_save_cube_texture();

//item texture sheet api
int LUA_load_item_texture_sheet(char* filename);
int LUA_load_item_texture_sheet(struct SDL_Surface* surface);
int LUA_blit_item_texture(int sheet_id, int source_x, int source_y);
void LUA_save_item_texture();
