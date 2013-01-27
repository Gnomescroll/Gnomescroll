#include "texture.hpp"

#include <SDL/texture_loader.hpp>

namespace HudContainer
{

GLuint SynthesizerTexture = 0;
GLuint CraftingTexture = 0;
GLuint StorageBlockTexture = 0;
GLuint SmelterTexture = 0;
GLuint CrusherTexture = 0;
GLuint EquipmentTexture = 0;

static void init_synthesizer_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/synthesizer_4.png", &SynthesizerTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(SynthesizerTexture != 0);
}

static void init_crafting_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/crafting_bench.png", &CraftingTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(CraftingTexture != 0);
}

static void init_storage_block_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/storage_block.png", &StorageBlockTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(StorageBlockTexture != 0);
}

static void init_smelter_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/smelter.png", &SmelterTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(SmelterTexture != 0);
}

static void init_crusher_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/crusher.png", &CrusherTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(CrusherTexture != 0);
}

static void init_equipment_texture()
{
    GLuint min_filter = GL_LINEAR;
    GLuint mag_filter = GL_NEAREST;
    int ret = create_texture_from_file("media/sprites/container/equipment.png", &EquipmentTexture, min_filter, mag_filter);
    GS_ASSERT(ret == 0);
    GS_ASSERT(EquipmentTexture != 0);
}

void init_texture()
{
    init_synthesizer_texture();
    init_crafting_texture();
    init_storage_block_texture();
    init_smelter_texture();
    init_crusher_texture();
    init_equipment_texture();
}

void teardown_texture()
{
}

}   // HudContainer
