#pragma once

namespace TextureSheetLoader
{

#include <c_lib/SDL/texture_loader.hpp>

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
		if(surface != NULL) SDL_FreeSurface(surface);
	}
};

const int TEXTURE_SHEET_LIST_MAX = 64;

class TextureSheetList
{
	public:

	int tile_dim; //x/y dimension of each tile

	int texture_num;
	TextureSheet* textures[TEXTURE_SHEET_LIST_MAX];

	TextureSheetList()
	{
		for(int i=0; i<TEXTURE_SHEET_LIST_MAX; i++) textures[i] = NULL;
		texture_num=0;
	}

	int load_texture(char* filename)
	{
		//load texture
		if(texture_num == TEXTURE_SHEET_LIST_MAX)
		{
			printf("TextureSheetList Error: max texture sheets reached\n");
			return -1;
		}

		int id = texture_num;

		textures[texture_num] = new TextureSheet();
		textures[texture_num]->surface = _load_image(filename);

		if(textures[texture_num]->surface == 0)
		{
			printf("TextureSheetList: loading spritesheet %s failed \n", filename);
			return 0;
		}

		texture_num++;
		return id;

	}

	int reload_texture(int id, char* filename)
	{
		delete textures[id];

		textures[id] = new TextureSheet();
		textures[id]->surface = _load_image(filename);

		if(textures[texture_num]->surface == 0)
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
	CubeTextureSheetList = new TextureSheetList;
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

	int LUA_load_cube_texture_sheet(char* filename)
	{
		return TextureSheetLoader::CubeTextureSheetList->load_texture(filename);
	}


	void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index) GNOMESCROLL_API;

	void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index)
	{
		//const int tex_size = 32;

		//blit texture stat for stack

		SDL_Surface* s = TextureSheetLoader::CubeTextureSheetList->textures[sheet_id]->surface;
		
		SDL_Surface* CubeTexture = TextureSheetLoader::CubeTexture;
		Uint32* CubeTextureStack = (Uint32*) TextureSheetLoader::CubeTextureStack;

		if( source_x* 16 >= s->w || source_y* 16 >= s->h )
		{
			printf("Error: LUA_blit_cube_texture, texture out of bounds \n");
			return;
		}

        int index = (dest_index % 16) + 16*(dest_index/16);
        Uint32 pix; 

        int s_lock = SDL_MUSTLOCK(s);
        int c_lock = SDL_MUSTLOCK(CubeTexture);

        if(s_lock) SDL_LockSurface(s);

        Uint32* Pixels = (Uint32*)CubeTextureStack;

        for(int i=0; i < 32; i++)
        for(int j=0; j < 32; j++) 
        {
            pix = ((Uint32*) s->pixels)[ s->w*(j+32*source_y) + (i+32*source_x) ];
            Pixels[ 32*32*index + (j*32+i) ] = pix;
        }


        if(c_lock) SDL_LockSurface(CubeTexture);

        Pixels = (Uint32*) CubeTexture->pixels;

        int dest_x = index % 16;
        int dest_y = index / 16;
        for(int i=0; i < 32; i++)
        for(int j=0; j < 32; j++) 
        {
            pix = ((Uint32*) s->pixels)[ s->w*(j+32*source_y) + (i+32*source_x) ];
            Pixels[ 512*(dest_y*32 + j) + (32*dest_x + i) ] = pix;
        }

        if(c_lock) SDL_UnlockSurface(CubeTexture);
        if(s_lock) SDL_UnlockSurface(s);

    }

	void LUA_save_cube_texture() GNOMESCROLL_API;

	void LUA_save_cube_texture()
	{
		save_surface_to_png(TextureSheetLoader::CubeTexture, (char*) "./screenshot/cube_texture.png");
	}


}