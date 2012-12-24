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
    GLint ao_matrix;
    GLint ao_interpolate;

    // uniforms
    //GLint camera_pos;
    GLint matrix;
} sprite_voxelizer_shader_vars;

const size_t SPRITE_VOXELIZER_MAX = MAX_ITEMS;
static VertexElementListColorByteAO* sprite_voxelizer_vlists[SPRITE_VOXELIZER_MAX] = {NULL}; 

static const unsigned char alpha_test = 0xFF/2;

// these are x,y deltas surrounding a face
// https://github.com/Charged/Miners/blob/master/src/miners/builder/helpers.d#L344
static const int cdirs[8][2] = {
    {-1,-1,},  // c1
    { 0,-1,},
    { 1,-1,},
    {-1, 0,},
    { 1, 0,},
    {-1, 1,},
    { 0, 1,},
    { 1, 1,},
    };

// these indicate whether we need to bother checking an adjacent pixel
static const int pdirs[3][8] = {
    { 0, 1, 0, 0, 0, 0, 1, 0, }, // top,bottom
    { 0, 1, 0, 0, 0, 0, 1, 0, }, // east,west
    { 1, 1, 1, 1, 1, 1, 1, 1, }, // north,south
    };


static const int_fast8_t CI[6*8*3] = {1, 1, 1, 0, 1, 1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1,
-1, 1, -1, 0, 1, -1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1,
1, -1, 1, 1, -1, 0, 1, -1, -1, 1, 0, -1, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 0, 1,
-1, 1, 1, -1, 1, 0, -1, 1, -1, -1, 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, 1, -1, 0, 1,
1, 1, 1, 1, 1, 0, 1, 1, -1, 0, 1, -1, -1, 1, -1, -1, 1, 0, -1, 1, 1, 0, 1, 1,
-1, -1, 1, -1, -1, 0, -1, -1, -1, 0, -1, -1, 1, -1, -1, 1, -1, 0, 1, -1, 1, 0, -1, 1 };


static inline int calcAdj(int side_1, int side_2, int corner) 
{
    const static int occ_array[3] = { 255, 128, 64 };
    int occ = (side_1 | side_2 | corner) + (side_1 & side_2);
    return occ_array[occ];
}

const static int_fast8_t _s_array[3*6] = 
    {
        0,0,1,  //top
        0,0,-1, //bottom
        1,0,0,  //north
        -1,0,0, //south
        0,1,0,  //west
        0,-1,0, //east
    };

static bool adjacent_pixel(int side, int ix, int iy, int tile_size, const Color* pixels)
{

    if(ix-1 < 0 || ix+1 >= tile_size)
        return false;
    if(iy-1 < 0 || iy+1 >= tile_size)
        return false;

    //is in plane; retursn false for top/bottom
    if(_s_array[3*side+2] != 0)
        return false;   //this might be a bug/glassert

    int _ix = ix + _s_array[3*side+0];
    int _iy = iy + _s_array[3*side+1];

    if(_ix < 0 || _ix >= tile_size)
        return false;
    if(_iy < 0 || _iy >= tile_size)
        return false;

    if(pixels[ _ix + (_iy * tile_size) ].a > alpha_test)    
        return true;
    return false;

}

static bool isOccludes(int ix, int iy, int iz, int tile_size, const Color* pixels)
{
    if(iz != 0)
        return false;
    if(ix < 0 || ix >= tile_size)
        return false;
    if(iy < 0 || iy >= tile_size)
        return false;

    if(pixels[ ix + (iy * tile_size) ].a > alpha_test)    
        return false;
    return true; 

}

