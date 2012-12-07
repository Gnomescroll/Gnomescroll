#include "sprite_voxelizer.hpp"

#include <voxel/common_data.h>
#include <common/color.hpp>

namespace Animations
{


static class Shader sprite_voxelizer_shader;

static struct
{
    GLint normal;
    GLint camera_pos;
    GLint color;
} sprite_voxelizer_shader_vars;

static VertexElementListColor* sprite_voxelizer_vlist = NULL; 

void init_sprite_voxelizer()
{
    sprite_voxelizer_shader.set_debug(true);
    sprite_voxelizer_shader.load_shader("sprite voxelizer",
        "./media/shaders/animation/sprite_voxelizer.vsh",
        "./media/shaders/animation/sprite_voxelizer.fsh");

    sprite_voxelizer_shader_vars.normal = sprite_voxelizer_shader.get_attribute("InNormal");
    sprite_voxelizer_shader_vars.color = sprite_voxelizer_shader.get_attribute("InColor");
    sprite_voxelizer_shader_vars.camera_pos = sprite_voxelizer_shader.get_uniform("InCameraPos");

    GS_ASSERT(sprite_voxelizer_vlist == NULL);
    sprite_voxelizer_vlist = new VertexElementListColor;
}

void teardown_sprite_voxelizer()
{
    if (sprite_voxelizer_vlist != NULL) delete sprite_voxelizer_vlist;
}

void prep_sprite_voxelizer()
{
    //GS_ASSERT(sprite_voxelizer_vlist != NULL);
    //if (sprite_voxelizer_vlist == NULL) return;

    //// animation/weapon.cpp should push the sprites here
    //// a single method will add the sprite to the shader list
    
    //class Particle::ColoredMinivox_list* list = Particle::colored_minivox_list;
    //GS_ASSERT(list != NULL);
    //if (list == NULL)
    //{
        //sprite_voxelizer_vlist->vertex_number = 0;
        //return;
    //}
    
    //if (list->n_max <= 0 || list->num <= 0)
    //{
        //sprite_voxelizer_vlist->vertex_number = 0;
        //return;
    //}

    //static struct Vec3 veb[8];     //vertex positions
    //static struct Vec3 vn[6];      //normals
    //static struct Vec3 veb2[6*4];  //vertex array for rendering

    //for (unsigned int i=0; i<list->num; i++)
    //{
        //Particle::ColoredMinivox* vox = &list->a[i]; 
        //vox->voxel.delta_rotation();
        
        //// frustum test
        //float size = vox->voxel.size;
        //struct Vec3 p = quadrant_translate_position(current_camera_position, vox->verlet.position);
        //struct Vec3 center = p;
        //center.x += size;
        //center.y += size;
        //if (!sphere_fulstrum_test(center.x, center.y, center.z, size)) continue;

        //for (int i=0; i<8; i++)
        //{
            //veb[i].x = size*v_set2[3*i+0];
            //veb[i].y = size*v_set2[3*i+1];
            //veb[i].z = size*v_set2[3*i+2];
        //}

        ////rotate normals
        //for (int i=0; i<6; i++)
        //{
            //vn[i] = vec3_apply_rotation(v_normal_vec3[i], vox->voxel.rotation_matrix);   //rotate
        //}

        ////rotate and translate cube vertices
        //for (int i=0; i<8; i++)
        //{
            //veb[i] = vec3_apply_rotation(veb[i], vox->voxel.rotation_matrix);  //rotate
            //veb[i] = vec3_add(p, veb[i]);                           //translate
        //}

        ////copy vertices into quad
        //for (int i=0; i<6; i++)
        //{
            //veb2[4*i+0] = veb[q_set[4*i+0]];
            //veb2[4*i+1] = veb[q_set[4*i+1]];
            //veb2[4*i+2] = veb[q_set[4*i+2]];
            //veb2[4*i+3] = veb[q_set[4*i+3]];
        //}

        //// draw voxel
        //for (int i=0; i<6; i++)
        //{
            //sprite_voxelizer_vlist->push_vertex(veb2[4*i+0], vn[i], vox->color);
            //sprite_voxelizer_vlist->push_vertex(veb2[4*i+1], vn[i], vox->color);
            //sprite_voxelizer_vlist->push_vertex(veb2[4*i+2], vn[i], vox->color);
            //sprite_voxelizer_vlist->push_vertex(veb2[4*i+3], vn[i], vox->color);
        //}
    //}
    
    //sprite_voxelizer_vlist->buffer();
}

void draw_sprite_voxelizer()
{
    if (sprite_voxelizer_vlist == NULL) return;
    
    sprite_voxelizer_vlist->buffer();

    
}

static inline void push_pixel_voxel(const struct Color pixel,
    struct Vec3 origin,
    struct Vec3 forward, struct Vec3 right, struct Vec3 up,
    float scale)
{
    //static struct Vec3 veb[8];     //vertex positions
    static struct Vec3 vn[6];      //normals
    static struct Vec3 veb2[6*4];  //vertex array for rendering

    //for (int i=0; i<8; i++)
    //{
        //veb[i].x = scale*v_set2[3*i+0];
        //veb[i].y = scale*v_set2[3*i+1];
        //veb[i].z = scale*v_set2[3*i+2];
    //}

    for (int i=0; i<6; i++)
    {
        sprite_voxelizer_vlist->push_vertex(veb2[4*i+0], vn[i], pixel);
        sprite_voxelizer_vlist->push_vertex(veb2[4*i+1], vn[i], pixel);
        sprite_voxelizer_vlist->push_vertex(veb2[4*i+2], vn[i], pixel);
        sprite_voxelizer_vlist->push_vertex(veb2[4*i+3], vn[i], pixel);
    }
}

// WARNING: this function does not do fulstrum checks 
void voxelize_sprite(
    const class TextureSheetLoader::TextureSheetLoader* sheet_loader,
    int sprite_index,
    struct Vec3 origin, struct Vec3 forward, struct Vec3 right, struct Vec3 up,
    float scale)
{
    GS_ASSERT(sprite_index >= 0);
    if (sprite_index < 0) return;
    
    const unsigned char alpha_test = 0xFF/2;

    size_t index = sprite_index * sheet_loader->tile_size * sheet_loader->tile_size;
    const struct Color4* pixels = sheet_loader->pixels;
    const float delta = scale/(float)sheet_loader->tile_size;
    
    for (size_t i=0; i<sheet_loader->tile_size; i++)
    for (size_t j=0; j<sheet_loader->tile_size; j++)
    {
        size_t k = index + i + (j * sheet_loader->tile_size);
        if (pixels[k].a <= alpha_test) continue;

        float dx = delta * (float)i;
        float dy = delta * (float)j;
        struct Vec3 pixel_origin = vec3_add(origin, vec3_scalar_mult(forward, dx));
        pixel_origin = vec3_add(pixel_origin, vec3_scalar_mult(right, dy));
        push_pixel_voxel(color_init(pixels[k]), pixel_origin, forward, right, up, scale);
    }
}

}   // Animations
