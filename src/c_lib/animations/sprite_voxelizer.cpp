#include "sprite_voxelizer.hpp"

#include <animations/common.hpp>
#include <voxel/common_data.h>
#include <common/color.hpp>

namespace Animations
{

static class Shader sprite_voxelizer_shader;

static struct
{
    GLint normal;
    GLint color;
    GLint camera_pos;
    GLint matrix;
} sprite_voxelizer_shader_vars;

const size_t SPRITE_VOXELIZER_MAX = MAX_ITEMS;
static VertexElementListColor* sprite_voxelizer_vlists[SPRITE_VOXELIZER_MAX] = {NULL}; 

static void push_sprite_vertex_cube(VertexElementListColor* vlist, float x, float y, const struct Color color)
{
    static struct Vec3 veb[8];     //vertex positions
    static struct Vec3 veb2[6*4];  //vertex array for rendering

    for (int i=0; i<8; i++)
    {
        veb[i].x = equipped_item_scale*v_set2[3*i+0] + x;
        veb[i].y = equipped_item_scale*v_set2[3*i+1] + y;
        veb[i].z = equipped_item_scale*v_set2[3*i+2];
    }

    // copy vertices into quad
    for (int i=0; i<6; i++)
    {
        veb2[4*i+0] = veb[q_set[4*i+0]];
        veb2[4*i+1] = veb[q_set[4*i+1]];
        veb2[4*i+2] = veb[q_set[4*i+2]];
        veb2[4*i+3] = veb[q_set[4*i+3]];
    }

    for (int i=0; i<6; i++)
    for (int j=0; j<4; j++)
        vlist->push_vertex(veb2[4*i+j], v_normal_vec3[i], color);
}

static void generate_sprite_vertices(
    const class TextureSheetLoader::TextureSheetLoader* sheet_loader,
    int sprite_id)
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)SPRITE_VOXELIZER_MAX) return;
    const struct Color4* pixels = sheet_loader->get_sprite_pixels(sprite_id);
    IF_ASSERT(pixels == NULL) return;
    if (sprite_voxelizer_vlists[sprite_id] == NULL)
        sprite_voxelizer_vlists[sprite_id] = new VertexElementListColor;
    VertexElementListColor* vlist = sprite_voxelizer_vlists[sprite_id];
    vlist->clear();
    
    const size_t tile_size = sheet_loader->tile_size;
    const size_t n_pixels =  tile_size*tile_size;
    const unsigned char alpha_test = 0xFF/2;
    for (size_t i=0; i<n_pixels; i++)
    {   // build vertices from pixels
        if (pixels[i].a <= alpha_test) continue;
        float x = float(i / tile_size);// / float(tile_size);
        float y = float(i % tile_size);// / float(tile_size);
        push_sprite_vertex_cube(vlist, x, y, color_init(pixels[i]));
    }
    // check if the sprite wasn't completely invisible
    IF_ASSERT(vlist->vlist_index <= 0)
        printf("Sprite id %d in use but is invisible after alpha testing\n", sprite_id);
    else
        vlist->buffer_static();
}

void load_sprite_voxelizer()
{   // generate vertex arrays from items in the spritesheet
    for (int i=0; i<MAX_ITEM_TYPES; i++)
    {
        class Item::ItemAttribute* attr = &Item::item_attributes[i];
        if (!attr->loaded || attr->particle_voxel) continue;
        generate_sprite_vertices(TextureSheetLoader::item_texture_sheet_loader, attr->sprite);
    }
}

void init_sprite_voxelizer()
{
    sprite_voxelizer_shader.set_debug(true);
    sprite_voxelizer_shader.load_shader("sprite voxelizer",
        "./media/shaders/animation/sprite_voxelizer.vsh",
        "./media/shaders/animation/sprite_voxelizer.fsh");

    sprite_voxelizer_shader_vars.normal = sprite_voxelizer_shader.get_attribute("InNormal");
    sprite_voxelizer_shader_vars.color = sprite_voxelizer_shader.get_attribute("InColor");
    sprite_voxelizer_shader_vars.camera_pos = sprite_voxelizer_shader.get_uniform("InCameraPos");
    sprite_voxelizer_shader_vars.matrix = sprite_voxelizer_shader.get_uniform("InMatrix");
}

void teardown_sprite_voxelizer()
{
    for (size_t i=0; i<SPRITE_VOXELIZER_MAX; i++)
        if (sprite_voxelizer_vlists[i] != NULL)
            delete sprite_voxelizer_vlists[i];
}

void draw_voxelized_sprite(int sprite_id, const struct Mat4& rotation_matrix)
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)SPRITE_VOXELIZER_MAX) return;
    IF_ASSERT(sprite_voxelizer_vlists[sprite_id] == NULL) return;
    IF_ASSERT(sprite_voxelizer_shader.shader == 0) return;
    VertexElementListColor* vlist = sprite_voxelizer_vlists[sprite_id];
    IF_ASSERT(vlist->vertex_number == 0) return;

    glUseProgramObjectARB(sprite_voxelizer_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.normal);
    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.color);

    struct Vec3 cpos = current_camera->get_position();
    glUniform3f(sprite_voxelizer_shader_vars.camera_pos, cpos.x, cpos.y, cpos.z);
    glUniformMatrix4fv(sprite_voxelizer_shader_vars.matrix, 1, GL_FALSE, rotation_matrix._f);

    size_t offset = 0;
    glVertexPointer(3, GL_FLOAT, vlist->stride, (GLvoid*)offset);
    offset += 3 * sizeof(GL_FLOAT);    
    glVertexAttribPointer(sprite_voxelizer_shader_vars.normal, 3, GL_FLOAT, GL_FALSE, vlist->stride, (GLvoid*)offset);
    offset += 3 * sizeof(GL_FLOAT);
    glVertexAttribPointer(sprite_voxelizer_shader_vars.color, 3, GL_UNSIGNED_BYTE, GL_TRUE, vlist->stride, (GLvoid*)offset);
    
    glDrawArrays(GL_QUADS, 0, vlist->vertex_number);

    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.normal);
    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.color);
    glDisableClientState(GL_VERTEX_ARRAY);
    glUseProgramObjectARB(0);

    CHECK_GL_ERROR();
}

bool draw_voxelized_sprite_gl_begin()
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    GL_ASSERT(GL_TEXTURE_2D, false);
    GL_ASSERT(GL_BLEND, false);
    return true;
}

void draw_voxelized_sprite_gl_end()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

}   // Animations
