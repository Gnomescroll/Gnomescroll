#include "texture.hpp"

#include <SDL/texture_loader.hpp>

namespace t_hud
{

//SDL_Surface* ItemGridSlotSurface = NULL;
//GLuint ItemGridSlotTexture = 0;

GLuint NaniteTexture = 0;
GLuint CraftingTexture = 0;
GLuint StorageBlockTexture = 0;
GLuint SmelterTexture = 0;

void init_nanite_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/nanite_4.png", &NaniteTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(NaniteTexture != 0);
}

void init_crafting_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/crafting_bench.png", &CraftingTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(CraftingTexture != 0);
}

void init_storage_block_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/storage_block.png", &StorageBlockTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(StorageBlockTexture != 0);
}

void init_smelter_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file((char*) "media/sprites/smelter.png", &SmelterTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(SmelterTexture != 0);
}

void init_texture()
{
    init_nanite_texture();
    init_crafting_texture();
    init_storage_block_texture();
    init_smelter_texture();
}

void teardown_texture()
{
}

}
