#pragma once

namespace TextureSheetLoader
{

#include <SDL/texture_loader.hpp>

struct TileMeta
{
    int texture_sheet;
    int xpos;
    int ypos;
};
/*
class TextureSheet
{
    public:
    SDL_Surface* surface;
    GLuint id; //opengl

    TextureSheet()
    {
        id = 0;
        surface = NULL;
    }

    ~TextureSheet()
    {
        if (surface != NULL)
            SDL_FreeSurface(surface);
    }
};
*/
const int TEXTURE_SHEET_LIST_MAX = 64;

class TextureSheetList
{
    public:

    int TILE_SIZE;
    int tile_dim; //x/y dimension of each tile

    int texture_num;
    int tile_num;
    
    SDL_Surface* textures[TEXTURE_SHEET_LIST_MAX]; //replace with surfaces

    struct TileMeta meta[256];

    SDL_Surface* texture_sheet;  //for 2d array
    unsigned int* texture_stack; //for 3d arrays

    TextureSheetList(int tile_size)
    {
        TILE_SIZE = tile_size;
        for(int i=0; i<TEXTURE_SHEET_LIST_MAX; i++) textures[i] = NULL;
        texture_num=0;
        tile_num=0;

        texture_sheet = create_surface_from_nothing(16*TILE_SIZE, 16*TILE_SIZE);
        texture_stack = (unsigned int*) malloc(256*TILE_SIZE*TILE_SIZE);
    }

    ~TextureSheetList()
    {
        for (int i=0; i<TEXTURE_SHEET_LIST_MAX; i++)
        {
            if (this->textures[i] != NULL) SDL_FreeSurface(textures[i]);
        }
    }

    int load_texture(char* filename)
    {
        int id = texture_num;

        textures[texture_num]= _load_image(filename);

        if(textures[texture_num] == NULL)
        {
            printf("TextureSheetList: loading spritesheet %s failed \n", filename);
            return 0;
        }

        texture_num++;
        return id;
    }

    int reload_texture(int id, char* filename)
    {
        SDL_FreeSurface(textures[id]);
        textures[id] = _load_image(filename);

        if(textures[texture_num] == NULL)
        {
            printf("TextureSheetList: reloading spritesheet %s failed \n", filename);
            return 0;
        }
    }

    void blit(SDL_Surface* destination, int tile_dim, int tile_xindex, int tile_yindex, int texture_sheet_index)
    {
        //TextureSheet* ts = textures[texture_sheet_index];


    }
};

TextureSheetList* CubeTextureSheetList = NULL;
//TextureSheetList CubeSelectorTextureSheetList;

static const int CUBE_TEXTURE_SIZE = 512;

SDL_Surface* CubeTexture = NULL;
unsigned int* CubeTextureStack = NULL;

void init()
{
    CubeTextureSheetList = new TextureSheetList(32);
    CubeTextureStack = (unsigned int*) malloc(4*CUBE_TEXTURE_SIZE*CUBE_TEXTURE_SIZE);
    CubeTexture = create_surface_from_nothing(CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE);
}

void teardown()
{
    delete CubeTextureSheetList;
    SDL_FreeSurface(CubeTexture);
    free(CubeTextureStack);
}

}

extern "C"
{
    int LUA_load_cube_texture_sheet(char* filename) GNOMESCROLL_API; //LUA_API;

    void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index) GNOMESCROLL_API;

    void LUA_save_cube_texture() GNOMESCROLL_API;
}
