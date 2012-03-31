#include "texture_sheet_loader.hpp"

namespace TextureSheetLoader
{





}

extern "C"
{
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
        if(c_lock) SDL_LockSurface(CubeTexture);

        Uint32* Pixels1 = (Uint32*) CubeTextureStack;
        Uint32* Pixels2 = (Uint32*) CubeTexture->pixels;

        int dest_x = index % 16;
        int dest_y = index / 16;

        for(int i=0; i < 32; i++)
        for(int j=0; j < 32; j++) 
        {
            pix = ((Uint32*) s->pixels)[ s->w*(j+32*source_y) + (i+32*source_x) ];
           	
           	Pixels1[ 32*32*index + (j*32+i) ] = pix;
            Pixels2[ 512*( (dest_y*32 + j) ) + (32*dest_x + i) ] = pix;
        }

        if(c_lock) SDL_UnlockSurface(CubeTexture);
        if(s_lock) SDL_UnlockSurface(s);

    }

	int LUA_load_cube_texture_sheet(char* filename)
	{
		return TextureSheetLoader::CubeTextureSheetList->load_texture(filename);
	}

	void LUA_save_cube_texture()
	{
		save_surface_to_png(TextureSheetLoader::CubeTexture, (char*) "./screenshot/cube_texture.png");
	}


}