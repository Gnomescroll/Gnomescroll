#include "texture.hpp"

#include <SDL/texture_loader.hpp>

namespace t_hud
{

GLuint SynthesizerTexture = 0;
GLuint CraftingTexture = 0;
GLuint StorageBlockTexture = 0;
GLuint SmelterTexture = 0;
GLuint RecyclerTexture = 0;

static void init_synthesizer_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/synthesizer_4.png", &SynthesizerTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(SynthesizerTexture != 0);
}

static void init_crafting_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/crafting_bench.png", &CraftingTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(CraftingTexture != 0);
}

static void init_storage_block_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/storage_block.png", &StorageBlockTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(StorageBlockTexture != 0);
}

static void init_smelter_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/smelter.png", &SmelterTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(SmelterTexture != 0);
}

static void init_recycler_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/recycler.png", &RecyclerTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(RecyclerTexture != 0);
}

void init_texture()
{
    init_synthesizer_texture();
    init_crafting_texture();
    init_storage_block_texture();
    init_smelter_texture();
    init_recycler_texture();
}

void teardown_texture()
{
}

}   // t_hud
