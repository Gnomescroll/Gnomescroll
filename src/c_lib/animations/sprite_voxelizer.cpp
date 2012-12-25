#include "sprite_voxelizer.hpp"

#include <animations/common.hpp>
#include <voxel/constants.hpp>
#include <common/color.hpp>

namespace Animations
{

static class Shader sprite_voxelizer_shader;

static struct
{
    // attributes
    GLint normal;
    GLint color;

    // uniforms
    GLint matrix;
} sprite_voxelizer_shader_vars;

const size_t SPRITE_VOXELIZER_MAX = MAX_ITEMS;
static VertexElementListColorByte* sprite_voxelizer_vlists[SPRITE_VOXELIZER_MAX] = {NULL}; 

static const unsigned char alpha_test = 0xFF/2;

static bool adjacent_pixel(const Color* pixels, size_t tile_size, int side, int a, int b)
{
    if (a <= 0 || a >= int(tile_size)-1)
        return false;
    if (b <= 0 || b >= int(tile_size)-1)
        return false;

    //is in plane; returns false for top/bottom
    if (_s_array[3*side+2])
        return false;   //this might be a bug/glassert

    int ia = a + _s_array[3*side+1];
    int ib = b + _s_array[3*side+0];

    if (ia < 0 || ia >= int(tile_size))
        return false;
    if (ib < 0 || ib >= int(tile_size))
        return false;

    return (pixels[ia + (ib * tile_size)].a > alpha_test);
}

static void push_sprite_vertex_cube(VertexElementListColorByte* vlist,
    const Color* pixels, size_t n_pixels, size_t tile_size, int index)
{
    static struct Vec3 veb[8] = {{{{0,0,0}}}};

    Color color = color_linearize(pixels[index]);

    const int x = 0;
    const int y = index % tile_size;
    const int z = index / tile_size;

    for (int i=0; i<8; i++)
    {
        veb[i].x = voxelized_sprite_config.scale*(v_set[3*i+0] + x);
        veb[i].y = voxelized_sprite_config.scale*(v_set[3*i+1] + y);
        veb[i].z = voxelized_sprite_config.scale*(v_set[3*i+2] + z);
    }

    for (int side=0; side<6; side++)
    {
        // skip occluded vertices
        if (adjacent_pixel(pixels, tile_size, (side+2)%6, y, z))
            continue;

        for (int j=0; j<4; j++)
            vlist->push_vertex(veb[q_set[4*side+j]], color, v_normal_b[side]);
    }
}

static void generate_sprite_vertices(
    const class TextureSheetLoader::TextureSheetLoader* sheet_loader,
    int sprite_id)
{
    IF_ASSERT(sprite_id < 0 || sprite_id >= (int)SPRITE_VOXELIZER_MAX) return;
    const Color* pixels = sheet_loader->get_sprite_pixels(sprite_id);
    IF_ASSERT(pixels == NULL) return;
    if (sprite_voxelizer_vlists[sprite_id] == NULL)
        sprite_voxelizer_vlists[sprite_id] = new VertexElementListColorByte;
    VertexElementListColorByte* vlist = sprite_voxelizer_vlists[sprite_id];
    vlist->clear();
    
    const size_t tile_size = sheet_loader->tile_size;
    const size_t n_pixels =  tile_size*tile_size;
    for (size_t i=0; i<n_pixels; i++)
        if (pixels[i].a > alpha_test)        
            push_sprite_vertex_cube(vlist, pixels, n_pixels, tile_size, i);

    // check if the sprite wasn't completely invisible
    IF_ASSERT(vlist->vlist_index <= 0)
    {
        printf("Sprite id %d in use but is invisible after alpha testing\n", sprite_id);
        return;
    }

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
    CHECK_GL_ERROR();
    
    sprite_voxelizer_shader.set_debug(true);
    sprite_voxelizer_shader.load_shader("sprite voxelizer",
        "./media/shaders/animation/sprite_voxelizer.vsh",
        "./media/shaders/animation/sprite_voxelizer.fsh");

    // attributes
    sprite_voxelizer_shader_vars.normal = sprite_voxelizer_shader.get_attribute("InNormal");

    // uniforms
    sprite_voxelizer_shader_vars.matrix = sprite_voxelizer_shader.get_uniform("InMatrix");

    CHECK_GL_ERROR();
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
    VertexElementListColorByte* vlist = sprite_voxelizer_vlists[sprite_id];
    IF_ASSERT(vlist->vertex_number == 0) return;

    glUseProgramObjectARB(sprite_voxelizer_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.normal);

    glUniformMatrix4fv(sprite_voxelizer_shader_vars.matrix, 1, GL_FALSE, rotation_matrix._f);

    size_t offset = 0;
    glVertexPointer(3, GL_FLOAT, vlist->stride, (GLvoid*)offset);
    offset += sizeof(struct Vec3);
    glColorPointer(3, GL_UNSIGNED_BYTE, vlist->stride, (GLvoid*)offset);
    offset += sizeof(Color);
    
    glVertexAttribPointer(sprite_voxelizer_shader_vars.normal, 3, GL_BYTE, GL_FALSE, vlist->stride, (GLvoid*)offset);
    offset += 4 * sizeof(char);

    GS_ASSERT(offset == sizeof(struct VertexElementColorNormalByte));
    
    glDrawArrays(GL_QUADS, 0, vlist->vertex_number);

    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.normal);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glUseProgramObjectARB(0);

    CHECK_GL_ERROR();
}

bool draw_voxelized_sprite_gl_begin(GLint cull_mode)
{
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face_mode);
    glCullFace(cull_mode);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    GL_ASSERT(GL_TEXTURE_2D, false);
    GL_ASSERT(GL_BLEND, false);
    //GL_ASSERT(GL_DEPTH_TEST, true);
    return true;
}

void draw_voxelized_sprite_gl_end()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    if (cull_face_mode != GL_INVALID_ENUM)
        glCullFace(cull_face_mode);
}

}   // Animations
