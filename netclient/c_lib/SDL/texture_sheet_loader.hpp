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

SDL_Surface* CubeTextures = NULL;
unsigned int* CubeTextureStack = NULL;

void init()
{
	CubeTextureSheetList = new TextureSheetList;
	CubeTextureStack = (unsigned int*) malloc(4*CUBE_TEXTURE_SIZE*CUBE_TEXTURE_SIZE);
	CubeTextures = create_surface_from_nothing(CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE);
}

void teardown()
{
	delete CubeTextureSheetList;
	SDL_FreeSurface(CubeTextures);
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

	void LUA_save_cube_texture() GNOMESCROLL_API;

	void LUA_save_cube_texture()
	{
		
		
	}
}