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

    //blit to sheet or return texture id
    int blit(int sheet_id, int source_x, int source_y, int dest_index)
    {
        if(sheet_id >= texture_num)
        {
            printf("Error: TextureSheetList::blit error!!! FIX \n");
            return 255;
        }    
        //check to see if already loaded
        for(int i=0; i<tile_num; i++)
        {
            struct TileMeta m = meta[i];
            if(m.texture_sheet == sheet_id && m.xpos == source_x && m.ypos == source_y)
            {
                printf("!!! Sprite \n");
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
        Uint32* CubeTextureStack = (Uint32*) this->texture_stack;

        if( source_x* 16 >= s->w || source_y* 16 >= s->h )
        {
            printf("Error: LUA_blit_cube_texture, texture out of bounds \n");
            return 255;
        }

        int index = (dest_index % 16) + 16*(dest_index/16);
        Uint32 pix; 

        int s_lock = SDL_MUSTLOCK(s);
        int c_lock = SDL_MUSTLOCK( texture_sheet);

        if(s_lock) SDL_LockSurface(s);
        if(c_lock) SDL_LockSurface( texture_sheet);

        Uint32* Pixels1 = (Uint32*) CubeTextureStack;
        Uint32* Pixels2 = (Uint32*) texture_sheet->pixels;

        int dest_x = index % 16;
        int dest_y = index / 16;

        for(int i=0; i < 32; i++)
        for(int j=0; j < 32; j++) 
        {
            pix = ((Uint32*) s->pixels)[ s->w*(j+32*source_y) + (i+32*source_x) ];
            
            Pixels1[ 32*32*index + (j*32+i) ] = pix;
            Pixels2[ 512*( (dest_y*32 + j) ) + (32*dest_x + i) ] = pix;
        }

        if(c_lock) SDL_UnlockSurface( texture_sheet);
        if(s_lock) SDL_UnlockSurface(s);

        /*

        */
        return INDEX;
    }
};

TextureSheetList* CubeTextureSheetList = NULL;

static const int CUBE_TEXTURE_SIZE = 512;

SDL_Surface* CubeTexture = NULL;
unsigned int* CubeTextureStack = NULL;

void init()
{
    CubeTextureSheetList = new TextureSheetList(32);
    CubeTextureStack = CubeTextureSheetList->texture_stack;
    CubeTexture = CubeTextureSheetList->texture_sheet;
}

void teardown()
{
    delete CubeTextureSheetList;
}

}

extern "C"
{
    int LUA_load_cube_texture_sheet(char* filename) GNOMESCROLL_API; //LUA_API;

    void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index) GNOMESCROLL_API;

    void LUA_save_cube_texture() GNOMESCROLL_API;
}