static void push_sprite_vertex_cube(VertexElementListColorByteAO* vlist,
    const Color* pixels, size_t n_pixels, size_t tile_size, int index)
{
    static struct Vec3 veb[8] = {{{{0,0,0}}}};
    static char ao[4] = {0};
    //static int neighbors[8] = {0};

    Color color = color_linearize(pixels[index]);
    
    int a = index % tile_size;
    int b = index / tile_size;

    float x = float((n_pixels-index-1) % tile_size);
    float y = float((n_pixels-index-1) / tile_size);
    
    for (int i=0; i<8; i++)
    {
        veb[i].x = voxelized_sprite_config.scale*(v_set[3*i+0] + x);
        veb[i].y = voxelized_sprite_config.scale*(v_set[3*i+1] + y);
        veb[i].z = voxelized_sprite_config.scale*(v_set[3*i+2] + 0);
    }

    for (int side=0; side<6; side++)
    {

        //static bool adjacent_pixel(int side, int ix, int iy, int tile_size, const Color* pixels)

        //dont draw occluded vertices
        if(adjacent_pixel(side, a,b,tile_size,pixels))
            continue;

        int CX[8];
        for(int i=0; i<8; i++) 
        {
            int index = side*8*3+i*3;
            CX[i] = isOccludes(x+CI[index+0],y+CI[index+1],CI[index+2],tile_size,pixels);
        }


        ao[0] = calcAdj(CX[7], CX[1], CX[0]);
        ao[1] = calcAdj(CX[5], CX[7], CX[6]);
        ao[2] = calcAdj(CX[1], CX[3], CX[2]);
        ao[3] = calcAdj(CX[3], CX[5], CX[4]);


/*
        for (int j=0; j<8; j++)
        {
            neighbors[j] = pdirs[i/2][j];
            if (neighbors[j] && a > 0 && a < int(tile_size)-1 && b > 0 && b < int(tile_size)-1)
            {
                int m = a - cdirs[j][0];
                int n = b - cdirs[j][1];
                int k = m + (n * tile_size);
                neighbors[j] = int(pixels[k].a > alpha_test);
            }
        }
*/

/*
        ao[0] = Voxels::get_ao_weight(neighbors[7], neighbors[1], neighbors[0]);
        ao[1] = Voxels::get_ao_weight(neighbors[5], neighbors[7], neighbors[6]);
        ao[2] = Voxels::get_ao_weight(neighbors[1], neighbors[3], neighbors[2]);
        ao[3] = Voxels::get_ao_weight(neighbors[3], neighbors[5], neighbors[4]);
*/

        for (int j=0; j<4; j++)
            vlist->push_vertex(veb[q_set[4*side+j]], color, v_normal_b[side], ao, voxel_tex_array[j].tex);
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
        sprite_voxelizer_vlists[sprite_id] = new VertexElementListColorByteAO;
    VertexElementListColorByteAO* vlist = sprite_voxelizer_vlists[sprite_id];
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
    sprite_voxelizer_shader_vars.ao_matrix = sprite_voxelizer_shader.get_attribute("InAOMatrix");
    sprite_voxelizer_shader_vars.ao_interpolate = sprite_voxelizer_shader.get_attribute("InAOInterpolate");

    // uniforms
    //sprite_voxelizer_shader_vars.camera_pos = sprite_voxelizer_shader.get_uniform("InCameraPos");
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
    VertexElementListColorByteAO* vlist = sprite_voxelizer_vlists[sprite_id];
    IF_ASSERT(vlist->vertex_number == 0) return;

    glUseProgramObjectARB(sprite_voxelizer_shader.shader);

    glBindBuffer(GL_ARRAY_BUFFER, vlist->VBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.normal);
    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.ao_matrix);
    glEnableVertexAttribArray(sprite_voxelizer_shader_vars.ao_interpolate);

    //struct Vec3 cpos = current_camera->get_position();
    //glUniform3f(sprite_voxelizer_shader_vars.camera_pos, cpos.x, cpos.y, cpos.z);
    glUniformMatrix4fv(sprite_voxelizer_shader_vars.matrix, 1, GL_FALSE, rotation_matrix._f);

    size_t offset = 0;
    glVertexPointer(3, GL_FLOAT, vlist->stride, (GLvoid*)offset);
    offset += sizeof(struct Vec3);
    glColorPointer(3, GL_UNSIGNED_BYTE, vlist->stride, (GLvoid*)offset);
    offset += sizeof(Color);
    
    glVertexAttribPointer(sprite_voxelizer_shader_vars.normal, 3, GL_BYTE, GL_FALSE, vlist->stride, (GLvoid*)offset);
    offset += 4 * sizeof(char);
    glVertexAttribPointer(sprite_voxelizer_shader_vars.ao_matrix, 4, GL_UNSIGNED_BYTE, GL_TRUE, vlist->stride, (GLvoid*)offset);
    offset += 4 * sizeof(char);
    glVertexAttribPointer(sprite_voxelizer_shader_vars.ao_interpolate, 4, GL_UNSIGNED_BYTE, GL_FALSE, vlist->stride, (GLvoid*)offset);
    offset += 4 * sizeof(char);

    GS_ASSERT(offset == sizeof(struct VertexElementColorNormalByteAO));
    
    glDrawArrays(GL_QUADS, 0, vlist->vertex_number);

    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.normal);
    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.ao_matrix);
    glDisableVertexAttribArray(sprite_voxelizer_shader_vars.ao_interpolate);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glUseProgramObjectARB(0);

    CHECK_GL_ERROR();
}

bool draw_voxelized_sprite_gl_begin()
{
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
}

}   // Animations
