#include "sprite_voxelizer.hpp"

#include <animations/common.hpp>
#include <voxel/common_data.h>
#include <common/color.hpp>

namespace Animations
{

static class Shader sprite_voxelizer_shader;

static struct
{
    GLint xy;
    GLint color;
    GLint camera_pos;
    GLint pos;
    GLint rot_matrix;
} sprite_voxelizer_shader_vars;

const size_t SPRITE_VOXELIZER_MAX = MAX_ITEMS;
static VertexElementListSpriteVoxel* sprite_voxelizer_vlists[SPRITE_VOXELIZER_MAX] = {NULL}; 

static void generate_sprite_vertices(
    const class TextureSheetLoader::TextureSheetLoader* sheet_loader,
    int sprite_id)
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)SPRITE_VOXELIZER_MAX) return;
    const struct Color4* pixels = sheet_loader->get_sprite_pixels(sprite_id);
    IF_ASSERT(pixels == NULL) return;
    if (sprite_voxelizer_vlists[sprite_id] == NULL)
        sprite_voxelizer_vlists[sprite_id] = new VertexElementListSpriteVoxel;
    VertexElementListSpriteVoxel* vlist = sprite_voxelizer_vlists[sprite_id];
    vlist->clear();
    size_t tile_size = sheet_loader->tile_size;
    size_t n_pixels =  tile_size*tile_size;
    const unsigned char alpha_test = 0xFF/2;
    for (size_t i=0; i<n_pixels; i++)
    {   // build vertices from pixels
        if (pixels[i].a >= alpha_test) continue;
        float x = i % tile_size;
        float y = i / tile_size;
        vlist->push_vertex(x,y, color_init(pixels[i]));
    }
    // check if the sprite wasn't completely invisible
    GS_ASSERT(vlist->vlist_index > 0);
}

void load_sprite_voxelizer()
{   // generate vertex arrays from items in the spritesheet
    for (int i=0; i<MAX_ITEMS; i++)
    {
        class Item::ItemAttribute* attr = &Item::item_attributes[i];
        if (attr == NULL) continue;
        if (attr->particle_voxel) continue;
        generate_sprite_vertices(TextureSheetLoader::item_texture_sheet_loader, attr->sprite);
    }
}

void init_sprite_voxelizer()
{
    sprite_voxelizer_shader.set_debug(true);
    sprite_voxelizer_shader.load_shader("sprite voxelizer",
        "./media/shaders/animation/sprite_voxelizer.vsh",
        "./media/shaders/animation/sprite_voxelizer.fsh");

    sprite_voxelizer_shader_vars.xy = sprite_voxelizer_shader.get_attribute("InXY");
    sprite_voxelizer_shader_vars.color = sprite_voxelizer_shader.get_attribute("InColor");
    sprite_voxelizer_shader_vars.camera_pos = sprite_voxelizer_shader.get_uniform("InCameraPos");
    sprite_voxelizer_shader_vars.pos = sprite_voxelizer_shader.get_uniform("InPos");
    sprite_voxelizer_shader_vars.rot_matrix = sprite_voxelizer_shader.get_uniform("InRotMatrix");
}

void teardown_sprite_voxelizer()
{
    for (size_t i=0; i<SPRITE_VOXELIZER_MAX; i++)
        if (sprite_voxelizer_vlists[i] != NULL)
            delete sprite_voxelizer_vlists[i];
}

void draw_voxelized_sprite(int sprite_id)
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)SPRITE_VOXELIZER_MAX) return;
    IF_ASSERT(sprite_voxelizer_vlists[sprite_id] == NULL) return;

    sprite_voxelizer_vlists[sprite_id]->buffer();
    // TODO assign uniforms
}

}   // Animations
