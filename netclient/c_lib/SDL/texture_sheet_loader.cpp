#include "texture_sheet_loader.hpp"

namespace TextureSheetLoader
{





}

extern "C"
{
    //returns destination index
    int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y)
    {
        return TextureSheetLoader::CubeTextureSheetList->blit(sheet_id, source_x, source_y);
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
